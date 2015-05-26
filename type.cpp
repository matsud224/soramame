#include <string>
#include <map>
#include <memory>
#include "type.h"

//変数名、TypeASTのペアから、デバッグ用に文字列を生成する
string Var2Str(pair<string, shared_ptr<TypeAST>> var){
	return var.first + ":" + var.second->GetName();
}