#pragma once

#include <string>
#include <vector>
#include <map>
#include <list>
//#include "utility.h"
#include <memory>

using namespace std;

class TypeAST{
public:
	virtual ~TypeAST(){}
	virtual string GetName()=0;
};

//単一の型
class BasicTypeAST : public TypeAST{
public:
    BasicTypeAST(string n):Name(n){}
    string Name;
    virtual string GetName(){
		return Name;
    }
};

//関数型
class FunctionTypeAST : public TypeAST{
public:
	FunctionTypeAST(vector<shared_ptr<TypeAST> > t_list):TypeList(t_list){}
	vector<shared_ptr<TypeAST> > TypeList;
	virtual string GetName(){
		string s="fun(";
		for(int i=0;i<TypeList.size()-1;i++){
			s+=TypeList[i]->GetName();
			if(i!=TypeList.size()-2){
				s+=",";
			}
		}
		s+=")=>"+TypeList[TypeList.size()-1]->GetName();
		return s;
	}

};

//継続型
class ContinuationTypeAST : public TypeAST{
public:
	ContinuationTypeAST(shared_ptr<TypeAST> t):Type(t){}
	shared_ptr<TypeAST> Type;
	virtual string GetName(){
		string s="continuation(";
		s+=Type->GetName();
		s+=")";
		return s;
	}

};

//チャンネル型
class ChannelTypeAST : public TypeAST{
public:
	ChannelTypeAST(shared_ptr<TypeAST> t):Type(t){}
	shared_ptr<TypeAST> Type;
	virtual string GetName(){
		string s="channel(";
		s+=Type->GetName();
		s+=")";
		return s;
	}

};

//リスト型
class ListTypeAST : public TypeAST{
public:
    ListTypeAST(shared_ptr<TypeAST> t):ContainType(t){}
    shared_ptr<TypeAST>  ContainType;
    virtual string GetName(){
		if (ContainType == nullptr){
			return "[]";
		}
		return "["+ContainType->GetName()+"]";
    }
};

//タプル型
class TupleTypeAST : public TypeAST{
public:
    TupleTypeAST(vector<shared_ptr<TypeAST> > t_list):ContainTypeList(t_list){}
    vector<shared_ptr<TypeAST> > ContainTypeList;
    virtual string GetName(){
		string name="(";
		for(int i=0;i<ContainTypeList.size();i++){
			name+=ContainTypeList[i]->GetName();
			if(i!=ContainTypeList.size()-1){
				name+=",";
			}
		}
		name+=")";
		return name;
    }
};


string Var2Str(pair<string, shared_ptr<TypeAST>> var);