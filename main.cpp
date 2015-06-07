﻿#include <iostream>
#include <fstream>
#include "lexer.h"
#include <string>
#include <vector>
#include "ast_etc.h"
#include "statement.h"
#include "type.h"
#include "expression.h"
#include "vm.h"
#include "utility.h"
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "parser.h"
#include "color_text.h"
#include "common.h"
#include <time.h>
#include <math.h>
#include "parser_actions.h"
#include <memory>
#include "exceptions.h"
#include <thread>
#include <chrono>
#include <sstream>


using namespace std;
using namespace std::chrono;

int comment_depth=0; //ブロックコメントのネストの深さ
pair<Symbol,TokenValue> commentstart_lex(string str,Lexer *lex){
	lex->curr_state=COMMENT;
	comment_depth++;
	return pair<Symbol,TokenValue>(INPUTEND ,Lexer::dummy);
};
pair<Symbol,TokenValue> commentend_lex(string str,Lexer *lex){
	comment_depth--;
	if(comment_depth==0){
		lex->curr_state=INITIAL;
	}
	return pair<Symbol,TokenValue>(INPUTEND ,Lexer::dummy);
};
pair<Symbol,TokenValue> var_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(VAR ,Lexer::dummy);};
pair<Symbol,TokenValue> if_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(IF ,Lexer::dummy);};
pair<Symbol,TokenValue> while_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(WHILE ,Lexer::dummy);};
pair<Symbol,TokenValue> fun_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(FUN ,Lexer::dummy);};
pair<Symbol,TokenValue> else_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(ELSE ,Lexer::dummy);};
pair<Symbol,TokenValue> new_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(NEW ,Lexer::dummy);};
pair<Symbol,TokenValue> return_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(RETURN_S ,Lexer::dummy);};
pair<Symbol,TokenValue> data_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(DATA ,Lexer::dummy);};
pair<Symbol,TokenValue> group_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(GROUP ,Lexer::dummy);};
pair<Symbol,TokenValue> continuation_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(CONTINUATION ,Lexer::dummy);};
pair<Symbol,TokenValue> channel_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(CHANNEL ,Lexer::dummy);};
pair<Symbol,TokenValue> callcc_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(CALLCC ,Lexer::dummy);};
pair<Symbol,TokenValue> async_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(ASYNC ,Lexer::dummy);};
pair<Symbol,TokenValue> boolval_lex(string str,Lexer *lex){
	TokenValue t;
	string val(str);
	if(val=="true"){
		t.boolval=true;
	}else{
		t.boolval=false;
	}
	return pair<Symbol,TokenValue>(BOOLVAL , t);
};
pair<Symbol,TokenValue> semicolon_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(SEMICOLON ,Lexer::dummy);};
pair<Symbol,TokenValue> colon_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(COLON ,Lexer::dummy);};
pair<Symbol,TokenValue> dot_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(DOT ,Lexer::dummy);};
pair<Symbol,TokenValue> comma_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(COMMA ,Lexer::dummy);};
pair<Symbol,TokenValue> lparen_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(LPAREN ,Lexer::dummy);};
pair<Symbol,TokenValue> rparen_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(RPAREN ,Lexer::dummy);};
pair<Symbol,TokenValue> lbrace_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(LBRACE ,Lexer::dummy);};
pair<Symbol,TokenValue> rbrace_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(RBRACE ,Lexer::dummy);};
pair<Symbol,TokenValue> lbracket_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(LBRACKET ,Lexer::dummy);};
pair<Symbol,TokenValue> rbracket_lex(string str,Lexer *lex){return pair<Symbol,TokenValue>(RBRACKET ,Lexer::dummy);};
pair<Symbol,TokenValue> ident_lex(string str,Lexer *lex){
	TokenValue t;
	t.str=str;
	return pair<Symbol,TokenValue>(IDENT ,t);
};
pair<Symbol,TokenValue> operator_lex(string str,Lexer *lex){
	TokenValue t;
	t.str=str;
	return pair<Symbol,TokenValue>(OPERATOR ,t);
};
pair<Symbol,TokenValue> intval_lex(string str,Lexer *lex){
	TokenValue t;
	stringstream ss;
	ss<<str; ss>>t.intval;
	return pair<Symbol,TokenValue>(INTVAL ,t);
};
pair<Symbol,TokenValue> doubleval_lex(string str,Lexer *lex){
	TokenValue t;
	stringstream ss;
	ss<<str; ss>>t.doubleval;
	return pair<Symbol,TokenValue>(DOUBLEVAL ,t);
};
pair<Symbol,TokenValue> asciival_lex(string str,Lexer *lex){
	TokenValue t;
	string str_temp(str);
	//ダブルクオーテーションを削除
	str_temp.erase(str_temp.begin());
	str_temp.erase(str_temp.end()-1);
	char c=str_temp[0];
	t.intval=c;
	return pair<Symbol,TokenValue>(INTVAL ,t);
};
pair<Symbol,TokenValue> stringval_lex(string str,Lexer *lex){
	TokenValue t;
	//ダブルクオーテーションを削除
	str.erase(str.begin());
	str.erase(str.end()-1);
	t.str=escape_str(str);
	return pair<Symbol,TokenValue>(STRINGVAL ,t);
};
pair<Symbol,TokenValue> nextline_lex(string str,Lexer *lex){
	lex->curr_line++;
	return pair<Symbol,TokenValue>(LINEEND ,Lexer::dummy);
};

