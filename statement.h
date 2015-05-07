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

class Environment;

class StatementAST{
public:
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)=0;
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)=0;
	virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int)=0;
	virtual vector<shared_ptr<ExprAST> > GetCallExprList()=0;
	virtual ~StatementAST(){}
};

class IfStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Condition;
    shared_ptr<BlockAST> ThenBody;
    shared_ptr<BlockAST> ElseBody;

    IfStatementAST(shared_ptr<ExprAST> cond,shared_ptr<BlockAST> thenbody,shared_ptr<BlockAST> elsebody):Condition(cond),ThenBody(thenbody),ElseBody(elsebody){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr< vector< pair<string,shared_ptr<TypeAST> > > > CurrentLocalVars);
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
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};


class ReturnStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Expression;

    ReturnStatementAST(shared_ptr<ExprAST> exp):Expression(exp){};
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class AsyncStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Expression;

    AsyncStatementAST(shared_ptr<ExprAST> exp):Expression(exp){};
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class VariableDefStatementAST : public StatementAST{
public:
    shared_ptr<pair<string,shared_ptr<TypeAST> > > Variable;
    shared_ptr<ExprAST> InitialValue; //初期値（初期値が未設定の時はnullptr）
    int LocalIndex;
    int FlameBack; //常に0

    VariableDefStatementAST(shared_ptr<pair<string,shared_ptr<TypeAST> > > var,shared_ptr<ExprAST> initval):Variable(var),InitialValue(initval){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};

class ExpressionStatementAST : public StatementAST{
public:
    shared_ptr<ExprAST> Expression;

    ExpressionStatementAST(shared_ptr<ExprAST> evalexpr):Expression(evalexpr){}
    virtual void Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo);
    virtual void CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars);
    virtual bool IsCTFEable(shared_ptr<CodegenInfo> cgi,int);
    virtual vector<shared_ptr<ExprAST> > GetCallExprList();
};
