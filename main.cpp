#include <iostream>
#include "lexer.h"
#include <string>
#include <vector>
#include "ast.h"
#include "vm.h"
#include "utility.h"
#include <map>
#include <set>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include "parser.h"
#include "lexer.h"
#include "color_text.h"
#include "common.h"
#include <time.h>
#include <math.h>
#include "parser_actions.h"
#include <readline/readline.h>


using namespace std;

pair<Symbol,TokenValue> commentstart_lex(char *str,Lexer *lex){
	lex->curr_state=COMMENT;
	return pair<Symbol,TokenValue>(INPUTEND ,Lexer::dummy);
};
pair<Symbol,TokenValue> commentend_lex(char *str,Lexer *lex){
	lex->curr_state=INITIAL;
	return pair<Symbol,TokenValue>(INPUTEND ,Lexer::dummy);
};
pair<Symbol,TokenValue> var_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(VAR ,Lexer::dummy);};
pair<Symbol,TokenValue> fun_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(FUN ,Lexer::dummy);};
pair<Symbol,TokenValue> if_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(IF ,Lexer::dummy);};
pair<Symbol,TokenValue> while_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(WHILE ,Lexer::dummy);};
pair<Symbol,TokenValue> for_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(FOR ,Lexer::dummy);};
pair<Symbol,TokenValue> else_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(ELSE ,Lexer::dummy);};
pair<Symbol,TokenValue> return_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(RETURN_S ,Lexer::dummy);};
pair<Symbol,TokenValue> boolval_lex(char *str,Lexer *lex){
	TokenValue t;
	string val(str);
	if(val=="true"){
		t.boolval=true;
	}else{
		t.boolval=false;
	}
	return pair<Symbol,TokenValue>(BOOLVAL , t);
};
pair<Symbol,TokenValue> semicolon_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(SEMICOLON ,Lexer::dummy);};
pair<Symbol,TokenValue> colon_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(COLON ,Lexer::dummy);};
pair<Symbol,TokenValue> comma_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(COMMA ,Lexer::dummy);};
pair<Symbol,TokenValue> lparen_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(LPAREN ,Lexer::dummy);};
pair<Symbol,TokenValue> rparen_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(RPAREN ,Lexer::dummy);};
pair<Symbol,TokenValue> lbrace_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(LBRACE ,Lexer::dummy);};
pair<Symbol,TokenValue> rbrace_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(RBRACE ,Lexer::dummy);};
pair<Symbol,TokenValue> lbracket_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(LBRACKET ,Lexer::dummy);};
pair<Symbol,TokenValue> rbracket_lex(char *str,Lexer *lex){return pair<Symbol,TokenValue>(RBRACKET ,Lexer::dummy);};
pair<Symbol,TokenValue> ident_lex(char *str,Lexer *lex){
	TokenValue t;
	t.str=str;
	return pair<Symbol,TokenValue>(IDENT ,t);
};
pair<Symbol,TokenValue> operator_lex(char *str,Lexer *lex){
	TokenValue t;
	t.str=str;
	return pair<Symbol,TokenValue>(OPERATOR ,t);
};
pair<Symbol,TokenValue> intval_lex(char *str,Lexer *lex){
	TokenValue t;
	t.intval=atoi(str);
	return pair<Symbol,TokenValue>(INTVAL ,t);
};
pair<Symbol,TokenValue> stringval_lex(char *str,Lexer *lex){
	TokenValue t;
	string str_temp(str);
	//ダブルクオーテーションを削除
	str_temp.erase(str_temp.begin());
	str_temp.erase(str_temp.end()-1);
	char *str_a=new char[str_temp.length()+1];
	for(int i=0;i<str_temp.length()+1;i++){
		str_a[i]=str_temp[i];
	}
	string escaped=escape_str(str_a);
	char *str_final=new char[escaped.length()+1];
	for(int i=0;i<escaped.length()+1;i++){
		str_final[i]=escaped[i];
	}
	t.str=str_final;
	return pair<Symbol,TokenValue>(STRINGVAL ,t);
};
pair<Symbol,TokenValue> nextline_lex(char *str,Lexer *lex){
	lex->curr_line++;
	return pair<Symbol,TokenValue>(INPUTEND ,Lexer::dummy);
};

