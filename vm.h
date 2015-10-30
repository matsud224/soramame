#pragma once
#include <stack>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include <time.h>
#include "compiler.h"

using namespace std;

class CodegenInfo;
class Executable;
class FunctionObject;

class Flame{
public:
    shared_ptr< vector< pair<string,VMValue> > > Variables; //型検査には通ってるので型情報は保管しない（変数名と値のペア）
    vector< VMValue > OperandStack; //ここで計算を行う
    shared_ptr<vector<int> > CodePtr; //バイトコードへのポインタ
    shared_ptr<Flame> StaticLink;
    shared_ptr<Flame> DynamicLink;
	shared_ptr<FunctionObject> FunctionInfo;
    int PC;
    Flame(shared_ptr< vector< pair<string,VMValue> > > vars,shared_ptr< vector<int> > codeptr,shared_ptr<Flame> dynamiclink,shared_ptr<Flame> staticlink,shared_ptr<FunctionObject> finfo):Variables(vars),CodePtr(codeptr),DynamicLink(dynamiclink),StaticLink(staticlink),FunctionInfo(finfo){
    	PC=0;
    	OperandStack.reserve(32);
	}
};

class VM{
public:
	static map<pair<string,string>, void (*)(shared_ptr<Flame>) > BuiltinFunctionList; //関数名と型名から関数を引っ張ってくる
	static ConstantPool PublicConstantPool;
    static shared_ptr<Flame> GetInitialFlame(shared_ptr<Executable>);
    static VMValue Run(shared_ptr<Flame>,bool); //trueで、呼び出し時点のフレームがポップされたら関数を抜ける
};


