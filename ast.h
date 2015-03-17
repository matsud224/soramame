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
//�p�[�X�������ɓ��������l�ߍ���ł���
class CodegenInfo{
public:
    map<string,OperatorInfo *> OperatorList;
    vector<FunctionAST *> FunctionList; //�g�b�v���x���̊֐��܂��͑g�ݍ��݊֐��������ɓo�^�����
    ConstantPool PublicConstantPool;
    int main_poolindex; //main�֐��̃R���X�^���g�v�[���E�C���f�b�N�X

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
    ExprAST *ParsePrimary();
    ExprAST *ParseIntValExpr();
    ExprAST *ParseBoolValExpr();
    ExprAST *ParseIdentifierExpr();
    ExprAST *ParseFloatValExpr();
    ExprAST *ParseParenExpr();
    ExprAST *ParseUnary();
    ExprAST *ParseExpression();
    ExprAST *ParseClosureExpr();
    ExprAST *ParseStringValExpr();
    ExprAST *ParseReturnExpr();

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
    //�֐��^�̈����E�߂�l�̌^�̃��X�g��Ԃ��܂��i�֐��^�łȂ��ꍇ��null�j
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
        //�R���X�^���g�v�[���ւ̓o�^
        poolindex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(this));
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
    int localindex;//���[�J���ϐ��̃C���f�b�N�X�ԍ��i�t���[���z��̃C���f�b�N�X�j

    VariableExprAST(const string &name):Name(name){type=NULL;}
    const string &GetName() {return Name;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        if(type!=NULL){
            return type;
        }

        //���O����v������̂����邩��ʂ̃t���[���ɑk��Ȃ���T��
        bool found=false;
        int currentflame;
        unsigned int i;
        localindex=-1;
        for(currentflame=(static_cast<int>(env->size())-1);currentflame>=0;currentflame--){
            for(i=0;i<(*env)[currentflame]->size();i++){
                localindex++;
                if(Name==(*(*env)[currentflame])[i].first){
                    //���O����v
                    type=(*(*env)[currentflame])[i].second; //���g�̌^������
                    cout<<type->GetName()<<endl;
                    found=true;
                    goto out_for;
                }
            }
        }
        out_for:
        if(!found){
            error("�ϐ�:"+Name+"�͖���`�܂��̓X�R�[�v�O�ł��B");
        }

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
        //�z�h�~�̈�
        if(type!=NULL){
            return type;
        }

        TypeAST *oprandt=operand->CheckType(env,geninfo);
        if(opcode=="!"){
            if(oprandt->GetName()!="bool"){
                error("�^�ɖ�肪����܂��B�P�����Z�q:"+opcode+" �I�y�����h:"+oprandt->GetName());
            }
            type=oprandt;
        }

        if(type==NULL){
            error("���m�̒P�����Z�q�ł�:"+opcode);
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

        //�g�ݍ��݌^�̌^�`�F�b�N
        if(opcode=="+" || opcode=="-" || opcode=="*" || opcode=="/"){
            if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float")){
                error("�^�ɖ�肪����܂��B�񍀉��Z�q:"+opcode+" ����:"+lhst->GetName()+" �E��:"+rhst->GetName());
            }

            type=lhst; //�I�y�����h�̌^�����Ɏ���̌^�����߂�
        }else if(opcode=="%" || opcode=="<<" || opcode==">>"){
            if(*lhst!=*rhst || (lhst->GetName()!="int")){
                error("�^�ɖ�肪����܂��B�񍀉��Z�q:"+opcode+" ����:"+lhst->GetName()+" �E��:"+rhst->GetName());
            }
            type=lhst;
        }else if(opcode=="&&" || opcode=="||"){
            if(*lhst!=*rhst || (lhst->GetName()!="bool")){
                error("�^�ɖ�肪����܂��B�񍀉��Z�q:"+opcode+" ����:"+lhst->GetName()+" �E��:"+rhst->GetName());
            }
            type=lhst;
        }else if(opcode=="<" || opcode==">" || opcode=="<=" || opcode==">="){
            if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float")){
                error("�^�ɖ�肪����܂��B�񍀉��Z�q:"+opcode+" ����:"+lhst->GetName()+" �E��:"+rhst->GetName());
            }
            type=new TypeAST("bool");
        }else if(opcode=="==" || opcode=="!="){
            if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float" && lhst->GetName()!="bool")){
                error("�^�ɖ�肪����܂��B�񍀉��Z�q:"+opcode+" ����:"+lhst->GetName()+" �E��:"+rhst->GetName());
            }
            type=new TypeAST("bool");
        }

        if(type==NULL){
            error("���m�̓񍀉��Z�q�ł�:"+opcode);
        }

        return type;
    }
};

