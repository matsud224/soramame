#include <string>
#include <map>
#include <memory>
#include "type.h"

//�ϐ����ATypeAST�̃y�A����A�f�o�b�O�p�ɕ�����𐶐�����
string Var2Str(pair<string, shared_ptr<TypeAST>> var){
	return var.first + ":" + var.second->GetName();
}