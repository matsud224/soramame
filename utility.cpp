#include <string>
#include <cstdlib>
#include <iostream>
#include "utility.h"
#include "lexer.h"
#include <sstream>
using namespace std;

void error(string msg)
{
    cerr<<msg<<endl;
    abort();
    return;
}

string IntToString(int number)
{
  stringstream ss;
  ss << number;
  return ss.str();
}

string escape_str(string str){
	string escaped=str;
	int result;
	//改行
	while(true){
		result=escaped.find("\\n",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\n");
	}
	//￥
	while(true){
		result=escaped.find("\\\\",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\\");
	}
	//水平タブ
	while(true){
		result=escaped.find("\\t",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\t");
	}
	//垂直タブ
	while(true){
		result=escaped.find("\\v",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\v");
	}
	//バックスペース
	while(true){
		result=escaped.find("\\b",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\b");
	}
	//シングルクォーテーション
	while(true){
		result=escaped.find("\\\'",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\'");
	}
	//ダブルクォーテーション
	while(true){
		result=escaped.find("\\\"",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\"");
	}
	//ベル文字
	while(true){
		result=escaped.find("\\a",0);
		if(result==string::npos){break;}
		escaped.replace(result,2,"\a");
	}

	return escaped;
}
