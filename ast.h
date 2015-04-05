#pragma once
#include <string>
#include <vector>
#include <map>
#include "utility.h"
#include "parser.h"
#include "lexer.h"
#include "vm.h"
#include "color_text.h"

using namespace std;


class OperatorInfo;
class TypeAST;
class ExprAST;
class FunctionAST;
class Flame;
class VariableDefStatementAST;
class TopLevelItem;
class BlockAST;
class ClosureObject;


class TypeAST{
public:
	virtual ~TypeAST(){}
	virtual string GetName()=0;

	virtual bool operator == (TypeAST &obj){return this->GetName()==obj.GetName();};
	virtual bool operator != (TypeAST &obj){return this->GetName()!=obj.GetName();};
};

//単一の型
class BasicTypeAST : public TypeAST{
public:
    BasicTypeAST(string n):Name(n){} //普通の型用
    string Name; //関数型のためにvectorにしてある
    virtual string GetName(){
		return Name;
    }
};

//関数型
class FunctionTypeAST : public TypeAST{
public:
	FunctionTypeAST(vector<TypeAST*> t_list):TypeList(t_list){}
	vector<TypeAST*> TypeList;
	virtual string GetName(){
		string s="(";
		for(int i=0;i<TypeList.size()-1;i++){
			s+=TypeList[i]->GetName();
			if(i!=TypeList.size()-2){
				s+=",";
			}
		}
		s+=")=>"+TypeList[TypeList.size()-1]->GetName();
		return s;
	}

};

class ExprAST{
public:
	TypeAST *TypeInfo;

    virtual ~ExprAST(){}
    virtual bool IsBuilt(){return true;}; //ASTが構築されたか
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo)=0;
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo)=0;
    virtual vector<int> FindChildFunction()=0;
};

class UnBuiltExprAST : public ExprAST{
public:
	vector<ExprAST*> *ExprList;
    UnBuiltExprAST(vector<ExprAST*> *val):ExprList(val){TypeInfo=NULL;}
    virtual bool IsBuilt(){return false;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){};
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return TypeInfo;}
    ExprAST *BuildAST(CodegenInfo*);
    virtual vector<int> FindChildFunction();
};

class IntValExprAST : public ExprAST{
public:
	int Value;
    IntValExprAST(int val):Value(val){TypeInfo=new BasicTypeAST("int");}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
};

class BoolValExprAST : public ExprAST{
public:
	bool Value;
    BoolValExprAST(bool val):Value(val){TypeInfo=new BasicTypeAST("bool");}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
};

class StringValExprAST : public ExprAST{
public:
    string Value;
    int PoolIndex;
    StringValExprAST(CodegenInfo *cgi,string val):Value(val){
        TypeInfo=new BasicTypeAST("string");
        //コンスタントプールへの登録
        PoolIndex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(&Value));
        cout<<"#"<<PoolIndex<<" : <string>"<<Value<<endl;
    }
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
};

//操車場アルゴリズムのために
class OperatorAST : public ExprAST{
public:
    string Operator;

    OperatorAST(string n):Operator(n){};
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){}
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){return NULL;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
};

class FunctionAST : public ExprAST{
public:
    vector< pair<string,TypeAST *> > *Args;
	Flame *ParentFlame; //クロージャの場合、生成元のフレームを覚えておく（実行時にVMが使用）
    vector<int> bytecodes;
    int PoolIndex;
    vector<int> *ChildPoolIndex; //生成したクロージャのコンスタントプールのインデックス
    bool isBuiltin;
	string Name;
	BlockAST *Body;

    //自らの型を返すだけでなく、bodyについて型検査を実施する
    TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    vector<int> FindChildFunction(); //関数内で作られるクロージャを探し、ChildPoolIndexへ登録します。

