#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include "utility.h"
#include "parser.h"
#include "lexer.h"
#include "vm.h"
#include "color_text.h"
#include "basic_object.h"

using namespace std;


class OperatorInfo;
class TypeAST;
class ExprAST;
class FunctionAST;
class Flame;
class VariableDefStatementAST;
class TopLevelItem;
class BlockAST;


class EnvItem{
public:
	pair<string,TypeAST*> VariableInfo;
	int LocalIndex;
};

class Environment{
public:
	vector< EnvItem > Items;
	bool is_internalblock; //関数内部のブロック（if,whileなど）か？
	vector< pair<string,TypeAST *> > *LocalVariablesPtr;

	//Environment(bool is_internal,vector< pair<string,TypeAST *> > *localvalsptr):is_internalblock(is_internal),LocalVariablesPtr(localvalsptr){};
};


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
    BasicTypeAST(string n):Name(n){}
    string Name;
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
		string s="fun(";
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

//リスト型
class ListTypeAST : public TypeAST{
public:
    ListTypeAST(TypeAST *t):ContainType(t){}
    TypeAST* ContainType;
    virtual string GetName(){
		return "["+ContainType->GetName()+"]";
    }
};

//タプル型
class TupleTypeAST : public TypeAST{
public:
    TupleTypeAST(vector<TypeAST*> t_list):ContainTypeList(t_list){}
    vector<TypeAST*> ContainTypeList;
    virtual string GetName(){
		string name="(";
		for(int i=0;i<ContainTypeList.size();i++){
			name+=ContainTypeList[i]->GetName();
			if(i!=ContainTypeList.size()-1){
				name+=",";
			}
		}
		name+=")";
		return name;
    }
};

class ExprAST{
public:
	TypeAST *TypeInfo;

    virtual ~ExprAST(){}
    virtual bool IsBuilt(){return true;}; //ASTが構築されたか
    virtual bool IsConstant()=0; //定数か
    virtual int GetVMValue(CodegenInfo*)=0; //スタックに置かれるintの値（整数値なら整数、他ならコンスタントプールのインデックス）
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo)=0;
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars)=0;
    virtual vector<int> FindChildFunction()=0;
    virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index)=0;
	virtual vector<ExprAST*> GetCallExprList()=0;
};

class UnBuiltExprAST : public ExprAST{
public:
	vector<ExprAST*> *ExprList;
    UnBuiltExprAST(vector<ExprAST*> *val):ExprList(val){TypeInfo=NULL;}
    virtual bool IsBuilt(){return false;}
    virtual bool IsConstant(){return false;}
    int GetVMValue(CodegenInfo *cgi){return -1;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){};
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){return TypeInfo;}
    ExprAST *BuildAST(CodegenInfo*);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){return false;}
	virtual vector<ExprAST*> GetCallExprList();
};

class IntValExprAST : public ExprAST{
public:
	int Value;
    IntValExprAST(int val):Value(val){TypeInfo=new BasicTypeAST("int");}
    virtual bool IsConstant(){return true;}
    int GetVMValue(CodegenInfo *cgi){return Value;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
    virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){return true;};
    virtual vector<ExprAST*> GetCallExprList();
};

class BoolValExprAST : public ExprAST{
public:
	bool Value;
    BoolValExprAST(bool val):Value(val){TypeInfo=new BasicTypeAST("bool");}
    virtual bool IsConstant(){return true;}
    int GetVMValue(CodegenInfo *cgi){return Value;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
	virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){return true;}
	virtual vector<ExprAST*> GetCallExprList();
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
    virtual bool IsConstant(){return true;}
    int GetVMValue(CodegenInfo *cgi){return PoolIndex;}
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
    virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){return true;};
    virtual vector<ExprAST*> GetCallExprList();
};

