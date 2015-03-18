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
    vector< pair<string,int> > *toplevel_vars=new vector< pair<string,int> >();
    for(unsigned int i=0;i<CodeInfo->FunctionList.size();i++){
        (*toplevel_vars).push_back(pair<string,int>(CodeInfo->FunctionList[i]->GetName(),reinterpret_cast<int>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->FunctionList[i]->poolindex))));
    }
    Flame *tl_flame=new Flame(toplevel_vars,&(reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->main_poolindex))->bytecodes),NULL,NULL);
    FunctionAST *mainfunc=reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->main_poolindex));
    for(unsigned int i=0;i<mainfunc->ChildPoolIndex->size();i++){
        //コンスタントプール内のクロージャに生成元のフレームを覚えさせる
        reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference((*(mainfunc->ChildPoolIndex))[i]))->ParentFlame=tl_flame;
    }
    Environment.push_back(tl_flame);

    int iopr1,iopr2;
    float fopr1,fopr2;
    bool bopr1,bopr2;
    void *ropr1;
    int localindex;

    int currentflame,counter;
	unsigned int i;

    while(true){
        if(Environment.size()==0){
            break;
        }
        switch((*(Environment.back()->CodePtr))[Environment.back()->PC++]){
        case ipush:
            Environment.back()->OperandStack.push((*(Environment.back()->CodePtr))[Environment.back()->PC++]);
            break;
        case fpush:
            Environment.back()->OperandStack.push((*(Environment.back()->CodePtr))[Environment.back()->PC++]);
            break;
        case btruepush:
            Environment.back()->OperandStack.push(static_cast<int>(true));
            break;
        case bfalsepush:
            Environment.back()->OperandStack.push(static_cast<int>(false));
            break;
        case iadd:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2+iopr1);
            break;
        case fadd:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(static_cast<int>(fopr2+fopr1));
            break;
        case bor:
            bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(static_cast<bool>(bopr2||bopr1));
            break;
        case isub:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2-iopr1);
            break;
        case fsub:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(static_cast<int>(fopr2-fopr1));
            break;
        case imul:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2*iopr1);
            break;
        case fmul:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(static_cast<int>(fopr2*fopr1));
            break;
        case band:
            bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(static_cast<bool>(bopr2&&bopr1));
            break;
        case idiv:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2/iopr1);
            break;
        case fdiv:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(static_cast<int>(fopr2/fopr1));
            break;
        case imod:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2%iopr1);
            break;
        case ineg:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr1*(-1));
            break;
        case fneg:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr1*(-1));
            break;
        case bnot:
            bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(!bopr1);
            break;
        case ilshift:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2<<iopr1);
            break;
        case irshift:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2>>iopr1);
            break;
        case if_icmpeq:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2==iopr1);
            break;
        case if_icmpne:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2!=iopr1);
            break;
        case if_icmpge:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2>=iopr1);
            break;
        case if_icmpgt:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2>iopr1);
            break;
        case if_icmple:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2<=iopr1);
            break;
        case if_icmplt:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2<iopr1);
            break;
        case if_bcmpeq:
            bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(bopr2==bopr1);
            break;
        case if_bcmpne:
            bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            bopr2=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(bopr2!=bopr1);
            break;
        case if_fcmpeq:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr2==fopr1);
            break;
        case if_fcmpne:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr2!=fopr1);
            break;
        case if_fcmpge:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr2>=fopr1);
            break;
        case if_fcmpgt:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr2>fopr1);
            break;
        case if_fcmple:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr2<=fopr1);
            break;
        case if_fcmplt:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            fopr2=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(fopr2<fopr1);
            break;
        case ldc:
            Environment.back()->OperandStack.push(reinterpret_cast<int>(CodeInfo->PublicConstantPool.GetReference((*(Environment.back()->CodePtr))[Environment.back()->PC++])));
            break;
        case invoke:
            {
                FunctionAST *callee=reinterpret_cast<FunctionAST *>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
                //フレームを作成
                vector< pair<string,int> > *vars=new vector< pair<string,int> >();
                //引数の準備
                for(int i=callee->Args.size()-1;i>=0;i--){
                    (*vars).push_back(pair<string,int>(callee->Args[i].first,reinterpret_cast<int>(Environment.back()->OperandStack.top())));
                    Environment.back()->OperandStack.pop();
                }
                //ローカル変数の準備
                for(int i=callee->LocalVariables.size()-1;i>=0;i--){
                    (*vars).push_back(pair<string,int>(callee->LocalVariables[i].first,0)); //ローカル変数はすべて0に初期化される
                }
                Flame *inv_flame=new Flame(vars,&(callee->bytecodes),callee->ParentFlame,Environment.back());
                for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
                    //コンスタントプール内のクロージャに生成元のフレームを覚えさせる
                    reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference((*(callee->ChildPoolIndex))[i]))->ParentFlame=inv_flame;
                }
                Environment.push_back(inv_flame);
            }
            break;
        case iloadlocal:
            localindex=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			counter=-1;
            //cout<<"localindex:"<<localindex<<endl;
			for(currentflame=(static_cast<int>(Environment.size())-1);currentflame>=0;currentflame--){
				for(i=0;i<Environment[currentflame]->Variables->size();i++){
					counter++;
					if(localindex==counter){
                        //cout<<"push:"<<((*(Environment[currentflame]->Variables))[i].second)<<endl;
						Environment.back()->OperandStack.push((*(Environment[currentflame]->Variables))[i].second);
						//cout<<(*(Environment[currentflame]->Variables))[i].second<<endl;
						goto fin1;
					}
				}
			}
			fin1:
            break;
        case floadlocal:
            localindex=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			counter=-1;

			for(currentflame=(static_cast<int>(Environment.size())-1);currentflame>=0;currentflame--){
				for(i=0;i<Environment[currentflame]->Variables->size();i++){
					counter++;
					if(localindex==counter){
						Environment.back()->OperandStack.push((*(Environment[currentflame]->Variables))[i].second);
						goto fin2;
					}
				}
			}
			fin2:
            break;
        case bloadlocal:
            localindex=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			counter=-1;

			for(currentflame=(static_cast<int>(Environment.size())-1);currentflame>=0;currentflame--){
				for(i=0;i<Environment[currentflame]->Variables->size();i++){
					counter++;
					if(localindex==counter){
						Environment.back()->OperandStack.push((*(Environment[currentflame]->Variables))[i].second);
						goto fin3;
					}
				}
			}
			fin3:
            break;
        case rloadlocal:
            localindex=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			counter=-1;

			for(currentflame=(static_cast<int>(Environment.size())-1);currentflame>=0;currentflame--){
				for(i=0;i<Environment[currentflame]->Variables->size();i++){
					counter++;
					if(localindex==counter){
						Environment.back()->OperandStack.push((*(Environment[currentflame]->Variables))[i].second);
						goto fin4;
					}
				}
			}
			fin4:
            break;
        case ret:
            Environment.pop_back();
            break;
        case iret:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.pop_back();
            if(!Environment.empty()){
                Environment.back()->OperandStack.push(iopr1);
            }
            break;
        case fret:
            fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.pop_back();
            if(!Environment.empty()){
                Environment.back()->OperandStack.push(fopr1);
            }
            break;
        case bret:
            bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.pop_back();
            if(!Environment.empty()){
                Environment.back()->OperandStack.push(bopr1);
            }
            break;
        case rret:
            ropr1=reinterpret_cast<void *>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
            Environment.pop_back();
            if(!Environment.empty()){
                Environment.back()->OperandStack.push(reinterpret_cast<int>(ropr1));
            }
            break;
        case invokebuiltin:
            {
                //ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
                string builtin_name=*(reinterpret_cast<string *>(Environment.back()->OperandStack.top()));
                Environment.back()->OperandStack.pop();
                if(builtin_name=="print"){
                    string str=*(reinterpret_cast<string *>(Environment.back()->OperandStack.top())); Environment.back()->OperandStack.pop();
                    cout<<str<<flush;
                }else if(builtin_name=="printint"){
                    iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
                    cout<<iopr1<<flush;
                }else if(builtin_name=="printfloat"){
                    fopr1=static_cast<float>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
                    cout<<fopr1<<flush;
                }else if(builtin_name=="printbool"){
                    bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
                    cout<<(bopr1?"true":"false")<<flush;
                }else if(builtin_name=="abs"){
                    iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
                    //返り値を直にプッシュ
                    Environment.back()->OperandStack.push(abs(iopr1));
                }else{
                    error("不正なビルトイン関数名です。");
                }
            }
            break;
        default:
            error("不正な命令です");
            break;
        }
    }

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
