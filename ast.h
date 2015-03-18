#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "lexer.h"
#include "utility.h"
#include <typeinfo>


using namespace std;

enum{Left,Right};
enum{Unary,Binary};

class OperatorInfo;
class TypeAST;
class ExprAST;
class FunctionAST;
class Flame;
class VariableDefineExprAST;

class ConstantPool{
private:
    vector<void *> refpool;
public:
    int SetReference(void *item){
        refpool.push_back(item);
        return refpool.size()-1;
    }
    void *GetReference(int index){
        return refpool[index];
    }
    unsigned int GetPoolSize(){
        return refpool.size();
    }
    void CLear();
};
//パースした時に得た情報を詰め込んでおく
class CodegenInfo{
public:
    map<string,OperatorInfo *> OperatorList;
    vector<FunctionAST *> FunctionList; //トップレベルの関数または組み込み関数がここに登録される
    ConstantPool PublicConstantPool;
    int main_poolindex; //main関数のコンスタントプール・インデックス

    void Initialize(){
        OperatorList.clear();
        FunctionList.clear();
        PublicConstantPool.CLear();
        return;
    }
};



class Parser{
public:
    Lexer *lexer;
    CodegenInfo genInfo;
    FunctionAST *ParseFunction();
    TypeAST *ParseType();
    ExprAST *ParsePrimary(vector<int> *);
    ExprAST *ParseIntValExpr();
    ExprAST *ParseBoolValExpr();
    ExprAST *ParseIdentifierExpr(vector<int> *);
    ExprAST *ParseFloatValExpr();
    ExprAST *ParseParenExpr(vector<int> *);
    ExprAST *ParseUnary();
    ExprAST *ParseExpression(vector<int> *);
    ExprAST *ParseClosureExpr(vector<int> *);
    ExprAST *ParseStringValExpr();
    ExprAST *ParseReturnExpr(vector<int> *);
    ExprAST *ParseVariableDefineExpr();

    Parser(Lexer *l):lexer(l){}
    void Parse();
    void TypeCheck();
    void Codegen();
    CodegenInfo GetCGInfo(){return genInfo;}
};

class OperatorInfo{
private:
    int unaryorbinary;
    int associativity;
    int precedence;
    //TypeAST lhstype,rhstype,rettype;
public:
    OperatorInfo(int uorb,int assoc,int prec=30):unaryorbinary(uorb),associativity(assoc),precedence(prec){}
    int GetAssociativity(){return associativity;}
    int GetPrecedence(){return precedence;}
    int GetUnaryOrBinary(){return unaryorbinary;}
};

class TypeAST{
private:
    string name;
public:
    TypeAST(const string &n):name(n){}
    string &GetName() {return name;}
    bool operator == (TypeAST another){return name==another.name;}
    bool operator != (TypeAST another){return !(*this==another);}
    //関数型の引数・戻り値の型のリストを返します（関数型でない場合はnull）
    vector<TypeAST *> ParseFunctionType(){
        //cout<<name<<endl;
        vector<TypeAST *> ret;
        Lexer l(name);
        Parser p(&l);
        if(l.GetNextToken(),l.CurrentToken=='('){
            l.GetNextToken();
            while(l.CurrentToken!=')' || l.CurrentToken==token_eof){
                ret.push_back(p.ParseType());
                //cout<<ret.back()->GetName()<<endl;
                if(l.CurrentToken==','){
                    l.GetNextToken();
                }
            }
            if(l.GetNextToken(),l.CurrentToken==token_operator){
                if(l.CurrentOperator=="=>"){
                    l.GetNextToken();
                    ret.push_back(p.ParseType());
                    //cout<<ret.back()->GetName()<<endl;
                    return ret;
                }
            }
        }
        ret.clear();
        return ret;
    }

};

class ExprAST{
protected:
    TypeAST *type;
public:
    virtual ~ExprAST(){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo)=0;
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo)=0;
};

class BoolValExprAST : public ExprAST{
private:
    bool value;
public:
    bool GetValue(){return value;}
    BoolValExprAST(bool val):value(val){type=new TypeAST("bool");}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return type;}
};

class StringValExprAST : public ExprAST{
private:
    string value;
public:
    int poolindex;
    string GetValue(){return value;}
    StringValExprAST(CodegenInfo *cgi,string val):value(val){
        type=new TypeAST("string");
        //コンスタントプールへの登録
        poolindex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(&value));
    }
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return type;}
};

class IntValExprAST : public ExprAST{
private:
    int value;
public:
    int GetValue(){return value;}
    IntValExprAST(int val):value(val){type=new TypeAST("int");}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return type;}
};

class FloatValExprAST : public ExprAST{
private:
    float value;
public:
    float GetValue(){return value;}
    FloatValExprAST(float val):value(val){type=new TypeAST("float");}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return type;}
};

class VariableExprAST : public ExprAST{
private:
    string Name;
public:
    int localindex;//ローカル変数のインデックス番号（フレーム配列のインデックス）