class ListValExprAST : public ExprAST{
public:
    list<ExprAST*> *Value;
    int PoolIndex;
    ListValExprAST(CodegenInfo *cgi,list<ExprAST*> *val):Value(val){
        TypeInfo=NULL;
        PoolIndex=-1;
    }
    virtual bool IsConstant(){
		list<ExprAST*>::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			if((*iter)->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    int GetVMValue(CodegenInfo *cgi){return PoolIndex;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo*,int);
    virtual vector<ExprAST*> GetCallExprList();
};

class TupleValExprAST : public ExprAST{
public:
    list<ExprAST*> *Value;
    int PoolIndex;
    TupleValExprAST(CodegenInfo *cgi,list<ExprAST*> *val):Value(val){
        TypeInfo=NULL;
        PoolIndex=-1;
    }
    virtual bool IsConstant(){
		list<ExprAST*>::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			if((*iter)->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    int GetVMValue(CodegenInfo *cgi){return PoolIndex;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo*,int);
    virtual vector<ExprAST*> GetCallExprList();
};

class DataValExprAST : public ExprAST{
public:
    vector< pair<string,ExprAST*> > *InitValue;

    int PoolIndex;
    DataValExprAST(CodegenInfo *cgi,string TypeName){
        TypeInfo=new BasicTypeAST(TypeName);
        PoolIndex=-1;
    }
    virtual bool IsConstant(){
		vector< pair<string,ExprAST*> >::iterator iter;
		for(iter=InitValue->begin();iter!=InitValue->end();iter++){
			if((*iter).second->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    int GetVMValue(CodegenInfo *cgi){return PoolIndex;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo*,int);
    virtual vector<ExprAST*> GetCallExprList();
};

//操車場アルゴリズムのために
class OperatorAST : public ExprAST{
public:
    string Operator;

    OperatorAST(string n):Operator(n){};
    virtual bool IsConstant(){return true;}
    int GetVMValue(CodegenInfo *cgi){return -1;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo){}
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){return NULL;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
	virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){return false;}
	virtual vector<ExprAST*> GetCallExprList();
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
	vector< pair<string,TypeAST *> > *LocalVariables; //Argsを含む
	bool is_builtin_CTFEable;
	virtual bool IsConstant(){return false;}
	int GetVMValue(CodegenInfo *cgi){
		return -1;
	}

    //自らの型を返すだけでなく、bodyについて型検査を実施する
    TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
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
    FunctionAST(CodegenInfo *cgi,string n,vector< pair<string,TypeAST *> > *a,TypeAST *rett,bool CTFEable):Name(n),Args(a),is_builtin_CTFEable(CTFEable){
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

    virtual bool IsCTFEable(CodegenInfo*,int);
    virtual vector<ExprAST*> GetCallExprList();
};


class VariableExprAST : public ExprAST{
public:
    string Name;
    int LocalIndex;//ローカル変数のインデックス番号（フレーム配列のインデックス）
    int FlameBack; //フレームを何回遡るか
    bool is_toplevel_func; //トップレベル関数または組み込み関数かどうか(CheckTypeで分かる)

    VariableExprAST(const string &name):Name(name){TypeInfo=NULL;}
    virtual bool IsConstant(){return false;}
    int GetVMValue(CodegenInfo *cgi){return -1;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction(){return vector<int>();};
    virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
    	if(FlameBack==0){return true;}
    	if(is_toplevel_func){
			if(LocalIndex==curr_fun_index){
				return true;
			}
			return cgi->TopLevelFunction[LocalIndex]->IsCTFEable(cgi,curr_fun_index);
    	}
    	return false;
	}
    virtual vector<ExprAST*> GetCallExprList();
};

class CallExprAST : public ExprAST{
public:
    ExprAST* callee;
    int CalculatedValue; //callee==NULLの時はこちらの計算結果を利用
    vector<ExprAST *> *args;

    CallExprAST(ExprAST* callee_func,vector<ExprAST *> *args_list):callee(callee_func),args(args_list){TypeInfo=NULL;}

    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual bool IsConstant(){
    	return (callee==NULL);
	}
    int GetVMValue(CodegenInfo *cgi){
    	if(callee==NULL){
			return CalculatedValue;
    	}else{
			return -1;
    	}
	}
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index);
    virtual vector<ExprAST*> GetCallExprList();
};

class UnaryExprAST : public ExprAST{
public:
    string Operator;
    ExprAST *Operand;

    UnaryExprAST(string opc,ExprAST *oprnd):Operator(opc),Operand(oprnd){TypeInfo=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual bool IsConstant(){return false;}
    int GetVMValue(CodegenInfo *cgi){return -1;}
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction(){return vector<int>();}; //AST未構築なので呼び出されることはない。
	virtual bool IsCTFEable(CodegenInfo *cgi,int curr_fun_index){return Operand->IsCTFEable(cgi,curr_fun_index)&&cgi->OperatorList[Operator]->IsCTFEable;}
	virtual vector<ExprAST*> GetCallExprList();
};

class BinaryExprAST : public ExprAST{
public:
	string Operator;
    ExprAST *LHS,*RHS;

    BinaryExprAST(string opc,ExprAST *lhs,ExprAST *rhs):Operator(opc),LHS(lhs),RHS(rhs){TypeInfo=NULL;}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual bool IsConstant(){return false;}
    int GetVMValue(CodegenInfo *cgi){return -1;}
    virtual TypeAST *CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction(){return vector<int>();}; //AST未構築なので呼び出されることはない。
    virtual bool IsCTFEable(CodegenInfo* cgi,int curr_fun_index){return LHS->IsCTFEable(cgi,curr_fun_index)&&RHS->IsCTFEable(cgi,curr_fun_index)&&cgi->OperatorList[Operator]->IsCTFEable;}
	virtual vector<ExprAST*> GetCallExprList();
};



class StatementAST{
public:
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo)=0;
    virtual void CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars)=0;
	virtual vector<int> FindChildFunction()=0;
	virtual bool IsCTFEable(CodegenInfo* cgi,int)=0;
	virtual vector<ExprAST*> GetCallExprList()=0;
};

class IfStatementAST : public StatementAST{
public:
    ExprAST *Condition;
    BlockAST *ThenBody;
    BlockAST *ElseBody;

    IfStatementAST(ExprAST *cond,BlockAST *thenbody,BlockAST *elsebody):Condition(cond),ThenBody(thenbody),ElseBody(elsebody){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo* cgi,int);
    virtual vector<ExprAST*> GetCallExprList();
};

class WhileStatementAST : public StatementAST{
public:
    ExprAST *Condition;
    BlockAST *Body;

    WhileStatementAST(ExprAST *cond,BlockAST *body):Condition(cond),Body(body){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo* cgi,int);
    virtual vector<ExprAST*> GetCallExprList();
};


class ReturnStatementAST : public StatementAST{
public:
    ExprAST *Expression;

    ReturnStatementAST(ExprAST *exp):Expression(exp){};
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo* cgi,int);
    virtual vector<ExprAST*> GetCallExprList();
};

class VariableDefStatementAST : public StatementAST{
public:
    pair<string,TypeAST *> *Variable;
    ExprAST* InitialValue; //初期値（初期値が未設定の時はNULL）
    int LocalIndex;
    int FlameBack; //常に0

    VariableDefStatementAST(pair<string,TypeAST *> *var,ExprAST* initval):Variable(var),InitialValue(initval){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo* cgi,int);
    virtual vector<ExprAST*> GetCallExprList();
};

class ExpressionStatementAST : public StatementAST{
public:
    ExprAST *Expression;

    ExpressionStatementAST(ExprAST* evalexpr):Expression(evalexpr){}
    virtual void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
    virtual void CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(CodegenInfo* cgi,int);
    virtual vector<ExprAST*> GetCallExprList();
};

class BlockAST{
public:
	vector<StatementAST *> *Body;

	BlockAST(vector<StatementAST *> *body):Body(body){}

	void Codegen(vector<int> *bytecodes,CodegenInfo *geninfo);
	void CheckType(vector<Environment> *env,CodegenInfo *geninfo,bool isinternalblock,vector< pair<string,TypeAST*> > *CurrentLocalVars);
    vector<int> FindChildFunction();
    bool IsCTFEable(CodegenInfo* cgi,int);
    vector<ExprAST*> GetCallExprList();
};


class DataDefAST{
public:
	string Name;
	vector< pair<string,TypeAST*> > MemberList;

	DataDefAST(){}
	DataDefAST(vector< pair<string,TypeAST*> > member):MemberList(member){}
};

class GroupDefAST{
public:
	string Name;
	string TargetName; //括弧の中の名前
	vector< pair<string,TypeAST*> > MemberList;

	GroupDefAST(){}
	GroupDefAST(vector< pair<string,TypeAST*> > member):MemberList(member){}
};
