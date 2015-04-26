#pragma once
#include <stack>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "basic_object.h"

using namespace std;

class CodegenInfo;

enum{
    ipush,bpush,
    ldc,
    iadd,dadd,
    isub,dsub,
    imul,dmul,
    idiv,ddiv,
    band,bor,
    imod,
    ineg,bnot,dneg,
    ilshift,irshift,
    invoke,
    loadlocal,loadbyindex,loadfield,
	ret,ret_withvalue,
    storelocal,storefield,storebyindex,
    makeclosure,
    skip,iffalse_skip,
    icmpeq,icmpne,icmplt,icmple,icmpgt,icmpge,
    dcmpeq,dcmpne,dcmplt,dcmple,dcmpgt,dcmpge,
    bcmpeq,bcmpne,
    back,
    makelist,
    makedata
};

class VMValue{
public:
	int int_value;
	bool bool_value;
	double double_value;
	shared_ptr<void> ref_value;
};

class Flame{
public:
    shared_ptr< vector< pair<string,VMValue> > > Variables; //型検査には通ってるので型情報は保管しない（変数名と値のペア）
    stack<shared_ptr<VMValue> > OperandStack; //ここで計算を行う
    shared_ptr<vector<int> > CodePtr; //バイトコードへのポインタ
    shared_ptr<Flame> StaticLink;
    int PC;
    bool NoChildren;
    Flame(shared_ptr< vector< pair<string,VMValue> > > vars,shared_ptr< vector<int> > codeptr,shared_ptr<Flame> staticlink):Variables(vars),CodePtr(codeptr),StaticLink(staticlink){PC=0;NoChildren=false;}
};

class VM{
public:
	shared_ptr<CodegenInfo> CodeInfo;
	vector<shared_ptr<Flame> > Environment;
    VM( shared_ptr<CodegenInfo> cinfo):CodeInfo(cinfo){}
    void Init();
    int Run(bool currflame_only); //trueで、呼び出し時点のフレームがポップされたら関数を抜ける
};