    VariableExprAST(const string &name):Name(name){type=NULL;}
    const string &GetName() {return Name;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        if(type!=NULL){
            return type;
        }

        //名前が一致するものがあるか上位のフレームに遡りながら探す
        bool found=false;
        int currentflame;
        unsigned int i;
        localindex=-1;
        for(currentflame=(static_cast<int>(env->size())-1);currentflame>=0;currentflame--){
            for(i=0;i<(*env)[currentflame]->size();i++){
                localindex++;
                if(Name==(*(*env)[currentflame])[i].first){
                    //名前が一致
                    type=(*(*env)[currentflame])[i].second; //自身の型を決定
                    found=true;
                    goto out_for;
                }
            }
        }
        out_for:
        if(!found){
            error("変数:"+Name+"は未定義またはスコープ外です。");
        }
        //cout<<Name<<":"<<localindex<<endl;
        return type;
    }
};

class UnaryExprAST : public ExprAST{
private:
    string opcode;
    ExprAST *operand;
public:
    UnaryExprAST(string opc,ExprAST *oprnd):opcode(opc),operand(oprnd){type=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        //循環防止の為
        if(type!=NULL){
            return type;
        }

        TypeAST *oprandt=operand->CheckType(env,geninfo);
        if(opcode=="!"){
            if(oprandt->GetName()!="bool"){
                error("型に問題があります。単項演算子:"+opcode+" オペランド:"+oprandt->GetName());
            }
            type=oprandt;
        }

        if(type==NULL){
            error("未知の単項演算子です:"+opcode);
        }

        return type;
    }
};

class BinaryExprAST : public ExprAST{
private:
    string opcode;
    ExprAST *LHS,*RHS;
public:
    BinaryExprAST(string opc,ExprAST *lhs,ExprAST *rhs):opcode(opc),LHS(lhs),RHS(rhs){type=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        TypeAST *lhst=LHS->CheckType(env,geninfo);
        TypeAST *rhst=RHS->CheckType(env,geninfo);
        type=NULL;

        //組み込み型の型チェック
        if(opcode=="+" || opcode=="-" || opcode=="*" || opcode=="/"){
            if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float")){
                error("型に問題があります。二項演算子:"+opcode+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
            }

            type=lhst; //オペランドの型を元に自らの型を決める
        }else if(opcode=="%" || opcode=="<<" || opcode==">>"){
            if(*lhst!=*rhst || (lhst->GetName()!="int")){
                error("型に問題があります。二項演算子:"+opcode+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
            }
            type=lhst;
        }else if(opcode=="&&" || opcode=="||"){
            if(*lhst!=*rhst || (lhst->GetName()!="bool")){
                error("型に問題があります。二項演算子:"+opcode+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
            }
            type=lhst;
        }else if(opcode=="<" || opcode==">" || opcode=="<=" || opcode==">="){
            if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float")){
                error("型に問題があります。二項演算子:"+opcode+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
            }
            type=new TypeAST("bool");
        }else if(opcode=="==" || opcode=="!="){
            if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float" && lhst->GetName()!="bool")){
                error("型に問題があります。二項演算子:"+opcode+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
            }
            type=new TypeAST("bool");
        }else if(opcode=="="){
			if(*lhst!=*rhst){
                error("型に問題があります。二項演算子:"+opcode+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
            }else if(typeid(*LHS)!=typeid(VariableExprAST)){
				error("代入式の左辺が変数ではありません。");
            }
            type=lhst;
        }

        if(type==NULL){
            error("未知の二項演算子です:"+opcode);
        }

        return type;
    }
};

class ReturnExprAST : public ExprAST{
private:
    ExprAST *expression;
public:
    TypeAST *GetType(){return type;}
    ReturnExprAST(ExprAST *exp):expression(exp){};
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        if(expression==NULL){
            type=new TypeAST("void");
        }else{
            type=expression->CheckType(env,geninfo);
        }
        return type;
    };
};

class VariableDefineExprAST : public ExprAST{
public:
    vector< pair<string,TypeAST *> > *Variables;
    TypeAST *GetType(){return type;}
    VariableDefineExprAST(vector< pair<string,TypeAST *> > *vars):Variables(vars){type=new TypeAST("void");}

    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){}; //今のところ代入式は併記できないのでコード生成は行わない

    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        return type;
    };
};



class FunctionAST : public ExprAST{
private:
    string name;
public:
    vector< pair<string,TypeAST *> > Args;
    vector< pair<string,TypeAST *> > LocalVariables;
	Flame *ParentFlame; //クロージャの場合、生成元のフレームを覚えておく（実行時にVMが使用）
    vector<int> bytecodes;
    int poolindex;
private:
    TypeAST *type;
    vector<ExprAST *> body;


public:

    vector<int> *ChildPoolIndex; //生成したクロージャのコンスタントプールのインデックス
    bool isBuiltin;

    string GetName(){return name;}
    vector<ExprAST *> &GetBody(){return body;}

    //単に自らの型を返す
    TypeAST *GetType(){

        return type;
    }

