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
