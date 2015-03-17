#include "vm.h"
#include "utility.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

void VM::Run()
{
    //トップレベルのフレームを作成
    map<string,int> *toplevel_vars=new map<string,int>();
    for(int i=0;i<CodeInfo->FunctionList.size();i++){
        (*toplevel_vars)[CodeInfo->FunctionList[i]->GetName()]=reinterpret_cast<int>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->FunctionList[i]->poolindex));
    }
    Environment.push(new Flame(toplevel_vars,reinterpret_cast<vector<int> *>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->main_poolindex)),NULL,NULL));

    cout<<"実行します..."<<endl;

    int iopr1,iopr2;
    float fopr1,fopr2;
    bool bopr1,bopr2;
    void *ropr1,*ropr2;
    int localindex;

    while(true){
        switch((*(Environment.top()->CodePtr))[Environment.top()->PC++]){
        case ipush:
            Environment.top()->OperandStack.push((*(Environment.top()->CodePtr))[Environment.top()->PC++]);
            break;
        case fpush:
            Environment.top()->OperandStack.push((*(Environment.top()->CodePtr))[Environment.top()->PC++]);
            break;
        case btruepush:
            Environment.top()->OperandStack.push(static_cast<int>(true));
            break;
        case bfalsepush:
            Environment.top()->OperandStack.push(static_cast<int>(false));
            break;
        case iadd:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2+iopr1);
            break;
        case fadd:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(static_cast<int>(fopr2+fopr1));
            break;
        case bor:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(static_cast<bool>(bopr2||bopr1));
            break;
        case isub:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2-iopr1);
            break;
        case fsub:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(static_cast<int>(fopr2-fopr1));
            break;
        case imul:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2*iopr1);
            break;
        case fmul:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(static_cast<int>(fopr2*fopr1));
            break;
        case band:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(static_cast<bool>(bopr2&&bopr1));
            break;
        case idiv:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2/iopr1);
            break;
        case fdiv:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(static_cast<int>(fopr2/fopr1));
            break;
        case imod:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2%iopr1);
            break;
        case ineg:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr1*(-1));
            break;
        case fneg:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr1*(-1));
            break;
        case bnot:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(!bopr1);
            break;
        case ilshift:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2<<iopr1);
            break;
        case irshift:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2>>iopr1);
            break;
        case iprint:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            cout<<iopr1<<endl;
            break;
        case fprint:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            cout<<fopr1<<endl;
            break;
        case bprint:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            cout<<(bopr1?"true":"false")<<endl;
            break;
        case if_icmpeq:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2==iopr1);
            break;
        case if_icmpne:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2!=iopr1);
            break;
        case if_icmpge:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2>=iopr1);
            break;
        case if_icmpgt:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2>iopr1);
            break;
        case if_icmple:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2<=iopr1);
            break;
        case if_icmplt:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            iopr2=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(iopr2<iopr1);
            break;
        case if_bcmpeq:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(bopr2==bopr1);
            break;
        case if_bcmpne:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(bopr2!=bopr1);
            break;
        case if_fcmpeq:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr2==fopr1);
            break;
        case if_fcmpne:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr2!=fopr1);
            break;
        case if_fcmpge:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr2>=fopr1);
            break;
        case if_fcmpgt:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr2>fopr1);
            break;
        case if_fcmple:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr2<=fopr1);
            break;
        case if_fcmplt:
            fopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();
            Environment.top()->OperandStack.push(fopr2<fopr1);
            break;
        case ldc:
            Environment.top()->OperandStack.push(reinterpret_cast<int>(CodeInfo->PublicConstantPool.GetReference((*(Environment.top()->CodePtr))[Environment.top()->PC++])));
            break;
        case invoke:

            break;
        case iloadlocal:
            localindex=(*(Environment.top()->CodePtr))[Environment.top()->PC++];

            break;
        case floadlocal:
            localindex=(*(Environment.top()->CodePtr))[Environment.top()->PC++];

            break;
        case bloadlocal:
            localindex=(*(Environment.top()->CodePtr))[Environment.top()->PC++];

            break;
        case rloadlocal:
            localindex=(*(Environment.top()->CodePtr))[Environment.top()->PC++];

            break;
        case ret:

            break;
        case iret:
            iopr1=Environment.top()->OperandStack.top(); Environment.top()->OperandStack.pop();

            break;
        case fret:
            iopr1=static_cast<float>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();

            break;
        case bret:
            bopr1=static_cast<bool>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();

            break;
        case rret:
            ropr1=static_cast<void *>(Environment.top()->OperandStack.top()); Environment.top()->OperandStack.pop();

            break;
        default:
            error("不正な命令です");
            break;
        }
    }

    cout<<"終了しました。"<<endl;
}