class ReturnExpr : public ExprAST{
private:
    ExprAST *expression;
public:
    TypeAST *GetType(){return type;}
    ReturnExpr(ExprAST *exp):expression(exp){};
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


class FunctionAST : public ExprAST{
private:
    string name;
public:
    vector< pair<string,TypeAST *> > Args;
    vector<int> bytecodes;
    int poolindex;
private:
    TypeAST *type;
    vector<ExprAST *> body;

    bool isBuiltin;
public:
    string GetName(){return name;}
    vector<ExprAST *> &GetBody(){return body;}

    //�P�Ɏ���̌^��Ԃ�
    TypeAST *GetType(){

        return type;
    }

    //����̌^��Ԃ������łȂ��Abody�ɂ��Č^���������{����
    TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        vector< pair<string,TypeAST *> > *fflame=new vector< pair<string,TypeAST *> >();
        //���������ɒǉ�
        vector< pair<string,TypeAST *> >::iterator argiter;
        for(argiter=Args.begin();argiter!=Args.end();argiter++){
            fflame->push_back(*argiter);
        }
        env->push_back(fflame);
        vector<ExprAST *>::iterator iter2;
        for(iter2=body.begin();iter2!=body.end();iter2++){
            (*iter2)->CheckType(env,geninfo);

            //return���̏ꍇ���ʈ����B�Ԃ��l�̌^�Ƃ��̊֐��̕Ԃ�l�̌^���s��v�Ȃ�΃G���[
            if(typeid(*(*iter2))==typeid(ReturnExpr)){
                if(type->ParseFunctionType().back()->GetName() != dynamic_cast<ReturnExpr *>(*iter2)->GetType()->GetName()){
                    error("'return����l�̌^'�ƁA'�֐��̖߂�l�̌^'����v���܂���B");
                }
            }
        }
        delete env->back();
        env->pop_back(); //�Ō���̃t���[�����폜


        return type; //����̌^���ꉞ�Ԃ��Ă���
    }

    FunctionAST(CodegenInfo *cgi,string &n,vector< pair<string,TypeAST *> > &a,TypeAST *t,vector<ExprAST *> &bdy):name(n),Args(a),type(t),body(bdy){
        isBuiltin=false;
        if(t->ParseFunctionType().empty()){
            error("�֐��ɑ΂��Ċ֐��^�ł͂Ȃ��^���t�^����܂����B");
        }
        poolindex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(this));
    }

    //�g�ݍ��݊֐��p�̃R���X�g���N�^
    FunctionAST(CodegenInfo *cgi,string n,vector< pair<string,TypeAST *> > *a,TypeAST *t):name(n),Args(*a),type(t){
        isBuiltin=true;
        if(t->ParseFunctionType().empty()){
            error("�֐��ɑ΂��Ċ֐��^�ł͂Ȃ��^���t�^����܂����B");
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
public:
    CallExprAST(const string &callee_func,vector<ExprAST *> &args_list):callee(callee_func),args(args_list){type=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
        //�z�h�~�̈�
        if(type!=NULL){
            return type;
        }

        //�����̌^�����Ɍ��߂Ă���
        vector<ExprAST *>::iterator iter2;
        for(iter2=args.begin();iter2!=args.end();iter2++){
            (*iter2)->CheckType(env,geninfo);
        }

        //���O����v������̂����邩��ʂ̃t���[���ɑk��Ȃ���T��
        bool found=false;
        int currentflame;
        unsigned int i;
        localindex=-1;
        for(currentflame=(static_cast<int>(env->size())-1);currentflame>=0;currentflame--){
            for(i=0;i<(*env)[currentflame]->size();i++){
                localindex++;
                if(callee==(*(*env)[currentflame])[i].first){
                    //���O����v
                    found=true;
                    vector<TypeAST *> currentarg=(*(*env)[currentflame])[i].second->ParseFunctionType();
                    if(args.size()+1==currentarg.size()){ //+1����̂́Aargs�ɂ͖߂�l�̌^���܂܂�Ă��Ȃ�����
                        for(unsigned int j=0;j<args.size();j++){
                            if(*(args[j]->CheckType(NULL,geninfo)) != *(currentarg[j])){
                                goto nextfunc1;
                            }
                        }
                        type=currentarg.back(); //�֐��̌^�ł͂Ȃ��֐��̖߂�l�̌^����
                        return type;
                    }
                }

                nextfunc1:
                if(type==NULL){}; //���x���̌�Ɏ����Ȃ��ƃG���[�ƂȂ邽�߁A���Ӗ��Ȏ����ЂƂ�
            }
        }

        if(found==true){
            error("�����̌^����v���܂���:"+callee);
        }else{
            error("����`�܂��̓X�R�[�v�O�̊֐��ł�:"+callee);
        }

        return NULL;
    }
};


//���ԏ�A���S���Y���ł̂ݎg�p
class OperatorAST : public ExprAST{
private:
    string name;
public:
    OperatorAST(string &n):name(n){};
    string GetName(){return name;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){}
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return NULL;}
};