TokenRule TOKENRULE[TOKENRULECOUNT]={
	{"/\\*",INITIAL,false,commentstart_lex},
	{R"(//.*[\r\n|\r|\n])",INITIAL,true,nextline_lex},
	{R"([\r\n|\n|\r])",INITIAL,true,nextline_lex},
	{"var",INITIAL,true,var_lex},
	{"fun",INITIAL,true,fun_lex},
	{"new",INITIAL,true,new_lex},
	{"channel",INITIAL,true,channel_lex},
    {"if",INITIAL,true,if_lex},
    {"while",INITIAL,true,while_lex},
    {"else",INITIAL,true,else_lex},
    {"return",INITIAL,true,return_lex},
    {"true",INITIAL,true,boolval_lex},
    {"false",INITIAL,true,boolval_lex},
    {"data",INITIAL,true,data_lex},
    {"group",INITIAL,true,group_lex},
    {"continuation",INITIAL,true,continuation_lex},
    {"callcc",INITIAL,true,callcc_lex},
    {"async",INITIAL,true,async_lex},
    {";",INITIAL,true,semicolon_lex},
	{":",INITIAL,true,colon_lex},
    {",",INITIAL,true,comma_lex},
    {"\\.",INITIAL,true,dot_lex},
    {"\\(",INITIAL,true,lparen_lex},
    {"\\)",INITIAL,true,rparen_lex},
    {"\\{",INITIAL,true,lbrace_lex},
    {"\\}",INITIAL,true,rbrace_lex},
    {"\\[",INITIAL,true,lbracket_lex},
    {"\\]",INITIAL,true,rbracket_lex},
    {R"([a-zA-Z_][a-zA-Z0-9_]*)",INITIAL,true,ident_lex},
	{R"([%$\\#=~\|\^\+\-\*/<>&!\?@]{1,6})",INITIAL,true,operator_lex},
    {R"(\d+\.\d+)",INITIAL,true,doubleval_lex},
    {R"(\d+)",INITIAL,true,intval_lex},
    {R"("([^\\"]|\\.)*")",INITIAL,true,stringval_lex},
    {R"('[\x20-\x7E]')",INITIAL,true,asciival_lex},
    {R"([\t\x20]+)",INITIAL,false,NULL}, //空白は読み飛ばす（コールバック関数をNULLにしておく）

	{R"(/\*)",COMMENT,false,commentstart_lex},
    {R"(\*/)",COMMENT,false,commentend_lex},
    {R"([\r\n|\n|\r])",COMMENT,false,nextline_lex},
    {R"(.)",COMMENT,false,NULL}
};

SyntaxRule SYNTAXRULE[SYNTAXRULECOUNT]={
    {{S,program,SYNTAXEND},success},
    {{program,EMPTY,SYNTAXEND},program_empty_reduce},
    {{program,program,functiondef,SYNTAXEND},program_addfundef_reduce},

    {{program,program,VAR,variabledef_list,SEMICOLON,SYNTAXEND},program_addvardef_reduce},
    {{program,program,VAR,variabledef_list,LINEEND,SYNTAXEND},program_addvardef_reduce},
    {{program,program,VAR,variabledef_list,SYNTAXEND},program_addvardef_reduce},

    {{program,program,datadef,SYNTAXEND},program_adddatadef_reduce},
    {{program,program,groupdef,SYNTAXEND},program_addgroupdef_reduce},
    {{intvalexpr,INTVAL,SYNTAXEND},intvalexpr_reduce},
    {{doublevalexpr,DOUBLEVAL,SYNTAXEND},doublevalexpr_reduce},
    {{boolvalexpr,BOOLVAL,SYNTAXEND},boovalexpr_reduce},
    {{stringvalexpr,STRINGVAL,SYNTAXEND},stringvalexpr_reduce},
    {{operator_n,OPERATOR,SYNTAXEND},operator_n_reduce},
    {{functiondef,FUN,IDENT,LPAREN,parameter_list,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},function_norettype_reduce},
    {{functiondef,FUN,IDENT,LPAREN,parameter_list,RPAREN,operator_n,type,LBRACE,block,RBRACE,SYNTAXEND},function_withrettype_reduce},
    {{parameter_list,EMPTY,SYNTAXEND},parameter_list_empty_reduce},
    {{parameter_list,parameter,SYNTAXEND},parameter_list_parameter_reduce},
    {{parameter_list,parameter_list,COMMA,parameter,SYNTAXEND},parameter_list_addparameter_reduce},
    {{parameter,IDENT,COLON,type,SYNTAXEND},parameter_reduce},
    {{variabledef_list,variabledef,SYNTAXEND},variabledef_list_variabledef_reduce},
    {{variabledef_list,variabledef_list,COMMA,variabledef,SYNTAXEND},variabledef_list_addvariabledef_reduce},
    {{variabledef,IDENT,COLON,type,SYNTAXEND},variabledef_nonassignment_reduce},
    {{variabledef,IDENT,COLON,type,operator_n,expression,SYNTAXEND},variabledef_withassignment_reduce},
    {{variabledef,IDENT,operator_n,expression,SYNTAXEND},variabledef_infer_reduce},
    {{statement_list,EMPTY,SYNTAXEND},statement_list_empty_reduce},
    {{statement_list,statement_list,statement,SYNTAXEND},statement_list_addstatement},

    {{statement_list,statement_list,VAR,variabledef_list,SEMICOLON,SYNTAXEND},statement_list_variabledef_reduce},
    {{statement_list,statement_list,VAR,variabledef_list,LINEEND,SYNTAXEND},statement_list_variabledef_reduce},
    {{statement_list,statement_list,VAR,variabledef_list,SYNTAXEND},statement_list_variabledef_reduce},

    {{statement,expression,SEMICOLON,SYNTAXEND},statement_expression_reduce},
    {{statement,expression,LINEEND,SYNTAXEND},statement_expression_reduce},
    {{statement,expression,SYNTAXEND},statement_expression_reduce},

    {{statement,returnstatement,SEMICOLON,SYNTAXEND},NULL},
    {{statement,returnstatement,LINEEND,SYNTAXEND},NULL},
    {{statement,returnstatement,SYNTAXEND},NULL},

	{{statement,asyncstatement,SEMICOLON,SYNTAXEND},NULL},
    {{statement,asyncstatement,LINEEND,SYNTAXEND},NULL},
    {{statement,asyncstatement,SYNTAXEND},NULL},

    {{statement,ifstatement,SYNTAXEND},NULL},
    {{statement,whilestatement,SYNTAXEND},NULL},
    {{expression,primary,SYNTAXEND},expression_primary_reduce},
    {{expression,operator_n,SYNTAXEND},expression_primary_reduce},
    {{expression,parenexpr,SYNTAXEND},expression_paren_reduce},
    {{expression,expression,primary,SYNTAXEND},expression_add_reduce},
    {{expression,expression,operator_n,SYNTAXEND},expression_add_reduce},
    {{expression,expression,parenexpr,SYNTAXEND},expression_addparen_reduce},
    {{funcallexpr,primary,LPAREN,arg_list,RPAREN,SYNTAXEND},funcallexpr_reduce},
    {{funcallexpr,parenexpr,LPAREN,arg_list,RPAREN,SYNTAXEND},funcallexpr_reduce},
    {{primary,intvalexpr,SYNTAXEND},NULL},
    {{primary,doublevalexpr,SYNTAXEND},NULL},
    {{primary,boolvalexpr,SYNTAXEND},NULL},
    {{primary,stringvalexpr,SYNTAXEND},NULL},
    {{primary,funcallexpr,SYNTAXEND},NULL},
    {{primary,closureexpr,SYNTAXEND},NULL},
    {{primary,callccexpr,SYNTAXEND},NULL},
    {{primary,variableexpr,SYNTAXEND},NULL},
    {{primary,listvalexpr,SYNTAXEND},NULL},
    {{primary,tuplevalexpr,SYNTAXEND},NULL},
    {{primary,dataexpr,SYNTAXEND},NULL},
    {{primary,newobjexpr,SYNTAXEND},NULL},
    {{primary,listrefexpr,SYNTAXEND},NULL},
    {{primary,datamemberrefexpr,SYNTAXEND},NULL},
    {{variableexpr,IDENT,SYNTAXEND},variableexpr_reduce},
    {{parenexpr,LPAREN,expression,RPAREN,SYNTAXEND},parenexpr_reduce},
    {{returnstatement,RETURN_S,expression,SYNTAXEND},returnstatement_reduce},
	{{returnstatement,RETURN_S,SYNTAXEND},returnstatement_noexp_reduce},
    {{type,IDENT,SYNTAXEND},type_normal_reduce},
    {{type,FUN,LPAREN,type_list,RPAREN,operator_n,type,SYNTAXEND},type_fun_reduce},
    {{type,LBRACKET,type,RBRACKET,SYNTAXEND},type_listtype_reduce},
    {{type,LPAREN,type_list,RPAREN,SYNTAXEND},type_tupletype_reduce},
    {{type,CONTINUATION,LPAREN,type,RPAREN,SYNTAXEND},type_conttype_reduce},
    {{type,CHANNEL,LPAREN,type,RPAREN,SYNTAXEND},type_chantype_reduce},
    {{type_list,EMPTY,SYNTAXEND},type_list_empty_reduce},
    {{type_list,type,SYNTAXEND},type_list_type_reduce},
    {{type_list,type_list,COMMA,type,SYNTAXEND},type_list_addtype_reduce},
    {{closureexpr,FUN,LPAREN,parameter_list,RPAREN,operator_n,type,LBRACE,block,RBRACE,SYNTAXEND},closureexpr_reduce},
    {{closureexpr,FUN,LPAREN,parameter_list,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},closureexpr_rettypeinfer_reduce},
    {{callccexpr,CALLCC,LPAREN,IDENT,COMMA,type,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},callccexpr_reduce},
    {{callccexpr,CALLCC,LPAREN,IDENT,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},callccexpr_void_reduce},
    {{arg_list,EMPTY,SYNTAXEND},arg_list_empty_reduce},
    {{arg_list,expression,SYNTAXEND},arg_list_expression_reduce},
    {{arg_list,arg_list,COMMA,expression,SYNTAXEND},arg_list_addexpression_reduce},
	{ { tuple_list, expression, COMMA, expression, SYNTAXEND }, tuple_list_expression_reduce },
	{ { tuple_list, tuple_list, COMMA, expression, SYNTAXEND }, arg_list_addexpression_reduce },
    {{ifstatement,IF,LPAREN,expression,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},ifstatement_noelse_reduce},
    {{ifstatement,IF,LPAREN,expression,RPAREN,LBRACE,block,RBRACE,ELSE,LBRACE,block,RBRACE,SYNTAXEND},ifstatement_withelse_reduce},
    {{block,statement_list,SYNTAXEND},block_reduce},
    {{whilestatement,WHILE,LPAREN,expression,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},while_reduce},
    {{asyncstatement,ASYNC,expression,SYNTAXEND},async_reduce},
    {{listvalexpr,LBRACKET,arg_list,RBRACKET,SYNTAXEND},listvalexpr_reduce},
    {{tuplevalexpr,LPAREN,tuple_list,RPAREN,SYNTAXEND},tuplevalexpr_reduce},
    {{datadef,DATA,IDENT,LBRACE,datamember_list,RBRACE,SYNTAXEND},datadef_reduce},
    {{groupdef,GROUP,IDENT,LPAREN,IDENT,RPAREN,LBRACE,groupmember_list,RBRACE,SYNTAXEND},groupdef_reduce},
	{{datamember_list,EMPTY,SYNTAXEND},dmlist_empty_reduce},

    {{datamember_list,datamember_list,IDENT,COLON,type,SEMICOLON,SYNTAXEND},dmlist_add_reduce},
    {{datamember_list,datamember_list,IDENT,COLON,type,LINEEND,SYNTAXEND},dmlist_add_reduce},
    {{datamember_list,datamember_list,IDENT,COLON,type,SYNTAXEND},dmlist_add_reduce},

    {{groupmember_list,EMPTY,SYNTAXEND},gmlist_empty_reduce},

    {{groupmember_list,groupmember_list,FUN,IDENT,LPAREN,type_list,RPAREN,SEMICOLON,SYNTAXEND},gmlist_addvoid_reduce},
    {{groupmember_list,groupmember_list,FUN,IDENT,LPAREN,type_list,RPAREN,LINEEND,SYNTAXEND},gmlist_addvoid_reduce},
    {{groupmember_list,groupmember_list,FUN,IDENT,LPAREN,type_list,RPAREN,SYNTAXEND},gmlist_addvoid_reduce},

    {{groupmember_list,groupmember_list,FUN,IDENT,LPAREN,type_list,RPAREN,operator_n,type,SEMICOLON,SYNTAXEND},gmlist_addnonvoid_reduce},
    {{groupmember_list,groupmember_list,FUN,IDENT,LPAREN,type_list,RPAREN,operator_n,type,LINEEND,SYNTAXEND},gmlist_addnonvoid_reduce},
    {{groupmember_list,groupmember_list,FUN,IDENT,LPAREN,type_list,RPAREN,operator_n,type,SYNTAXEND},gmlist_addnonvoid_reduce},

	{{groupmember_list,groupmember_list,IDENT,COLON,type,SEMICOLON,SYNTAXEND},gmlist_addfield_reduce},
    {{groupmember_list,groupmember_list,IDENT,COLON,type,LINEEND,SYNTAXEND},gmlist_addfield_reduce},
    {{groupmember_list,groupmember_list,IDENT,COLON,type,SYNTAXEND},gmlist_addfield_reduce},

    {{dataexpr,IDENT,LBRACE,initassign_list,RBRACE,SYNTAXEND},dataexpr_reduce},
    {{initassign_list,EMPTY,SYNTAXEND},ialist_empty_reduce},
	{{initassign_list,initassign_list,IDENT,operator_n,expression,SYNTAXEND},ialist_add_reduce},
	{{initassign_list,initassign_list,IDENT,operator_n,expression,COMMA,SYNTAXEND},ialist_add_reduce},

	{{listrefexpr,primary,LBRACKET,expression,RBRACKET,SYNTAXEND},listrefexpr_reduce}, //配列・タプルのインデックス参照
	{{listrefexpr,parenexpr,LBRACKET,expression,RBRACKET,SYNTAXEND},listrefexpr_paren_reduce},

	{{datamemberrefexpr,primary,DOT,IDENT,SYNTAXEND},datamemberrefexpr_reduce}, //構造体メンバ参照
	{{datamemberrefexpr,parenexpr,DOT,IDENT,SYNTAXEND},datamemberrefexpr_reduce},

	{{newobjexpr,NEW,LPAREN,type,RPAREN,SYNTAXEND},newobjexpr_reduce},

	{ { functiondef, FUN, operator_n, IDENT,COMMA,IDENT,COMMA,INTVAL,LPAREN, parameter_list, RPAREN, LBRACE, block, RBRACE, SYNTAXEND }, function_operator_norettype_reduce },
	{ { functiondef, FUN, operator_n,  IDENT, COMMA, IDENT, COMMA, INTVAL, LPAREN, parameter_list, RPAREN, operator_n, type, LBRACE, block, RBRACE, SYNTAXEND }, function_operator_withrettype_reduce }
};


bool SHOW_BYTECODE;

int main(int argc,char* argv[])
{
	srand((unsigned int)time(NULL));

	shared_ptr<Lexer> lexer;
	shared_ptr<Parser> parser;

	SHOW_BYTECODE = false;

	char* path=argv[1];
	if (argc == 3){
		if (strcmp(argv[1], "-b") == 0){
			SHOW_BYTECODE = true;
			path = argv[2];
		}
		else{
			error("コマンドライン引数1が不正です");
		}
	}else if(argc!=2){
		error("コマンドライン引数が不正です");
	}
	try{
		ifstream ifs(path);
		string str="",line;
		while (std::getline(ifs, line)) {
			str += line+"\n";
		}

		lexer=make_shared<Lexer>(str.c_str());
		parser=make_shared<Parser>();

		shared_ptr<Compiler> compiler=make_shared<Compiler>(lexer,parser);
		shared_ptr<Executable> executable=compiler->Compile();
		lexer.reset();
		parser.reset();
		compiler.reset();

		if (SHOW_BYTECODE){ getchar(); return 0; }

		cout<<BG_BLUE<<"~~~~~~~~~~~~~~~~~~~~~~~~"<<RESET<<endl;

		auto start = system_clock::now();

		VM::Run(VM::GetInitialFlame(executable),false);

		auto end = system_clock::now();
		auto dur = end - start;
		auto msec = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
		std::cout << endl<<endl<< msec << " msec" << endl;
    }catch(SyntaxError){
        cerr<<BG_RED"Syntax error  line:";
        set<int>::iterator iter;
        parser->error_candidates.insert(lexer->curr_line);
        for(iter=parser->error_candidates.end(),iter--;iter!=parser->error_candidates.end();iter++){
			cerr<<(*iter)<<",";
        }
        cerr<<"\b"<<RESET<<" \b"<<endl;
    }catch(NoMatchRule){
        cerr<<BG_RED<<"トークンを生成できません  line:"<<lexer->curr_line<<RESET<<endl;
    }catch(ParserException){
		cerr<<BG_RED"パーサで問題が発生しました"<<RESET<<endl;
    }catch(LexerException){
		cerr<<BG_RED"レキシカルアナライザで問題が発生しました"<<RESET<<endl;
	}

	//std::chrono::milliseconds dura( 5000 );
	//std::this_thread::sleep_for( dura );

	cout << "finished." << endl;
	getchar();

    return 0;
}