    //自らの型を返すだけでなく、bodyについて型検査を実施する
    TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        vector< pair<string,TypeAST *> > *fflame=new vector< pair<string,TypeAST *> >();
        //引数を環境に追加
        vector< pair<string,TypeAST *> >::iterator argiter;
        for(argiter=Args.begin();argiter!=Args.end();argiter++){
            fflame->push_back(*argiter);
        }
        env->push_back(fflame);
        vector<ExprAST *>::iterator iter2;
        for(iter2=body.begin();iter2!=body.end();iter2++){
            (*iter2)->CheckType(env,geninfo);

            if(typeid(*(*iter2))==typeid(VariableDefineExprAST)){
				//変数宣言を見つけたら変数リストへ随時追加していく
				LocalVariables.insert(LocalVariables.end(),dynamic_cast<VariableDefineExprAST *>(*iter2)->Variables->begin(),dynamic_cast<VariableDefineExprAST *>(*iter2)->Variables->end());
				env->back()->insert(env->back()->end(),dynamic_cast<VariableDefineExprAST *>(*iter2)->Variables->begin(),dynamic_cast<VariableDefineExprAST *>(*iter2)->Variables->end());
            }else if(typeid(*(*iter2))==typeid(ReturnExprAST)){
            	//return文の場合特別扱い。返す値の型とこの関数の返り値の型が不一致ならばエラー
                if(type->ParseFunctionType().back()->GetName() != dynamic_cast<ReturnExprAST *>(*iter2)->GetType()->GetName()){
                    error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
                }
            }
        }

        delete env->back();
        env->pop_back(); //最後尾のフレームを削除


        return type; //自らの型を一応返しておく
    }

    FunctionAST(CodegenInfo *cgi,string &n,vector< pair<string,TypeAST *> > &a,TypeAST *t,vector<ExprAST *> &bdy,vector<int> *cpi):name(n),Args(a),type(t),body(bdy),ChildPoolIndex(cpi){
        isBuiltin=false;
        if(t->ParseFunctionType().empty()){
            error("関数に対して関数型ではない型が付与されました。");
        }
        poolindex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(this));
    }

    //組み込み関数用のコンストラクタ
    FunctionAST(CodegenInfo *cgi,string n,vector< pair<string,TypeAST *> > *a,TypeAST *t):name(n),Args(*a),type(t){
        isBuiltin=true;
        if(t->ParseFunctionType().empty()){
            error("関数に対して関数型ではない型が付与されました。");
        }
        poolindex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(this));
    }

    virtual void Codegen(vector<int> *unused_argumnt,CodegenInfo *geninfo);
    bool isBuiltinFunction(){return isBuiltin;}
};



class CallExprAST : public ExprAST{
private:
    string callee;
    vector<ExprAST *> args;
    int localindex;
    bool isBuiltin;
public:
    CallExprAST(const string &callee_func,vector<ExprAST *> &args_list):callee(callee_func),args(args_list){type=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        //循環防止の為
        if(type!=NULL){
            return type;
        }

        //引数の型を順に決めていく
        vector<ExprAST *>::iterator iter2;
        for(iter2=args.begin();iter2!=args.end();iter2++){
            (*iter2)->CheckType(env,geninfo);
        }

        //名前が一致するものがあるか上位のフレームに遡りながら探す
        bool found=false;
        int currentflame;
        unsigned int i;
        localindex=-1;
        for(currentflame=(static_cast<int>(env->size())-1);currentflame>=0;currentflame--){
            for(i=0;i<(*env)[currentflame]->size();i++){
                localindex++;
                if(callee==(*(*env)[currentflame])[i].first){
                    //名前が一致
                    found=true;
                    vector<TypeAST *> currentarg=(*(*env)[currentflame])[i].second->ParseFunctionType();
                    if(args.size()+1==currentarg.size()){ //+1するのは、argsには戻り値の型が含まれていないから
                        for(unsigned int j=0;j<args.size();j++){
                            if(*(args[j]->CheckType(NULL,geninfo)) != *(currentarg[j])){
                                goto nextfunc1;
                            }
                        }
                        type=currentarg.back(); //関数の型ではなく関数の戻り値の型を代入
                        if(currentflame==0){
                            //トップレベルに関数がある場合はビルトイン関数を疑う
                            for(unsigned int k=0;k<geninfo->FunctionList.size();k++){
                                if(geninfo->FunctionList[k]->GetName()==callee){
                                    isBuiltin=geninfo->FunctionList[k]->isBuiltin;
                                }
                            }
                        }
                        return type;
                    }
                }

                nextfunc1:
                if(type==NULL){}; //ラベルの後に式がないとエラーとなるため、無意味な式をひとつ
            }
        }

        if(found==true){
            error("引数の型が一致しません:"+callee);
        }else{
            error("未定義またはスコープ外の関数です:"+callee);
        }

        return NULL;
    }
};



//操車場アルゴリズムでのみ使用
class OperatorAST : public ExprAST{
private:
    string name;
public:
    OperatorAST(string &n):name(n){};
    string GetName(){return name;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){}
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return NULL;}
};
