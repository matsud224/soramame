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
#include <memory>

using namespace std;


class OperatorInfo;
class TypeAST;
class ExprAST;
class FunctionAST;
class Flame;
class VariableDefStatementAST;
class TopLevelItem;
class BlockAST;
class FunctionObject;

class EnvItem{
public:
	pair<string,shared_ptr<TypeAST> > VariableInfo;
	int LocalIndex;
};

class Environment{
public:
	vector< EnvItem > Items;
	bool is_internalblock; //関数内部のブロック（if,whileなど）か？
	shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > LocalVariablesPtr;
};


class TypeAST{
public:
	virtual ~TypeAST(){cout<<"回収されました(TypeAST)"<<endl;}
	virtual string GetName()=0;
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
	FunctionTypeAST(vector<shared_ptr<TypeAST> > t_list):TypeList(t_list){}
	vector<shared_ptr<TypeAST> > TypeList;
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
    ListTypeAST(shared_ptr<TypeAST> t):ContainType(t){}
    shared_ptr<TypeAST>  ContainType;
    virtual string GetName(){
		return "["+ContainType->GetName()+"]";
    }
};

//タプル型
class TupleTypeAST : public TypeAST{
public:
    TupleTypeAST(vector<shared_ptr<TypeAST> > t_list):ContainTypeList(t_list){}
    vector<shared_ptr<TypeAST> > ContainTypeList;
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
	shared_ptr<TypeAST> TypeInfo;

    virtual ~ExprAST(){cout<<"回収されました(ExprAST)"<<endl;}
    virtual bool IsBuilt(){return true;}; //ASTが構築されたか
    virtual bool IsConstant()=0; //定数か
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)=0;
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr< vector< pair<string,shared_ptr<TypeAST> > > > CurrentLocalVars)=0;
    virtual vector<int> FindChildFunction()=0;
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index)=0;
	virtual vector<shared_ptr<ExprAST> > GetCallExprList()=0;
};

