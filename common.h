#pragma once
#include <vector>
#include <string>
#include <memory>

using namespace std;

const int TOKENRULECOUNT=33;
const int SYNTAXRULECOUNT=99;
const int SYMBOLCOUNT=60; //SYNTAXEND,INPUTEND,EMPTYをのぞく


enum Symbol{
	SYNTAXEND=-3,INPUTEND=-2,EMPTY=-1,LINEEND,S,VAR,FUN,IF,ELSE,RETURN_S,INTVAL,BOOLVAL,STRINGVAL,IDENT,OPERATOR,program,function,parameter_list,parameter,
	variabledef_list,variabledef,statement_list,statement,operator_n/*operatorはキーワードなので*/,expression,arg_list,intvalexpr,boolvalexpr,stringvalexpr,primary,
	variableexpr, parenexpr, funcallexpr, closureexpr, returnstatement,type,type_list,ifstatement,
	SEMICOLON,LPAREN,RPAREN,LBRACE,RBRACE,LBRACKET,RBRACKET,COMMA,COLON,DOT,block,WHILE,whilestatement,listvalexpr,tuplevalexpr,
	DATA,GROUP,datadef,groupdef,datamember_list,groupmember_list,dataexpr,initassign_list,listrefexpr,datamemberrefexpr
};

class FunctionAST;
class TopLevelItem;
class ExprAST;
class TypeAST;
class VariableDefStatementAST;
class StatementAST;
class BlockAST;
class DataDefAST;
class GroupDefAST;

union TokenValue{
	int intval;
	bool boolval;
	string str;

	shared_ptr<FunctionAST> function_ast;
	shared_ptr< vector<shared_ptr<TopLevelItem> > > toplevel_list;
	shared_ptr< vector<shared_ptr<ExprAST> > > arg_exp_list;
	shared_ptr< vector< pair<string,shared_ptr<TypeAST> >  > > parameter_list;
	shared_ptr< pair<string,shared_ptr<TypeAST> > > parameter_ast;
	shared_ptr< vector<shared_ptr<VariableDefStatementAST> > > variabledef_list;
	shared_ptr< VariableDefStatementAST > variabledef_ast;
	shared_ptr<vector<shared_ptr<StatementAST> > > statement_list;
	shared_ptr<StatementAST> statement_ast;
	shared_ptr<vector<shared_ptr<ExprAST> > > expression_list;
	shared_ptr<ExprAST> expression_ast;
	shared_ptr<TypeAST> type_ast;
	shared_ptr<vector<shared_ptr<TypeAST> > > type_list;
	shared_ptr<BlockAST> block_ast;
	shared_ptr<DataDefAST> datadef_ast;
	shared_ptr<GroupDefAST> groupdef_ast;
	shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > datainitval_list;
};
