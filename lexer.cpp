#include "lexer.h"
#include "common.h"
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "utility.h"

using namespace std;

TokenValue Lexer::dummy;

bool isoperatorchar(char c){
	switch(c){
	case '%': case '$': case '#': case '=': case '~':
	case '|': case '^': case '+': case '-': case '*':
	case '/': case '<': case '>': case '&': case '!':
	case '?': case '@':
		return true;
	default:
		return false;
	}
}

pair<Symbol,TokenValue> Lexer::Get()
{

	retry:
    char c;
    TokenValue t; t=TokenValue();
    string buffer; buffer.reserve(32);
    if(continue_comment){goto blockcomment_resume;}
    while(c=ifs.get(),isspace(c)){
		if(c=='\n'){this->curr_line++; return pair<Symbol,TokenValue>(LINEEND ,Lexer::dummy);}
    }

    if(c==EOF){
		return pair<Symbol,TokenValue>(INPUTEND,dummy);
    }else if(c=='/' && ifs.peek()=='*'){
		//ブロックコメント
		static int level=0;
		level++;
		blockcomment_resume:
		if(continue_comment){continue_comment=false;}
		else{ifs.get();}

		while(true){
			c=ifs.get();
			if(c==EOF){
				error("ブロックコメントが正しくありません： */ が不足しています");
			}else if(c=='\n'){
				continue_comment=true;
				return pair<Symbol,TokenValue>(LINEEND ,Lexer::dummy);
			}else if(c=='/' && ifs.peek()=='*'){
				ifs.get();
				level++;
			}else if(c=='*' && ifs.peek()=='/'){
				ifs.get();
				level--;
				if(level==0){
					break;
				}
			}
		}
		goto retry;
    }else if(isalpha(c) || c=='_'){
		buffer.append(1,c);
		while(c=ifs.peek(),isalnum(c)||c=='_'){
			buffer.append(1,c);
			ifs.get();
		}
		if(buffer=="var"){
			return pair<Symbol,TokenValue>(VAR ,Lexer::dummy);
		}else if(buffer=="fun"){
			return pair<Symbol,TokenValue>(FUN ,Lexer::dummy);
		}else if(buffer=="newchannel"){
			return pair<Symbol,TokenValue>(NEWCHAN ,Lexer::dummy);
		}else if(buffer=="channel"){
			return pair<Symbol,TokenValue>(CHANNEL ,Lexer::dummy);
		}else if(buffer=="if"){
			return pair<Symbol,TokenValue>(IF ,Lexer::dummy);
		}else if(buffer=="while"){
			return pair<Symbol,TokenValue>(WHILE ,Lexer::dummy);
		}else if(buffer=="else"){
			return pair<Symbol,TokenValue>(ELSE ,Lexer::dummy);
		}else if(buffer=="return"){
			return pair<Symbol,TokenValue>(RETURN_S ,Lexer::dummy);
		}else if(buffer=="true"){
			t.boolval=true;
			return pair<Symbol,TokenValue>(BOOLVAL , t);
		}else if(buffer=="false"){
			t.boolval=false;
			return pair<Symbol,TokenValue>(BOOLVAL , t);
		}else if(buffer=="data"){
			return pair<Symbol,TokenValue>(DATA ,Lexer::dummy);
		}else if(buffer=="group"){
			return pair<Symbol,TokenValue>(GROUP ,Lexer::dummy);
		}else if(buffer=="continuation"){
			return pair<Symbol,TokenValue>(CONTINUATION ,Lexer::dummy);
		}else if(buffer=="callcc"){
			return pair<Symbol,TokenValue>(CALLCC ,Lexer::dummy);
		}else if(buffer=="async"){
			return pair<Symbol,TokenValue>(ASYNC ,Lexer::dummy);
		}else if(buffer=="newvector"){
			return pair<Symbol,TokenValue>(NEWVECTOR ,Lexer::dummy);
		}else if(buffer=="vector"){
			return pair<Symbol,TokenValue>(VECTOR ,Lexer::dummy);
		}else{
			t.str=buffer;
			return pair<Symbol,TokenValue>(IDENT ,t);
		}
    }else if(isdigit(c)){
		//数字
		bool isdouble=false;
		stringstream ss;
		buffer.append(1,c);
		while(c=ifs.peek(),isdigit(c)||c=='.'){
			if(c=='.'){ isdouble=true; }
			buffer.append(1,c);
			ifs.get();
		}
		ss<<buffer;
		if(isdouble){
			ss>>t.doubleval;
			return pair<Symbol,TokenValue>(DOUBLEVAL ,t);
		}else{
			ss>>t.intval;
			return pair<Symbol,TokenValue>(INTVAL ,t);
		}
	}else if(c=='/' && ifs.peek()=='/'){
		//行コメント
		while(c=ifs.get(),c!='\n'){ }
		return pair<Symbol,TokenValue>(LINEEND ,Lexer::dummy);
    }else if(c=='\"'){
		//文字列リテラル
		while(c=ifs.peek(),isascii(c)){
			if(c=='\"'){ ifs.get(); break; }
			buffer.append(1,c);
			if(c=='\\'){ifs.get();buffer.append(1,ifs.peek());}
			ifs.get();
		}
		t.str=escape_str(buffer);
		return pair<Symbol,TokenValue>(STRINGVAL ,t);
    }else if(c=='\''){
		//文字リテラル
		while(c=ifs.peek(),isascii(c)){
			if(c=='\''){ ifs.get(); break; }
			buffer.append(1,c);
			if(c=='\\'){ifs.get();buffer.append(1,ifs.peek());}
			ifs.get();
		}
		t.intval=escape_str(buffer)[0];
		return pair<Symbol,TokenValue>(INTVAL ,t);
    }else if(isoperatorchar(c)){
		//演算子
		buffer.append(1,c);
		while(c=ifs.peek(),isoperatorchar(c)){
			buffer.append(1,c);
			ifs.get();
		}
		t.str=buffer;
		return pair<Symbol,TokenValue>(OPERATOR ,t);
    }else{
		switch(c){
		case ';':
			return pair<Symbol,TokenValue>(SEMICOLON ,Lexer::dummy);
		case ':':
			return pair<Symbol,TokenValue>(COLON ,Lexer::dummy);
		case ',':
			return pair<Symbol,TokenValue>(COMMA ,Lexer::dummy);
		case '.':
			return pair<Symbol,TokenValue>(DOT ,Lexer::dummy);
		case '(':
			return pair<Symbol,TokenValue>(LPAREN ,Lexer::dummy);
		case ')':
			return pair<Symbol,TokenValue>(RPAREN ,Lexer::dummy);
		case '{':
			return pair<Symbol,TokenValue>(LBRACE ,Lexer::dummy);
		case '}':
			return pair<Symbol,TokenValue>(RBRACE ,Lexer::dummy);
		case '[':
			return pair<Symbol,TokenValue>(LBRACKET ,Lexer::dummy);
		case ']':
			return pair<Symbol,TokenValue>(RBRACKET ,Lexer::dummy);
		default:
			throw NoMatchRule();
		}
    }
}
