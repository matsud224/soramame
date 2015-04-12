#pragma once
#include <stack>
#include <vector>
#include <map>
#include <string>

using namespace std;

class CodegenInfo;

enum{
    ipush,
    iadd,
    isub,
    imul,
    idiv,
    band,bor,
    imod,
    ineg,bnot,
    ilshift,irshift,
    invoke,
    iloadlocal, //localという名前だが、結局はフレームを遡っていくのでグローバル変数に行き着くかもしれない
    ret,iret,
    istorelocal,
    makeclosure,
    skip,iffalse_skip,
    icmpeq,icmpne,icmplt,icmple,icmpgt,icmpge,
    back,
    makelist
};



class Flame{
public:
    vector< pair<string,int> > *Variables; //型検査には通ってるので型情報は保管しない（変数名と値のペア）
    stack<int> OperandStack; //ここで計算を行う
    vector<int> *CodePtr; //バイトコードへのポインタ
    Flame *StaticLink;
    int PC;
    Flame(vector< pair<string,int> > *vars,vector<int> *codeptr,Flame *staticlink):Variables(vars),CodePtr(codeptr),StaticLink(staticlink){PC=0;}
};

class VM{
private:
     CodegenInfo *CodeInfo;
     vector<Flame *> Environment;
public:
    VM( CodegenInfo *cinfo):CodeInfo(cinfo){}
    int Run();
};



