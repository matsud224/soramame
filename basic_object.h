#pragma once

#include <memory>

class Flame;

class ClosureObject{
public:
	shared_ptr<FunctionObject> FunctionRef;
	shared_ptr<Flame> ParentFlame;

	ClosureObject(shared_ptr<FunctionObject> fref,shared_ptr<Flame> parent):FunctionRef(fref),ParentFlame(parent){};
};

class DataObject{
public:
	string OriginalType;
	shared_ptr<map<string,int> > MemberMap;

	DataObject(string original,shared_ptr<map<string,int> > membermap):OriginalType(original),MemberMap(membermap){}
};


class FunctionObject{
public:
	string Name;
	bool isBuiltin;
	shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > Args;
	shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > LocalVariables;
	shared_ptr<vector<int> > ChildPoolIndex;
	shared_ptr<vector<int> > bytecodes;

	FunctionObject(string name,bool isbuiltin,shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > args,shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > localvars,shared_ptr<vector<int> > bc):Name(name),isBuiltin(isbuiltin),Args(args),LocalVariables(localvars),bytecodes(bc){}
};
