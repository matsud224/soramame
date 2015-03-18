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
    iloadlocal,floadlocal,bloadlocal,rloadlocal, //local�Ƃ������O�����A���ǂ̓t���[����k���Ă����̂ŃO���[�o���ϐ��ɍs��������������Ȃ�
    ret,iret,fret,bret,rret,
    invokebuiltin //�r���g�C���֐��Ăяo���i�X�^�b�N�g�b�v�̕����񂪑g�ݍ��݊֐����j
};



class Flame{
public:
    vector< pair<string,int> > *Variables; //�^�����ɂ͒ʂ��Ă�̂Ō^���͕ۊǂ��Ȃ��i�ϐ����ƒl�̃y�A�j
    stack<int> OperandStack; //�����Ōv�Z���s��
    vector<int> *CodePtr; //�R�[�h�x�N�g���ւ̃|�C���^
    Flame *StaticLink;
    //Flame *DynamicLink;
    int PC;
    Flame(vector< pair<string,int> > *vars,vector<int> *codeptr,Flame *staticlink):Variables(vars),CodePtr(codeptr),StaticLink(staticlink){PC=0;}
};

class VM{
private:
     CodegenInfo *CodeInfo;
     vector<Flame *> Environment;
public:
    VM( CodegenInfo *cinfo):CodeInfo(cinfo){}
    void Run();
    //void OutputCode();
};



