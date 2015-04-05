#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <string.h>
#include <iostream>
#include "common.h"
#include "oniguruma.h"

using namespace std;


class Lexer;

enum State{INITIAL,COMMENT};

struct TokenRule{
    char *rule;
    State state;
    bool hasValue;
    pair<Symbol,TokenValue> (* callback)(char *str,Lexer *lexer);
};

extern TokenRule TOKENRULE[];

class LexerException{

};

class NoMatchRule : public LexerException{

};

class OnigurumaException : public LexerException{
public:
	unsigned char *Message;
	OnigurumaException(unsigned char *message):Message(message){}
};

class Lexer{
private:
    const char *rawcode;
    regex_t* regex_objs[TOKENRULECOUNT];
public:
	static TokenValue dummy;

	int curr_index;
	State curr_state;
	int curr_line;

    Lexer(const char *code):rawcode(code){
        curr_index=0;
        curr_line=1;
        curr_state=INITIAL;

        int r;
        OnigErrorInfo einfo;
        for(int i=0;i<TOKENRULECOUNT;i++){
            r=onig_new(&regex_objs[i],reinterpret_cast<OnigUChar*>(TOKENRULE[i].rule),reinterpret_cast<OnigUChar*>(TOKENRULE[i].rule+strlen(TOKENRULE[i].rule)),
                       ONIG_OPTION_DEFAULT,ONIG_ENCODING_ASCII,ONIG_SYNTAX_DEFAULT,&einfo);
            if(r!=ONIG_NORMAL){
                static unsigned char s[ONIG_MAX_ERROR_MESSAGE_LEN];
                onig_error_code_to_str(s,r,&einfo);
                throw OnigurumaException(s);
                return;
            }
        }
    };
    pair<Symbol,TokenValue> Get();
};
