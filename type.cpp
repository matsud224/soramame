#include <string>
#include <map>
#include <memory>
#include "type.h"

string Var2Str(pair<string, shared_ptr<TypeAST>> var){
	return var.first + ":" + var.second->GetName();
}
