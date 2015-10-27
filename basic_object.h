#pragma once

#include <thread>
#include <memory>
#include "ast_etc.h"
#include "statement.h"
#include "type.h"
#include "expression.h"
#include <condition_variable>

class Flame;
class FunctionObject;

class ClosureObject{
public:
	shared_ptr<FunctionObject> FunctionRef;
	shared_ptr<Flame> ParentFlame;

	ClosureObject(shared_ptr<FunctionObject> fref,shared_ptr<Flame> parentflame):FunctionRef(fref),ParentFlame(parentflame){};
};

class ContinuationObject{
public:
	vector<pair<int,vector<VMValue> > > Snapshot;
	shared_ptr<Flame> StartFlame;

	ContinuationObject(vector<pair<int,vector<VMValue> > > snapshot,shared_ptr<Flame> startflame):Snapshot(snapshot),StartFlame(startflame){};
};

class ChannelObject{
public:
	queue<VMValue> SentValues;
	queue<pair<shared_ptr<condition_variable>,bool*> > Receivers;
	queue<pair<shared_ptr<condition_variable>,bool*> > Senders;
	queue<bool> Sender_ProduceValue;
	mutex mtx;

	int Capacity;

	ChannelObject(int capacity){this->Capacity=capacity;};
};

class DataObject{
public:
	string OriginalType;
	shared_ptr<map<string,VMValue> > MemberMap;

	DataObject(string original,shared_ptr<map<string,VMValue> > membermap):OriginalType(original),MemberMap(membermap){}
};


class FunctionObject{
public:
	string Name;
	bool isBuiltin;
	shared_ptr<TypeAST> TypeInfo;
	shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > Args;
	shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > LocalVariables;
	shared_ptr<vector<int> > bytecodes;

	FunctionObject(string name,shared_ptr<TypeAST> type,bool isbuiltin,shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > args,shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > localvars,shared_ptr<vector<int> > bc):Name(name),TypeInfo(type),isBuiltin(isbuiltin),Args(args),LocalVariables(localvars),bytecodes(bc){}
	FunctionObject(string name,shared_ptr<TypeAST> type,bool isbuiltin,shared_ptr<vector<pair<string,shared_ptr<TypeAST> > > > args):Name(name),TypeInfo(type),isBuiltin(isbuiltin),Args(args){}
};
