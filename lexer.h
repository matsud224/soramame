#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <string.h>
#include <iostream>
#include "common.h"
#include <regex>
#include <memory>
#include "exceptions.h"

using namespace std;


class Lexer;

enum State{INITIAL,COMMENT};

class TokenRule{
public:
    string rule;
    State state;
    bool hasValue;
    pair<Symbol,TokenValue> (* callback)(string str,Lexer* lexer);
};

extern TokenRule TOKENRULE[];



class Lexer{
private:
    const char *rawcode;
    regex regex_objs[TOKENRULECOUNT];
public:
	static TokenValue dummy;

	int curr_index;
	State curr_state;
	int curr_line;

	Lexer(const char *code) :rawcode(code){
		curr_index = 0;
		curr_line = 1;
		curr_state = INITIAL;

		for (int i = 0; i < TOKENRULECOUNT; i++){
			//cout << i << "番目の正規表現オブジェクトを生成します" << endl;
			try{
				regex_objs[i] = regex(TOKENRULE[i].rule);
			}
			catch (const std::regex_error& rerr)
			{
				std::cout << "regex error: "
					<< rerr.what()
					<< std::endl;
			}
		}
    };
    pair<Symbol,TokenValue> Get();
};
