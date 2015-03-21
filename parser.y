%{
#include <iostream>
#include <string>

using namespace std;

void yyerror(const char *str){
	fprintf(stderr,"error: %s\n",str);
}

int yywrap(){
	return 1;
}

int main(){
	yyparse();
}

%}

%start program

%union {
	int intval;
	bool boolval;
	string str;
	ProgramAST program_ast;
	FunctionAST function_ast;
	vector<ParameterAST> parameter_ast_list;
	ParameterAST parameter;
	vector<VariableDefAST> variabledef_ast_list;
	VariableDefAST variabledef_ast;
	vector<StatementAST> statement_ast_list;
	StatementAST statement_ast;
	vector<ExprAST> expression_ast_list;
	ExprAST expression_ast;
	OperatorAST operator_ast;
	ReturnStatementAST return_statement_ast;
	TypeAST type_ast;
	vector<TypeAST> type_ast_list;
	IfStatementAST if_statemnt_ast;
}

%token VAR FUN IF ELSE RETURN ARROW
%token<intval> INTVAL
%token<boolval> BOOLVAL
%token<str> STRINGVAL IDENT OPERATOR
%type<program_ast> program
%type<function_ast> function
%type<parameter_ast_list> parameter_list
%type<parameter_ast> parameter
%type<variabledef_ast_list> variabledef_list
%type<variabledef_ast> variabledef
%type<statement_ast_list> statement_list
%type<statement_ast> statement
%type<operator_ast> operator
%type<expression_ast_list> expression arg_list
%type<expression_ast> intvalexpr boolvalexpr stringvalexpr primary variableexpr parenexpr funcallexpr closureexpr
%type<return_statement_ast> returnstatement
%type<type_ast> type
%type<type_ast_list> type_list

%%

program :
		| program function
		| program VAR variabledef_list ';'
		
intvalexpr : INTVAL
boolvalexpr : BOOLVAL
stringvalexpr : STRINGVAL

operator : OPERATOR

function : FUN IDENT '(' parameter_list ')' '{' statement_list '}'
		| FUN IDENT '(' parameter_list ')' ARROW type '{' statement_list '}'

parameter_list : 
		| parameter
		| parameter_list ',' parameter

parameter : IDENT ':' type

variabledef_list : variabledef
		| variabledef_list ',' variabledef

variabledef : IDENT ':' type
		| IDENT ':' type '=' expression

statement_list :
		| statement_list statement

statement : expression ';'
		| VAR variabledef_list ';'
		| returnstatement ';'
		| ifstatement

expression : primary
		| expression primary
		| expression operator
		| expression infixfuncall
	
funcallexpr : variableexpr '(' arg_list ')'
		| closureexpr '(' arg_list ')'

primary : intvalexpr
		| boolvalexpr
		| stringvalexpr
		| parenexpr
		| funcallexpr
		| closureexpr
		| variableexpr
		
		
variableexpr : IDENT

parenexpr : '(' expression ')'

returnstatement : RETURN expression

type : IDENT
		| '(' type_list')' ARROW type

type_list : 
		| type
		| type_list ',' type

		
infixfuncall : '`' IDENT '`'

closureexpr : '(' parameter_list ')' ARROW type '{' statement_list '}'

arg_list : 
		| expression
		| arg_list ',' expression

ifstatement : IF '(' expression ')' '{' statement_list '}'
			| IF '(' expression ')' '{' statement_list '}' ELSE '{' statement_list '}'
			
%%

#include "lex.yy.c"