TokenRule TOKENRULE[TOKENRULECOUNT]={
	{"#\\{",INITIAL,false,commentstart_lex},
	{"#.*[\\r\\n|\\n|\\r]",INITIAL,false,NULL},
	{"[\\r\\n|\\n|\\r]",INITIAL,false,nextline_lex},
	{"var",INITIAL,true,var_lex},
	{"fun",INITIAL,true,fun_lex},
    {"if",INITIAL,true,if_lex},
    {"while",INITIAL,true,while_lex},
    {"for",INITIAL,true,for_lex},
    {"else",INITIAL,true,else_lex},
    {"return",INITIAL,true,return_lex},
    {"true",INITIAL,true,boolval_lex},
    {"false",INITIAL,true,boolval_lex},
    {";",INITIAL,true,semicolon_lex},
	{":",INITIAL,true,colon_lex},
    {",",INITIAL,true,comma_lex},
    {"\\(",INITIAL,true,lparen_lex},
    {"\\)",INITIAL,true,rparen_lex},
    {"\\{",INITIAL,true,lbrace_lex},
    {"\\}",INITIAL,true,rbrace_lex},
    {"\\[",INITIAL,true,lbracket_lex},
    {"\\]",INITIAL,true,rbracket_lex},
    {"[a-zA-Z_][a-zA-Z0-9_]*",INITIAL,true,ident_lex},
    {"[%=~\\|\\^\\+\\-\\*/<>&!]{1,3}",INITIAL,true,operator_lex},
    {"\\d+",INITIAL,true,intval_lex},
    {"\"(?>[^\\\\\"]|\\\\.)*?\"",INITIAL,true,stringval_lex},
    {"\\s+",INITIAL,false,NULL}, //空白は読み飛ばす（コールバック関数をNULLにしておく）

    {"}#",COMMENT,false,commentend_lex},
    {"[\\r\\n|\\n|\\r]",COMMENT,false,nextline_lex},
    {".",COMMENT,false,NULL}
};

SyntaxRule SYNTAXRULE[SYNTAXRULECOUNT]={
    {{S,program,SYNTAXEND},success},
    {{program,EMPTY,SYNTAXEND},program_empty_reduce},
    {{program,program,function,SYNTAXEND},program_addfundef_reduce},
    {{program,program,VAR,variabledef_list,SEMICOLON,SYNTAXEND},program_addvardefreduce},
    {{intvalexpr,INTVAL,SYNTAXEND},intvalexpr_reduce},
    {{boolvalexpr,BOOLVAL,SYNTAXEND},boovalexpr_reduce},
    {{stringvalexpr,STRINGVAL,SYNTAXEND},stringvalexpr_reduce},
    {{operator_n,OPERATOR,SYNTAXEND},operator_n_reduce},
    {{function,FUN,IDENT,LPAREN,parameter_list,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},function_norettype_reduce},
    {{function,FUN,IDENT,LPAREN,parameter_list,RPAREN,operator_n,type,LBRACE,block,RBRACE,SYNTAXEND},function_withrettype_reduce},
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
    {{statement,expression,SEMICOLON,SYNTAXEND},statement_expression_reduce},
    {{statement,returnstatement,SEMICOLON,SYNTAXEND},NULL},
    {{statement,ifstatement,SYNTAXEND},NULL},
    {{statement,whilestatement,SYNTAXEND},NULL},
    {{statement,forstatement,SYNTAXEND},NULL},
    {{expression,primary,SYNTAXEND},expression_primary_reduce},
    {{expression,parenexpr,SYNTAXEND},NULL},
    {{expression,expression,primary,SYNTAXEND},expression_add_reduce},
    {{expression,expression,parenexpr,SYNTAXEND},expression_addparen_reduce},
    {{funcallexpr,funcallexpr,LPAREN,arg_list,RPAREN,SYNTAXEND},funcallexpr_reduce},
    {{funcallexpr,closureexpr,LPAREN,arg_list,RPAREN,SYNTAXEND},funcallexpr_reduce},
    {{funcallexpr,variableexpr,LPAREN,arg_list,RPAREN,SYNTAXEND},funcallexpr_reduce},
    {{primary,intvalexpr,SYNTAXEND},NULL},
    {{primary,boolvalexpr,SYNTAXEND},NULL},
    {{primary,stringvalexpr,SYNTAXEND},NULL},
    {{primary,funcallexpr,SYNTAXEND},NULL},
    {{primary,closureexpr,SYNTAXEND},NULL},
    {{primary,variableexpr,SYNTAXEND},NULL},
    {{primary,listvalexpr,SYNTAXEND},NULL},
	{{primary,operator_n,SYNTAXEND},NULL},
    {{variableexpr,IDENT,SYNTAXEND},variableexpr_reduce},
    {{parenexpr,LPAREN,expression,RPAREN,SYNTAXEND},parenexpr_reduce},
    {{returnstatement,RETURN_S,expression,SYNTAXEND},returnstatement_reduce},
	{{returnstatement,RETURN_S,SYNTAXEND},returnstatement_noexp_reduce},
    {{type,IDENT,SYNTAXEND},type_normal_reduce},
    {{type,FUN,LPAREN,type_list,RPAREN,operator_n,type,SYNTAXEND},type_fun_reduce},
    {{type,LBRACKET,type,RBRACKET,SYNTAXEND},type_listtype_reduce},
    {{type_list,EMPTY,SYNTAXEND},type_list_empty_reduce},
    {{type_list,type,SYNTAXEND},type_list_type_reduce},
    {{type_list,type_list,COMMA,type,SYNTAXEND},type_list_addtype_reduce},
    {{closureexpr,FUN,LPAREN,parameter_list,RPAREN,operator_n,type,LBRACE,block,RBRACE,SYNTAXEND},closureexpr_reduce},
    {{closureexpr,FUN,LPAREN,parameter_list,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},closureexpr_rettypeinfer_reduce},
    {{arg_list,EMPTY,SYNTAXEND},arg_list_empty_reduce},
    {{arg_list,expression,SYNTAXEND},arg_list_expression_reduce},
    {{arg_list,arg_list,COMMA,expression,SYNTAXEND},arg_list_addexpression_reduce},
    {{ifstatement,IF,LPAREN,expression,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},ifstatement_noelse_reduce},
    {{ifstatement,IF,LPAREN,expression,RPAREN,LBRACE,block,RBRACE,ELSE,LBRACE,block,RBRACE,SYNTAXEND},ifstatement_withelse_reduce},
    {{block,statement_list,SYNTAXEND},block_reduce},
    {{whilestatement,WHILE,LPAREN,expression,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},while_reduce},
    {{forstatement,FOR,LPAREN,statement,SEMICOLON,expression,SEMICOLON,statement,RPAREN,LBRACE,block,RBRACE,SYNTAXEND},for_reduce},
    {{listvalexpr,LBRACKET,arg_list,RBRACKET,SYNTAXEND},listvalexpr_reduce},
};




