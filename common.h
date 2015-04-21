#pragma once
#include <vector>
#include <string>

using namespace std;

const int TOKENRULECOUNT=32;
const int SYNTAXRULECOUNT=96;
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
	char *str;

	FunctionAST *function_ast;
	vector<TopLevelItem*> *toplevel_list;
	vector<ExprAST*> *arg_exp_list;
	vector< pair<string,TypeAST*> > *parameter_list;
	pair<string,TypeAST*> *parameter_ast;
	vector<VariableDefStatementAST*> *variabledef_list;
	VariableDefStatementAST *variabledef_ast;
	vector<StatementAST*> *statement_list;
	StatementAST *statement_ast;
	vector<ExprAST*> *expression_list;
	ExprAST* expression_ast;
	TypeAST *type_ast;
	vector<TypeAST*> *type_list;
	BlockAST *block_ast;
	DataDefAST *datadef_ast;
	GroupDefAST *groupdef_ast;
	vector< pair<string,ExprAST*> > *datainitval_list;
};
