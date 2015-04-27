#pragma once
#include "color_text.h"
#include "vm.h"
#include <iostream>
#include <memory>
#include <vector>

class Parser;
class Lexer;
class FunctionAST;
class VariableDefStatementAST;
class TypeAST;
class DataDefAST;
class GroupDefAST;

enum{Left,Right};
enum{Unary,Binary};


class ConstantPool{
private:
    vector< VMValue > refpool;
public:
    int SetValue(VMValue item){
        refpool.push_back(item);
        return refpool.size()-1;
    }
    VMValue GetValue(int index){
        return refpool[index];
    }
    unsigned int Size(){
        return refpool.size();
    }
    void Clear(){
		refpool.clear();
    };
};

class OperatorInfo{
public:
	OperatorInfo(){}
    OperatorInfo(int uorb,int assoc,int prec):UnaryOrBinary(uorb),Associativity(assoc),Precedence(prec){}
	int UnaryOrBinary;
    int Associativity;
    int Precedence;
};

//パースした時に得た情報を詰め込んでおく
class CodegenInfo{
public:
    multimap<string,OperatorInfo> OperatorList;
    map<pair<string,string>, void (*)(shared_ptr<VM>) > BuiltinFunctionList; //関数名と型名から関数を引っ張ってくる
    vector<shared_ptr<FunctionAST> > TopLevelFunction;
    vector<shared_ptr<VariableDefStatementAST> > TopLevelVariableDef;
    vector<shared_ptr<DataDefAST> > TopLevelDataDef;
    vector<shared_ptr<GroupDefAST> > TopLevelGroupDef;
    shared_ptr<vector<int> > Bootstrap;
    ConstantPool PublicConstantPool;
    int MainFuncPoolIndex; //main関数のコンスタントプール・インデックス
	vector<int> ChildPoolIndex;
	shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > LocalVariables;

	CodegenInfo(){
		LocalVariables=make_shared<vector< pair<string,shared_ptr<TypeAST> > > >();
		Bootstrap=make_shared<vector<int> >();
	}
};

class Executable{
public:

};

class Compiler{
private:
	void ASTgen();
    void RegisterChildClosure();
    void TypeCheck();
    void CTFE(int);
    void Codegen();
    void RegisterBuiltinFunction(string name,void (*funcptr)(shared_ptr<VM>),shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > arg,shared_ptr<TypeAST>  rettype,bool ctfeable); //トップレベル関数リスト、ビルトイン関数リストへ登録します
public:
	shared_ptr<Lexer> lexer;
	shared_ptr<Parser> parser;
    shared_ptr<CodegenInfo> genInfo;

    Compiler(shared_ptr<Lexer> l,shared_ptr<Parser> p):lexer(l),parser(p){
		genInfo=make_shared<CodegenInfo>();
    }

    void Compile(){
    	cout<<endl<<BG_GREEN<<"構文解析を行っています..."<<RESET<<endl;
		ASTgen();
		RegisterChildClosure();
		cout<<BG_GREEN<<"型検査を行っています..."<<RESET<<endl;
		TypeCheck();
		cout<<BG_GREEN<<"コード生成を行っています..."<<RESET<<endl;
		Codegen();
		cout<<BG_GREEN<<"コンパイル時関数実行を行っています..."<<RESET<<endl;
		CTFE(3);

		VM vm(genInfo);
		cout<<BG_BLUE<<"VMを起動します..."<<RESET<<endl;
		vm.Init();
		vm.Run(false);
		cout<<endl;
    }
};
