#pragma once
#include <string>
#include <vector>
#include <map>
#include <list>
#include "utility.h"
#include "parser.h"
#include "lexer.h"
#include "vm.h"
#include "type.h"
#include "color_text.h"
#include <memory>


using namespace std;

class FunctionObject;
class ClosureObject;

void ShowBytecode(shared_ptr<vector<int>> bc);

class ExprAST{
public:
	shared_ptr<TypeAST> TypeInfo;

    virtual ~ExprAST(){}
    virtual bool IsBuilt(){return true;}; //ASTが構築されたか
    virtual bool IsConstant()=0; //定数か
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)=0;
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr< vector< pair<string,shared_ptr<TypeAST> > > > CurrentLocalVars)=0;
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index)=0;
	virtual vector<shared_ptr<ExprAST> > GetCallExprList()=0;
};

class UnBuiltExprAST : public ExprAST{
public:
	shared_ptr< vector<shared_ptr<ExprAST> > > ExprList;
    UnBuiltExprAST(shared_ptr< vector<shared_ptr<ExprAST> > > val):ExprList(val){TypeInfo=nullptr;}
    virtual bool IsBuilt(){return false;}
    virtual bool IsConstant(){return false;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo){};
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
    shared_ptr<ExprAST> BuildAST(shared_ptr<CodegenInfo>);
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
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return true;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class DoubleValExprAST : public ExprAST{
public:
	double Value;
	int PoolIndex;
    DoubleValExprAST(shared_ptr<CodegenInfo> cgi,double val):Value(val){
    	TypeInfo=make_shared<BasicTypeAST>("double");
		VMValue v;v.primitive.double_value=Value;
		PoolIndex=VM::PublicConstantPool.SetValue(v);
	}
    virtual bool IsConstant(){return true;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
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
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return true;}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class NullValExprAST : public ExprAST{
public:
    NullValExprAST(shared_ptr<TypeAST> type){TypeInfo=type;}
    virtual bool IsConstant(){return true;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
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
        PoolIndex=VM::PublicConstantPool.SetValue(v);
        //cout<<"#"<<PoolIndex<<" : <string>"<<Value<<endl;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){return true;}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return TypeInfo;}
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return true;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ListValExprAST : public ExprAST{
public:
    shared_ptr<list<shared_ptr<ExprAST> > > Value;
    ListValExprAST(shared_ptr<CodegenInfo> cgi,shared_ptr<list<shared_ptr<ExprAST> > > val):Value(val){
        TypeInfo=nullptr;
    }
    virtual bool IsConstant(){
		for(auto iter=Value->begin();iter!=Value->end();iter++){
			if((*iter)->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class TupleValExprAST : public ExprAST{
public:
    shared_ptr<list<shared_ptr<ExprAST> > > Value;
    TupleValExprAST(shared_ptr<CodegenInfo> cgi,shared_ptr<list<shared_ptr<ExprAST> > > val):Value(val){
        TypeInfo=nullptr;
    }
    virtual bool IsConstant(){
		for(auto iter=Value->begin();iter!=Value->end();iter++){
			if((*iter)->IsConstant()==false){
				return false;
			}
		}
		return true;
    }
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class DataValExprAST : public ExprAST{
public:
    shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > InitValue;

    DataValExprAST(string TypeName,shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > initval):InitValue(initval){
        TypeInfo=make_shared<BasicTypeAST>(TypeName);
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
    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

//操車場アルゴリズムのために
class OperatorAST : public ExprAST{
public:
    string Operator;
	OperatorInfo Info;

    OperatorAST(string n):Operator(n){};
    virtual bool IsConstant(){return true;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo){}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){return nullptr;}
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return false;}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class FunctionAST : public ExprAST,public enable_shared_from_this<FunctionAST>{
public:
	int PoolIndex;
	bool isBuiltin;
	string Name;
	bool is_builtin_CTFEable;
    shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > Args;
	shared_ptr<Flame> ParentFlame; //クロージャの場合、生成元のフレームを覚えておく（実行時にVMが使用）
    shared_ptr<vector<int> > bytecodes;
    shared_ptr<vector<label_or_immediate> > bytecode_labels;
	shared_ptr<BlockAST> Body;
	shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > LocalVariables; //Argsを含む

	virtual bool IsConstant(){return false;}

    //自らの型を返すだけでなく、bodyについて型検査を実施する
	virtual shared_ptr<TypeAST> CheckType( shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo, shared_ptr<vector< pair<string, shared_ptr<TypeAST> >  > > CurrentLocalVars);

    FunctionAST(shared_ptr<CodegenInfo> cgi,string n,shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > a,shared_ptr<TypeAST> rett,shared_ptr<BlockAST> bdy):Name(n),Args(a),Body(bdy){
		vector<shared_ptr<TypeAST> > typelist;
		vector< pair<string,shared_ptr<TypeAST> > >::iterator iter;
		for(iter=a->begin();iter!=a->end();iter++){
			typelist.push_back((*iter).second);
		}
		typelist.push_back(rett);

		LocalVariables=make_shared<vector< pair<string,shared_ptr<TypeAST> > > >();
		bytecodes=make_shared<vector<int> >();
		bytecode_labels=make_shared<vector<label_or_immediate> >();

		TypeInfo=make_shared<FunctionTypeAST>(typelist);
        isBuiltin=false;
        VMValue v; v.ref_value=make_shared<FunctionObject>(Name,TypeInfo,isBuiltin,Args,LocalVariables,bytecode_labels);
		PoolIndex = VM::PublicConstantPool.SetValue(v);
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
        PoolIndex=VM::PublicConstantPool.SetValue(v);
    }

    virtual void Codegen(shared_ptr<vector<int> > unused_argumnt,shared_ptr<CodegenInfo> geninfo);

    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ContinuationAST : public ExprAST{
public:
	string VarName;
	shared_ptr<BlockAST> Body;
	shared_ptr<FunctionAST> InternalClosure; //結局は構文糖衣みたいなもの

	virtual bool IsConstant(){return false;}

    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);

    ContinuationAST(shared_ptr<CodegenInfo> cgi,shared_ptr<TypeAST> t,string varname,shared_ptr<BlockAST> bdy):VarName(varname),Body(bdy){
        TypeInfo=t;

        shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > args=make_shared< vector< pair<string,shared_ptr<TypeAST> > > >();
        args->push_back(pair<string,shared_ptr<TypeAST> >(varname,make_shared<ContinuationTypeAST>(t)));
		InternalClosure = make_shared<FunctionAST>(cgi, "<anonymous:" + IntToString(anonymous_id++) + ">", args, t, Body);
    }

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);

    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int){return false;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class NewChannelAST : public ExprAST{
public:
	shared_ptr<ExprAST> CapacityExpression;

	virtual bool IsConstant(){return false;}

    shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);

    NewChannelAST(shared_ptr<TypeAST> t,shared_ptr<ExprAST> capacity){
        TypeInfo=make_shared<ChannelTypeAST>(t);
        this->CapacityExpression=capacity;
	}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);

    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int){return false;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class NewVectorAST : public ExprAST{
public:
	shared_ptr<ExprAST> CapacityExpression;

	virtual bool IsConstant(){return false;}

    shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);

    NewVectorAST(shared_ptr<TypeAST> t,shared_ptr<ExprAST> capacity){
        TypeInfo=make_shared<VectorTypeAST>(t);
        this->CapacityExpression=capacity;
	}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);

    virtual bool IsCTFEable(shared_ptr<CodegenInfo>,int){return false;};
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class VariableExprAST : public ExprAST{
public:
    string Name;
    int LocalIndex;//ローカル変数のインデックス番号（フレーム配列のインデックス）
    int FlameBack; //フレームを何回遡るか
    bool is_toplevel_func; //トップレベル関数または組み込み関数かどうかで分かる)

    VariableExprAST(const string &name):Name(name){TypeInfo=nullptr;}
    virtual bool IsConstant(){return false;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
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

class CallExprAST : public ExprAST,public enable_shared_from_this<CallExprAST>{
public:
    shared_ptr<ExprAST> callee;
    VMValue CalculatedValue; //callee==nullptrの時はこちらの計算結果を利用
    shared_ptr<vector<shared_ptr<ExprAST> > > args;
    bool IsTail;
    bool IsAsync;

    CallExprAST(shared_ptr<ExprAST> callee_func,shared_ptr<vector<shared_ptr<ExprAST> > > args_list):callee(callee_func),args(args_list){
    	TypeInfo=nullptr;IsTail=false;IsAsync=false;
	}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){
    	return (callee==nullptr);
	}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ListRefExprAST : public ExprAST{
public:
    shared_ptr<ExprAST> target;
    shared_ptr<ExprAST> IndexExpression;

    ListRefExprAST(shared_ptr<ExprAST> t,shared_ptr<ExprAST> idx):target(t),IndexExpression(idx){TypeInfo=nullptr;}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){
    	return false;
	}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
    void AssignmentCodegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
};

class DataMemberRefExprAST : public ExprAST{
public:
    shared_ptr<ExprAST> target;
    string MemberName;

    DataMemberRefExprAST(shared_ptr<ExprAST> t,string memname):target(t),MemberName(memname){TypeInfo=nullptr;}

    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){
    	return false;
	}
    virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
    void AssignmentCodegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
};

class UnaryExprAST : public ExprAST{
public:
    string Operator;
    shared_ptr<ExprAST> Operand;
	int tofuncall_FlameBack, tofuncall_LocalIndex;

    UnaryExprAST(string opc,shared_ptr<ExprAST> oprnd):Operator(opc),Operand(oprnd){TypeInfo=nullptr;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){return false;}
	virtual shared_ptr<TypeAST> CheckType(shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo, shared_ptr<vector< pair<string, shared_ptr<TypeAST> >  > > CurrentLocalVars);
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return Operand->IsCTFEable(cgi,curr_fun_index);}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class BinaryExprAST : public ExprAST{
public:
	string Operator;
    shared_ptr<ExprAST> LHS,RHS;
	int tofuncall_FlameBack, tofuncall_LocalIndex;

    BinaryExprAST(string opc,shared_ptr<ExprAST> lhs,shared_ptr<ExprAST> rhs):Operator(opc),LHS(lhs),RHS(rhs){TypeInfo=nullptr;}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual bool IsConstant(){return false;}
	virtual shared_ptr<TypeAST> CheckType( shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo, shared_ptr<vector< pair<string, shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){return LHS->IsCTFEable(cgi,curr_fun_index)&&RHS->IsCTFEable(cgi,curr_fun_index);}
	virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};
