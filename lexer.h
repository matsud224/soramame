#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>
#include <string.h>
#include <iostream>
#include <fstream>
#include "common.h"
#include <memory>
#include "exceptions.h"

using namespace std;


class Lexer{
private:
    ifstream& ifs;
public:
	static TokenValue dummy;

	int curr_line;
	bool continue_comment=false;

	Lexer(ifstream &s):ifs(s){
		curr_line=1;
    };
    pair<Symbol,TokenValue> Get();
};
