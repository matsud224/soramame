#include "vm.h"
#include "utility.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include "ast.h"
#include "basic_object.h"
#include <cmath>

using namespace std;


void VM::Run()
{
    //トップレベルのフレームを作成
    vector< pair<string,int> > *toplevel_vars=new vector< pair<string,int> >();

    Flame *tl_flame=new Flame(toplevel_vars,&(CodeInfo->Bootstrap),NULL);

    for(unsigned int i=0;i<CodeInfo->TopLevelVariableDef.size();i++){
        (*toplevel_vars).push_back(pair<string,int>(CodeInfo->TopLevelVariableDef[i]->Variable->first,0)); //値はとりあえず0にしておく
    }
    for(unsigned int i=0;i<CodeInfo->TopLevelFunction.size();i++){
		ClosureObject *cobj=new ClosureObject(CodeInfo->TopLevelFunction[i]->PoolIndex, tl_flame);
        (*toplevel_vars).push_back(pair<string,int>(CodeInfo->TopLevelFunction[i]->Name,CodeInfo->PublicConstantPool.SetReference(cobj)));
    }

    for(unsigned int i=0;i<CodeInfo->ChildPoolIndex.size();i++){
		//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
		reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->ChildPoolIndex[i]))->ParentFlame=tl_flame;
	}
    Environment.push_back(tl_flame);

    int iopr1,iopr2;
    bool bopr1,bopr2;
    int localindex;

    Flame *currentflame;
    int counter;
	unsigned int i;

    while(true){
        if(Environment.size()==0){
            break;
        }
        switch((*(Environment.back()->CodePtr))[Environment.back()->PC++]){
        case ipush:
            Environment.back()->OperandStack.push((*(Environment.back()->CodePtr))[Environment.back()->PC++]);
            break;
        case iadd:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2+iopr1);
            break;
        case bor:
            bopr1=(Environment.back()->OperandStack.top())==0?false:true;  Environment.back()->OperandStack.pop();
            bopr2=(Environment.back()->OperandStack.top())==0?false:true;  Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((bopr2||bopr1)?1:0);
            break;
        case isub:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2-iopr1);
            break;
        case imul:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2*iopr1);
            break;
        case band:
            bopr1=(Environment.back()->OperandStack.top())==0?false:true;  Environment.back()->OperandStack.pop();
            bopr2=(Environment.back()->OperandStack.top())==0?false:true;  Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((bopr2&&bopr1)?1:0);
            break;
        case idiv:
            iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push(iopr2/iopr1);
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
        case bnot:
            bopr1=(Environment.back()->OperandStack.top())==0?false:true;  Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((!bopr1)?1:0);
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
		case icmpeq:
			iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((iopr2==iopr1)?1:0);
			break;
		case icmpne:
			iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((iopr2!=iopr1)?1:0);
			break;
		case icmplt:
			iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((iopr2<iopr1)?1:0);
			break;
		case icmple:
			iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((iopr2<=iopr1)?1:0);
			break;
		case icmpgt:
			iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((iopr2>iopr1)?1:0);
			break;
		case icmpge:
			iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
            Environment.back()->OperandStack.push((iopr2>=iopr1)?1:0);
			break;
        case invoke:
            {
            	ClosureObject *cobj=reinterpret_cast<ClosureObject *>(CodeInfo->PublicConstantPool.GetReference(Environment.back()->OperandStack.top()));
                FunctionAST *callee=reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference(cobj->PoolIndex));
                Environment.back()->OperandStack.pop();

                if(callee->isBuiltin){
					//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
					string builtin_name=callee->Name;
					if(builtin_name=="print"){
						string str=*(reinterpret_cast<string *>(CodeInfo->PublicConstantPool.GetReference(Environment.back()->OperandStack.top()))); Environment.back()->OperandStack.pop();
						cout<<str<<flush;
					}else if(builtin_name=="printint"){
						iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
						cout<<iopr1<<flush;
					}else if(builtin_name=="printbool"){
						bopr1=static_cast<bool>(Environment.back()->OperandStack.top()); Environment.back()->OperandStack.pop();
						cout<<(bopr1?"true":"false")<<flush;
					}else if(builtin_name=="abs"){
						iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
						//返り値を直にプッシュ
						Environment.back()->OperandStack.push(abs(iopr1));
					}else if(builtin_name=="rand"){
						//返り値を直にプッシュ
						Environment.back()->OperandStack.push(rand());
					}else if(builtin_name=="pow"){
						iopr1=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
						iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop();
						//返り値を直にプッシュ
						Environment.back()->OperandStack.push(pow(iopr1,iopr2));
					}else if(builtin_name=="strlen"){
						string str=*(reinterpret_cast<string *>(CodeInfo->PublicConstantPool.GetReference(Environment.back()->OperandStack.top()))); Environment.back()->OperandStack.pop();
						Environment.back()->OperandStack.push(str.length());
					}
                }else{
					//フレームを作成
					vector< pair<string,int> > *vars=new vector< pair<string,int> >();
					//引数の準備
					for(int i=callee->Args->size()-1;i>=0;i--){
						(*vars).push_back(pair<string,int>(callee->Args->at(i).first,reinterpret_cast<int>(Environment.back()->OperandStack.top())));
						Environment.back()->OperandStack.pop();
					}
					//ローカル変数の準備
					for(int i=callee->Body->LocalVariables->size()-1;i>=0;i--){
						(*vars).push_back(pair<string,int>(callee->Body->LocalVariables->at(i).first,0)); //ローカル変数はすべて0に初期化される
					}
					Flame *inv_flame=new Flame(vars,&(callee->bytecodes),cobj->ParentFlame);
					for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
						//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
						reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference((*(callee->ChildPoolIndex))[i]))->ParentFlame=inv_flame;
					}
					Environment.push_back(inv_flame);
                }
            }
            break;
        case iloadlocal:
            localindex=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			counter=-1;
            //cout<<"localindex:"<<localindex<<endl;
			currentflame=Environment.back();
			do{
				for(i=0;i<currentflame->Variables->size();i++){
					counter++;
					if(localindex==counter){
                        //cout<<"push:"<<((*(Environment[currentflame]->Variables))[i].second)<<endl;
						Environment.back()->OperandStack.push((*(currentflame->Variables))[i].second);
						//cout<<(*(Environment[currentflame]->Variables))[i].second<<endl;
						goto fin1;
					}
				}
			}while(currentflame=currentflame->StaticLink);
			fin1:
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
        case istorelocal:
            //スタックトップにlocalindexが、その下に値がおいてある
            iopr1=(*(Environment.back()->CodePtr))[Environment.back()->PC++]; //localindex
            iopr2=Environment.back()->OperandStack.top(); Environment.back()->OperandStack.pop(); //値
            counter=iopr1;
            //cout<<"localindex:"<<localindex<<endl;
            currentflame=Environment.back();
			do{
				for(i=0;i<currentflame->Variables->size();i++){
					counter--;
					if(counter==-1){
						(*(currentflame->Variables))[i].second=iopr2;
						goto fin6;
					}
				}
			}while(currentflame=currentflame->StaticLink);
			fin6:
            break;
		case makeclosure:
			{
			//オペランドにpoolindexをとり、クロージャオブジェクトを生成し、そのpoolindexをプッシュ
			iopr1=(*(Environment.back()->CodePtr))[Environment.back()->PC++]; //poolindex
			ClosureObject *cobj=new ClosureObject(iopr1,reinterpret_cast<FunctionAST*>(CodeInfo->PublicConstantPool.GetReference(iopr1))->ParentFlame);
			Environment.back()->OperandStack.push(CodeInfo->PublicConstantPool.SetReference(cobj));
			//cout<<"makeclosure: "<<iopr1<<","<<Environment.back()->OperandStack.top()<<endl;
			}
			break;
		case skip:
			iopr1=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			Environment.back()->PC+=iopr1;
			break;
		case iffalse_skip:
			iopr1=(*(Environment.back()->CodePtr))[Environment.back()->PC++];
			bopr1=(Environment.back()->OperandStack.top())==0?false:true;  Environment.back()->OperandStack.pop();
			if(!bopr1){
				Environment.back()->PC+=iopr1;
			}
			break;
        default:
            error("不正な命令です");
            break;
        }
    }

}

