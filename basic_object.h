#pragma once

class Flame;

class ClosureObject{
public:
	int PoolIndex;
	Flame* ParentFlame;

	ClosureObject(int index,Flame* parent):PoolIndex(index),ParentFlame(parent){};
};

class ContinuationObject{
public:
	Flame* ExecFlame;
	int PC;
};
