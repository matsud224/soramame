#pragma once
#include <stack>
#include <vector>
#include <map>
#include "ast.h"

using namespace std;

enum{
    ipush,fpush,
    btruepush,bfalsepush,
    iadd,fadd,
    isub,fsub,
    imul,fmul,
    idiv,fdiv,
    band,bor,
    imod,
    ineg,fneg,bnot,
    ilshift,irshift,
    iprint,fprint,bprint,
    if_icmpeq,if_icmpne,if_icmplt,if_icmpgt,if_icmple,if_icmpge,
    if_fcmpeq,if_fcmpne,if_fcmplt,if_fcmpgt,if_fcmple,if_fcmpge,
    if_bcmpeq,if_bcmpne,
    ldc,
    invoke,
    iloadlocal,floadlocal,bloadlocal,rloadlocal, //localという名前だが、結局はフレームを遡っていくのでグローバル変数に行き着くかもしれない
    ret,iret,fret,bret,rret,
};

class Flame{
public:
    map<string,int> *VariableMap; //型検査には通ってるので型情報は保管しない（変数名と値のペア）
    stack<int> OperandStack; //ここで計算を行う
    vector<int> *CodePtr; //コードベクトルへのポインタ
    Flame *StaticLink;
    Flame *DynamicLink;
    int PC;
    Flame(map<string,int> *varmap,vector<int> *codeptr,Flame *staticlink,Flame *dynamiclink):VariableMap(varmap),CodePtr(codeptr),StaticLink(staticlink),DynamicLink(dynamiclink){PC=0;}
};

class VM{
private:
     CodegenInfo *CodeInfo;
     stack<Flame *> Environment;
public:
    VM( CodegenInfo *cinfo):CodeInfo(cinfo){}
    void Run();
    //void OutputCode();
};



