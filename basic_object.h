#pragma once

class Flame;

class ClosureObject{
public:
	int PoolIndex;
	Flame* ParentFlame;

	ClosureObject(int index,Flame* parent):PoolIndex(index),ParentFlame(parent){};
};

class DataObject{
public:
	string OriginalType;
	map<string,int>* MemberMap;

	DataObject(string original,map<string,int>* membermap):OriginalType(original),MemberMap(membermap){}
};