int main()
{
	srand((unsigned int)time(NULL));

	Lexer *lexer;
	Parser parser;
	try{
		string str="";
		char c;
		while((c=getchar())!=-1){
			str+=c;
		}
		lexer=new Lexer(str.c_str());

		Compiler compiler(lexer,&parser);
		compiler.Compile();
    }catch(SyntaxError){
        cerr<<BG_RED"Syntax error  line:";
        set<int>::iterator iter;
        parser.error_candidates.insert(lexer->curr_line);
        for(iter=parser.error_candidates.begin();iter!=parser.error_candidates.end();iter++){
			cerr<<(*iter)<<",";
        }
        cerr<<RESET<<endl;
    }catch(NoMatchRule){
        cerr<<BG_RED"トークンを生成できません"RESET<<endl;
    }catch(OnigurumaException ex){
    	cerr<<BG_RED"正規表現エンジン 鬼車で問題が発生しました："<<ex.Message<<RESET;
    }catch(ParserException){
		cerr<<BG_RED"パーサで問題が発生しました"RESET<<endl;
    }catch(LexerException){
		cerr<<BG_RED"レキシカルアナライザで問題が発生しました"RESET<<endl;
    }catch(...){
		cerr<<BG_RED"Unknown error"<<RESET<<endl;
	}

    return 0;
}

/*
int main(int argc, char* argv[])
{
    if(argc==2){
        string code(argv[1]);
        cout<<"入力:"<<endl<<code<<endl<<endl;
        Lexer lexer(code);


        lexer.SetOrigin();
        cout<<"ASTを生成しています..."<<endl;;
        Parser parser(&lexer);
        parser.Parse();
        cout<<"完了"<<endl;
        cout<<"型をチェックしています...";
        parser.TypeCheck();
        cout<<"完了"<<endl;
        cout<<"コードを生成しています...";
        parser.Codegen();
        cout<<"完了"<<endl<<endl<<"実行します..."<<endl;
        CodegenInfo cgi=parser.GetCGInfo();
        VM vm(&cgi);
        vm.Run();
        cout<<endl<<"終了しました。"<<endl;
    }else if(argc==3){
        string code(argv[2]);
        if(string(argv[1])=="-t"){
            cout<<"入力:"<<endl<<code<<endl<<endl;
            Lexer lexer(code);


            lexer.SetOrigin();
            cout<<"ASTを生成しています..."<<endl;;
            Parser parser(&lexer);
            parser.Parse();
            cout<<"完了"<<endl;
            cout<<"型をチェックしています...";
            parser.TypeCheck();
            cout<<"完了"<<endl;
            cout<<"コードを生成しています...";
            parser.Codegen();
            cout<<"完了"<<endl;
            CodegenInfo cgi=parser.GetCGInfo();
        }else if(string(argv[1])=="-s"){
            cout<<"未実装です"<<endl;
        }else{
            error(string(argv[1])+":無効なオプションです。");
        }
	}else{
		error("コマンドライン引数を確認して下さい。");
		return -1;
	}

	getch();

    return 0;
}



    lexerのテスト用
    cout<<"lexer:"<<endl;
    while(lexer.CurrentToken!=token_eof){
        lexer.GetNextToken();
        switch(lexer.CurrentToken){
            case token_eof:
                cout<<"<eof>"<<endl;
                break;
            case token_func:
                cout<<"<func>"<<endl;
                break;
            case token_identifier:
                cout<<"<identifier> "<<lexer.CurrentIdentifier<<endl;
                break;
            case token_intval:
                cout<<"<intval> "<<lexer.CurrentIntVal<<endl;
                break;
            case token_floatval:
                cout<<"<floatval> "<<lexer.CurrentFloatVal<<endl;
                break;
            case token_operator:
                cout<<"<operator> "<<lexer.CurrentOperator<<endl;
                break;
            case token_boolval:
                cout<<"<bool> "<<lexer.CurrentFloatVal<<endl;
                break;
            case token_stringval:
                cout<<"<stringval>"<<lexer.CurrentStringVal<<endl;
                break;
            default:
                cout<<"<char>"<<(char)(lexer.CurrentToken)<<endl;
                break;
        }
    }
    cout<<endl;*/