    FunctionAST(CodegenInfo *cgi,string n,vector< pair<string,TypeAST *> > *a,TypeAST *rett,BlockAST *bdy):Name(n),Args(a),Body(bdy){
		vector<TypeAST*> typelist;
		vector< pair<string,TypeAST *> >::iterator iter;
		for(iter=a->begin();iter!=a->end();iter++){
			typelist.push_back((*iter).second);
		}
		typelist.push_back(rett);

		TypeInfo=new FunctionTypeAST(typelist);
        isBuiltin=false;
        PoolIndex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(this));
        cout<<"#"<<PoolIndex<<" : <closure>"<<Name<<endl;
    }

    //組み込み関数用のコンストラクタ
    FunctionAST(CodegenInfo *cgi,string n,vector< pair<string,TypeAST *> > *a,TypeAST *rett):Name(n),Args(a){
		vector<TypeAST*> typelist;
		vector< pair<string,TypeAST *> >::iterator iter;
		for(iter=a->begin();iter!=a->end();iter++){
			typelist.push_back((*iter).second);
		}
		typelist.push_back(rett);
		TypeInfo=new FunctionTypeAST(typelist);
        isBuiltin=true;
        PoolIndex=cgi->PublicConstantPool.SetReference(reinterpret_cast<void *>(this));
        cout<<"#"<<PoolIndex<<" : <closure>"<<Name<<endl;
    }

    virtual void Codegen(vector<int> *unused_argumnt,CodegenInfo *geninfo);
};


class VariableExprAST : public ExprAST{
public:
    string Name;
    int LocalIndex;//ローカル変数のインデックス番号（フレーム配列のインデックス）

    VariableExprAST(const string &name):Name(name){TypeInfo=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction(){return vector<int>();};
};

class CallExprAST : public ExprAST{
public:
    string callee;
    vector<ExprAST *> *args;
    int PoolIndex;
    int LocalIndex;
    bool isBuiltin;

    CallExprAST(const string &callee_func,vector<ExprAST *> *args_list):callee(callee_func),args(args_list){TypeInfo=NULL;PoolIndex=-1;LocalIndex=-1;}
    CallExprAST(int poolidx,vector<ExprAST *> *args_list):PoolIndex(poolidx),args(args_list){TypeInfo=NULL;callee="<closure>";LocalIndex=-1;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction();
};

class UnaryExprAST : public ExprAST{
public:
    string Operator;
    ExprAST *Operand;

    UnaryExprAST(string opc,ExprAST *oprnd):Operator(opc),Operand(oprnd){TypeInfo=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction(){return vector<int>();}; //AST未構築なので呼び出されることはない。
};

class BinaryExprAST : public ExprAST{
public:
	string Operator;
    ExprAST *LHS,*RHS;

    BinaryExprAST(string opc,ExprAST *lhs,ExprAST *rhs):Operator(opc),LHS(lhs),RHS(rhs){TypeInfo=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction(){return vector<int>();}; //AST未構築なので呼び出されることはない。
};



class StatementAST{
public:
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo)=0;
    virtual void CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo)=0;
	virtual vector<int> FindChildFunction()=0;
};

class IfStatementAST : public StatementAST{
public:
    ExprAST *Condition;
    BlockAST *ThenBody;
    BlockAST *ElseBody;

    IfStatementAST(ExprAST *cond,BlockAST *thenbody,BlockAST *elsebody):Condition(cond),ThenBody(thenbody),ElseBody(elsebody){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction();
};

class ReturnStatementAST : public StatementAST{
public:
    ExprAST *Expression;

    ReturnStatementAST(ExprAST *exp):Expression(exp){};
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction();
};

class VariableDefStatementAST : public StatementAST{
public:
    pair<string,TypeAST *> *Variable;
    ExprAST* InitialValue; //初期値（初期値が未設定の時はNULL）
    int LocalIndex;

    VariableDefStatementAST(pair<string,TypeAST *> *var,ExprAST* initval):Variable(var),InitialValue(initval){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction();
};

class ExpressionStatementAST : public StatementAST{
public:
    ExprAST *Expression;

    ExpressionStatementAST(ExprAST* evalexpr):Expression(evalexpr){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    virtual vector<int> FindChildFunction();
};

class BlockAST{
public:
	vector<StatementAST *> *Body;
	vector< pair<string,TypeAST *> > *LocalVariables;

	BlockAST(vector<StatementAST *> *body,vector< pair<string,TypeAST *> > *local):Body(body),LocalVariables(local){}

	void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
	void CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo);
    vector<int> FindChildFunction();
};
