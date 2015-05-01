#pragma once
#include <stack>
#include <vector>
#include <map>
#include <string>
#include <memory>
#include "compiler.h"

using namespace std;

class CodegenInfo;
class Executable;

enum{
    ipush,bpush,pushnull,
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
    makelist,makedata,
    makecontinuation,resume_continuation
};

class Flame{
public:
    shared_ptr< vector< pair<string,VMValue> > > Variables; //型検査には通ってるので型情報は保管しない（変数名と値のペア）
    stack< VMValue > OperandStack; //ここで計算を行う
    shared_ptr<vector<int> > CodePtr; //バイトコードへのポインタ
    shared_ptr<Flame> StaticLink;
    shared_ptr<Flame> DynamicLink;
    int PC;
    Flame(shared_ptr< vector< pair<string,VMValue> > > vars,shared_ptr< vector<int> > codeptr,shared_ptr<Flame> dynamiclink,shared_ptr<Flame> staticlink):Variables(vars),CodePtr(codeptr),DynamicLink(dynamiclink),StaticLink(staticlink){
    	PC=0;
	}
};

class VM : public enable_shared_from_this<VM>{
public:
	shared_ptr<Executable> ExecutableData;
	shared_ptr<Flame> CurrentFlame;
    VM( shared_ptr<Executable> exec):ExecutableData(exec){}
    void Init();
    VMValue Run(bool currflame_only); //trueで、呼び出し時点のフレームがポップされたら関数を抜ける
};