/*
void VM::OutputCode()
{


    cout<<"バイトコードを出力します..."<<endl;


    //とりあえずmain関数がCode[0]に入ってるとして強引に実行
    while(PC<Code[0].size()){
        switch(Code[0][PC++]){
        case ipush:
            cout<<"ipush　"<<Code[0][PC++]<<endl;
            break;
        case fpush:
            cout<<"fpush　"<<static_cast<float>(Code[0][PC++])<<endl;
            break;
        case btruepush:
            cout<<"btruepush"<<endl;
            break;
        case bfalsepush:
            cout<<"bfalsepush"<<endl;
            break;
        case iadd:
            cout<<"iadd"<<endl;
            break;
        case fadd:
            cout<<"fadd"<<endl;
            break;
        case isub:
            cout<<"isub"<<endl;
            break;
        case fsub:
            cout<<"fsub"<<endl;
            break;
        case imul:
            cout<<"imul"<<endl;
            break;
        case fmul:
            cout<<"fmul"<<endl;
            break;
        case idiv:
            cout<<"idiv"<<endl;
            break;
        case fdiv:
            cout<<"fdiv"<<endl;
            break;
        case imod:
            cout<<"imod"<<endl;
            break;
        case ineg:
            cout<<"ineg"<<endl;
            break;
        case fneg:
            cout<<"fneg"<<endl;
            break;
        case band:
            cout<<"band"<<endl;
            break;
        case bor:
            cout<<"bor"<<endl;
            break;
        case bnot:
            cout<<"bnot"<<endl;
            break;
        case ilshift:
            cout<<"ilshift"<<endl;
            break;
        case irshift:
            cout<<"irshift"<<endl;
            break;
        case iprint:
            cout<<"iprint"<<endl;
            break;
        case fprint:
            cout<<"fprint"<<endl;
            break;
        case bprint:
            cout<<"bprint"<<endl;
            break;
        case if_icmpeq:
            cout<<"if_icmpeq"<<endl;
            break;
        case if_icmpne:
            cout<<"if_icmpne"<<endl;
            break;
        case if_icmpge:
            cout<<"if_icmpge"<<endl;
            break;
        case if_icmpgt:
            cout<<"if_icmpgt"<<endl;
            break;
        case if_icmple:
            cout<<"if_icmple"<<endl;
            break;
        case if_icmplt:
            cout<<"if_icmplt"<<endl;
            break;
        case if_bcmpeq:
            cout<<"if_bcmpeq"<<endl;
            break;
        case if_bcmpne:
            cout<<"if_bcmpne"<<endl;
            break;
        case if_fcmpeq:
            cout<<"if_fcmpeq"<<endl;
            break;
        case if_fcmpne:
            cout<<"if_fcmpne"<<endl;
            break;
        case if_fcmpge:
            cout<<"if_fcmpge"<<endl;
            break;
        case if_fcmpgt:
            cout<<"if_fcmpgt"<<endl;
            break;
        case if_fcmple:
            cout<<"if_fcmple"<<endl;
            break;
        case if_fcmplt:
            cout<<"if_fcmplt"<<endl;
            break;
        case ldc:
            cout<<"ldc #"<<Code[0][PC++]<<endl;
        default:
            error("不正な命令です");
            break;
        }
    }
}
*/