class UnBuiltExprAST : public ExprAST{
public:
	shared_ptr< vector<shared_ptr<ExprAST> > > ExprList;
    UnBuiltExprAST(shared_ptr< vector<shared_ptr<ExprAST> > > val):ExprList(val){TypeInfo=NULL;}
    virtual bool IsBuilt(){return false;}
    virtual bool IsConstant(){return false;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo){};
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
    shared_ptr<ExprAST> BuildAST(shared_ptr<CodegenInfo>);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return false;}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class IntValExprAST : public ExprAST{
public:
	int Value;
    IntValExprAST(int val):Value(val){TypeInfo=make_shared<BasicTypeAST>("int");}
    virtual bool IsConstant(){return true;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return true;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class BoolValExprAST : public ExprAST{
public:
	bool Value;
    BoolValExprAST(bool val):Value(val){TypeInfo=make_shared<BasicTypeAST>("bool");}
    virtual bool IsConstant(){return true;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return true;}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class StringValExprAST : public ExprAST{
public:
    shared_ptr<string> Value;
    int PoolIndex;
    StringValExprAST(shared_ptr<CodegenInfo> cgi,string val){
        Value=make_shared<string>(val);
        TypeInfo=make_shared<BasicTypeAST>("string");
        //コンスタントプールへの登録
        VMValue v;v.ref_value=Value;
        PoolIndex=cgi->PublicConstantPool.SetValue(v);
        //cout<<"#"<<PoolIndex<<" : <string>"<<Value<<endl;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){return true;}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return true;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ListValExprAST : public ExprAST{
public:
    shared_ptr<list<shared_ptr<ExprAST> > > Value;
    int PoolIndex;
    ListValExprAST(shared_ptr<CodegenInfo> cgi,shared_ptr<list<shared_ptr<ExprAST> > > val):Value(val){
        TypeInfo=NULL;
        PoolIndex=-1;
    }
    virtual bool IsConstant(){
		list<shared_ptr<ExprAST> >::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			if((*iter)->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class TupleValExprAST : public ExprAST{
public:
    shared_ptr<list<shared_ptr<ExprAST> > > Value;
    int PoolIndex;
    TupleValExprAST(shared_ptr<CodegenInfo> cgi,shared_ptr<list<shared_ptr<ExprAST> > > val):Value(val){
        TypeInfo=NULL;
        PoolIndex=-1;
    }
    virtual bool IsConstant(){
		list<shared_ptr<ExprAST> >::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			if((*iter)->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class DataValExprAST : public ExprAST{
public:
    shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > InitValue;

    int PoolIndex;
    DataValExprAST(string TypeName,shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > initval):InitValue(initval){
        TypeInfo=make_shared<BasicTypeAST>(TypeName);
        PoolIndex=-1;
    }
    virtual bool IsConstant(){
		vector< pair<string,shared_ptr<ExprAST> > >::iterator iter;
		for(iter=InitValue->begin();iter!=InitValue->end();iter++){
			if((*iter).second->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

//操車場アルゴリズムのために
class OperatorAST : public ExprAST{
public:
    string Operator;

    OperatorAST(string n):Operator(n){};
    virtual bool IsConstant(){return true;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo){}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return NULL;}
    virtual vector<int> FindChildFunction(){return vector<int>();};
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return false;}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class FunctionAST : public ExprAST{
public:
	int PoolIndex;
	bool isBuiltin;
	string Name;
	bool is_builtin_CTFEable;
    shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > Args;
	shared_ptr<Flame> ParentFlame; //クロージャの場合、生成元のフレームを覚えておく（実行時にVMが使用）
    shared_ptr<vector<int> > bytecodes;
    shared_ptr<vector<int> > ChildPoolIndex; //生成したクロージャのコンスタントプールのインデックス
	shared_ptr<BlockAST> Body;
	shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > LocalVariables; //Argsを含む

	virtual bool IsConstant(){return false;}

    //自らの型を返すだけでなく、bodyについて型検査を実施する
    shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    vector<int> FindChildFunction(); //関数内で作られるクロージャを探し、ChildPoolIndexへ登録します。

    FunctionAST(shared_ptr<CodegenInfo> cgi,string n,shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > a,shared_ptr<TypeAST> rett,shared_ptr<BlockAST> bdy):Name(n),Args(a),Body(bdy){
		vector<shared_ptr<TypeAST> > typelist;
		vector< pair<string,shared_ptr<TypeAST> > >::iterator iter;
		for(iter=a->begin();iter!=a->end();iter++){
			typelist.push_back((*iter).second);
		}
		typelist.push_back(rett);

		TypeInfo=make_shared<FunctionTypeAST>(typelist);
        isBuiltin=false;
        VMValue v; v.ref_value=make_shared<FunctionObject>(Name,TypeInfo,isBuiltin,Args,LocalVariables,bytecodes);
        PoolIndex=cgi->PublicConstantPool.SetValue(v);
        //cout<<"#"<<PoolIndex<<" : <closure>"<<Name<<endl;
    }

    //組み込み関数用のコンストラクタ
    FunctionAST(shared_ptr<CodegenInfo> cgi,string n,shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > a,shared_ptr<TypeAST> rett,bool CTFEable):Name(n),Args(a),is_builtin_CTFEable(CTFEable){
		vector<shared_ptr<TypeAST> > typelist;
		vector< pair<string,shared_ptr<TypeAST> > >::iterator iter;
		for(iter=a->begin();iter!=a->end();iter++){
			typelist.push_back((*iter).second);
		}
		typelist.push_back(rett);
		TypeInfo=make_shared<FunctionTypeAST>(typelist);
        isBuiltin=true;
        VMValue v; v.ref_value=make_shared<FunctionObject>(Name,TypeInfo,isBuiltin,Args);
        PoolIndex=cgi->PublicConstantPool.SetValue(v);
        //cout<<"#"<<PoolIndex<<" : <closure>"<<Name<<endl;
    }

    virtual void Codegen(shared_ptr<vector<int> > unused_argumnt,shared_ptr<CodegenInfo> geninfo);

    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};


class VariableExprAST : public ExprAST{
public:
    string Name;
    int LocalIndex;//ローカル変数のインデックス番号（フレーム配列のインデックス）
    int FlameBack; //フレームを何回遡るか
    bool is_toplevel_func; //トップレベル関数または組み込み関数かどうか(CheckTypeで分かる)

    VariableExprAST(const string &name):Name(name){TypeInfo=NULL;}
    virtual bool IsConstant(){return false;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction(){return vector<int>();};
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
    	if(FlameBack==0){return true;}
    	if(is_toplevel_func){
			if(LocalIndex==curr_fun_index){
				return true;
			}
			return cgi->TopLevelFunction[LocalIndex]->IsCTFEable(cgi,curr_fun_index);
    	}
    	return false;
	}
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class CallExprAST : public ExprAST{
public:
    shared_ptr<ExprAST> callee;
    VMValue CalculatedValue; //callee==NULLの時はこちらの計算結果を利用
    shared_ptr<vector<shared_ptr<ExprAST> > > args;

    CallExprAST(shared_ptr<ExprAST> callee_func,shared_ptr<vector<shared_ptr<ExprAST> > > args_list):callee(callee_func),args(args_list){TypeInfo=NULL;}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){
    	return (callee==NULL);
	}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ListRefExprAST : public ExprAST{
public:
    shared_ptr<ExprAST> target;
    shared_ptr<ExprAST> IndexExpression;

    ListRefExprAST(shared_ptr<ExprAST> t,shared_ptr<ExprAST> idx):target(t),IndexExpression(idx){TypeInfo=NULL;}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){
    	return false;
	}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class DataMemberRefExprAST : public ExprAST{
public:
    shared_ptr<ExprAST> target;
    string MemberName;

    DataMemberRefExprAST(shared_ptr<ExprAST> t,string memname):target(t),MemberName(memname){TypeInfo=NULL;}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){
    	return false;
	}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class UnaryExprAST : public ExprAST{
public:
    string Operator;
    shared_ptr<ExprAST> Operand;

    UnaryExprAST(string opc,shared_ptr<ExprAST> oprnd):Operator(opc),Operand(oprnd){TypeInfo=NULL;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){return false;}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction(){return vector<int>();}; //AST未構築なので呼び出されることはない。
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return Operand->IsCTFEable(cgi,curr_fun_index);}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class BinaryExprAST : public ExprAST{
public:
	string Operator;
    shared_ptr<ExprAST> LHS,RHS;

    BinaryExprAST(string opc,shared_ptr<ExprAST> lhs,shared_ptr<ExprAST> rhs):Operator(opc),LHS(lhs),RHS(rhs){TypeInfo=NULL;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){return false;}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction(){return vector<int>();}; //AST未構築なので呼び出されることはない。
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return LHS->IsCTFEable(cgi,curr_fun_index)&&RHS->IsCTFEable(cgi,curr_fun_index);}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};



class StatementAST{
public:
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)=0;
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)=0;
	virtual vector<int> FindChildFunction()=0;
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int)=0;
	virtual vector<shared_ptr<ExprAST> > GetCallExprList()=0;
	virtual ~StatementAST(){cout<<"回収されました(StatementAST)"<<endl;}
};

class IfStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Condition;
    shared_ptr<BlockAST> ThenBody;
    shared_ptr<BlockAST> ElseBody;

    IfStatementAST(shared_ptr<ExprAST> cond,shared_ptr<BlockAST> thenbody,shared_ptr<BlockAST> elsebody):Condition(cond),ThenBody(thenbody),ElseBody(elsebody){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr< vector< pair<string,shared_ptr<TypeAST> > > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class WhileStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Condition;
    shared_ptr<BlockAST> Body;

    WhileStatementAST(shared_ptr<ExprAST> cond,shared_ptr<BlockAST> body):Condition(cond),Body(body){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};


class ReturnStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Expression;

    ReturnStatementAST(shared_ptr<ExprAST> exp):Expression(exp){};
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class VariableDefStatementAST : public StatementAST{
public:
    shared_ptr<pair<string,shared_ptr<TypeAST> > > Variable;
    shared_ptr<ExprAST> InitialValue; //初期値（初期値が未設定の時はNULL）
    int LocalIndex;
    int FlameBack; //常に0

    VariableDefStatementAST(shared_ptr<pair<string,shared_ptr<TypeAST> > > var,shared_ptr<ExprAST> initval):Variable(var),InitialValue(initval){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ExpressionStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Expression;

    ExpressionStatementAST(shared_ptr<ExprAST> evalexpr):Expression(evalexpr){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual vector<int> FindChildFunction();
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class BlockAST{
public:
	shared_ptr<vector<shared_ptr<StatementAST> > > Body;

	BlockAST(shared_ptr<vector<shared_ptr<StatementAST> > > body):Body(body){}

	void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
	void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,bool isinternalblock,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    vector<int> FindChildFunction();
    bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    vector<shared_ptr<ExprAST> > GetCallExprList();
};


class DataDefAST{
public:
	string Name;
	vector< pair<string,shared_ptr<TypeAST> >  > MemberList;

	DataDefAST(){}
	DataDefAST(vector< pair<string,shared_ptr<TypeAST> >  > member):MemberList(member){}
};

class GroupDefAST{
public:
	string Name;
	string TargetName; //括弧の中の名前
	vector< pair<string,shared_ptr<TypeAST> >  > MemberList;

	GroupDefAST(){}
	GroupDefAST(vector< pair<string,shared_ptr<TypeAST> >  > member):MemberList(member){}
};
