#include "vm.h"
#include "utility.h"
#include <cstdlib>
#include <string>
#include <iostream>
#include "ast.h"
#include "basic_object.h"
#include <cmath>
#include <map>

#define STACK_GETSTR *(reinterpret_cast<string *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT)))
#define STACK_POP Environment.back()->OperandStack.pop()
#define STACK_PUSH(x) Environment.back()->OperandStack.push((x))
#define STACK_GETINT Environment.back()->OperandStack.top()
#define OPERAND_GET (*(Environment.back()->CodePtr))[Environment.back()->PC++]

using namespace std;


int VM::Run()
{
    //トップレベルのフレームを作成
    vector< pair<string,int> > *toplevel_vars=new vector< pair<string,int> >();

    Flame *tl_flame=new Flame(toplevel_vars,&(CodeInfo->Bootstrap),NULL);

    for(unsigned int i=0;i<CodeInfo->TopLevelFunction.size();i++){
		ClosureObject *cobj=new ClosureObject(CodeInfo->TopLevelFunction[i]->PoolIndex, tl_flame);
        (*toplevel_vars).push_back(pair<string,int>(CodeInfo->TopLevelFunction[i]->Name,CodeInfo->PublicConstantPool.SetReference(cobj)));
    }
	for(unsigned int i=0;i<CodeInfo->TopLevelVariableDef.size();i++){
        (*toplevel_vars).push_back(pair<string,int>(CodeInfo->TopLevelVariableDef[i]->Variable->first,0)); //値はとりあえず0にしておく
    }

    for(unsigned int i=0;i<CodeInfo->ChildPoolIndex.size();i++){
		//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
		reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference(CodeInfo->ChildPoolIndex[i]))->ParentFlame=tl_flame;
	}
    Environment.push_back(tl_flame);

    int iopr1,iopr2;
    bool bopr1,bopr2;
    int flameback,localindex;

    Flame *currentflame;
    int counter;
	int i;

    while(true){
        if(Environment.size()==0){
			return 0;
        }
        switch(OPERAND_GET){
        case ipush:
            STACK_PUSH(OPERAND_GET);
            break;
        case iadd:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2+iopr1);
            break;
        case bor:
            bopr1=(STACK_GETINT)==0?false:true;  STACK_POP;
            bopr2=(STACK_GETINT)==0?false:true;  STACK_POP;
            STACK_PUSH((bopr2||bopr1)?1:0);
            break;
        case isub:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2-iopr1);
            break;
        case imul:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2*iopr1);
            break;
        case band:
            bopr1=(STACK_GETINT)==0?false:true;  STACK_POP;
            bopr2=(STACK_GETINT)==0?false:true;  STACK_POP;
            STACK_PUSH((bopr2&&bopr1)?1:0);
            break;
        case idiv:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2/iopr1);
            break;
        case imod:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2%iopr1);
            break;
        case ineg:
            iopr1=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr1*(-1));
            break;
        case bnot:
            bopr1=(STACK_GETINT)==0?false:true;  STACK_POP;
            STACK_PUSH((!bopr1)?1:0);
            break;
        case ilshift:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2<<iopr1);
            break;
        case irshift:
            iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH(iopr2>>iopr1);
            break;
		case icmpeq:
			iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH((iopr2==iopr1)?1:0);
			break;
		case icmpne:
			iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH((iopr2!=iopr1)?1:0);
			break;
		case icmplt:
			iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH((iopr2<iopr1)?1:0);
			break;
		case icmple:
			iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH((iopr2<=iopr1)?1:0);
			break;
		case icmpgt:
			iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH((iopr2>iopr1)?1:0);
			break;
		case icmpge:
			iopr1=STACK_GETINT; STACK_POP;
            iopr2=STACK_GETINT; STACK_POP;
            STACK_PUSH((iopr2>=iopr1)?1:0);
			break;
        case invoke:
            {
            	ClosureObject *cobj=reinterpret_cast<ClosureObject *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT));
                FunctionAST *callee=reinterpret_cast<FunctionAST *>(CodeInfo->PublicConstantPool.GetReference(cobj->PoolIndex));
                STACK_POP;

                if(callee->isBuiltin){
					//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
					string builtin_name=callee->Name;
					string typestr=callee->TypeInfo->GetName();
					if(builtin_name=="print"){
						if(typestr=="fun(string)=>void"){
							string str=STACK_GETSTR; STACK_POP;
							cout<<str<<flush;
						}else if(typestr=="fun(int)=>void"){
							iopr1=STACK_GETINT; STACK_POP;
							cout<<iopr1<<flush;
						}else if(typestr=="fun(bool)=>void"){
							bopr1=static_cast<bool>(STACK_GETINT); STACK_POP;
							cout<<(bopr1?"true":"false")<<flush;
						}
					}else if(builtin_name=="abs"){
						iopr1=STACK_GETINT; STACK_POP;
						//返り値を直にプッシュ
						STACK_PUSH(abs(iopr1));
					}else if(builtin_name=="rand"){
						//返り値を直にプッシュ
						STACK_PUSH(rand());
					}else if(builtin_name=="pow"){
						iopr1=STACK_GETINT; STACK_POP;
						iopr2=STACK_GETINT; STACK_POP;
						//返り値を直にプッシュ
						STACK_PUSH(pow(iopr1,iopr2));
					}else if(builtin_name=="strlen"){
						string str=STACK_GETSTR; STACK_POP;
						STACK_PUSH(str.length());
					}else if(builtin_name=="get_intlist"){
						list<int> *lst=(reinterpret_cast<list<int> *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
						iopr1=STACK_GETINT; STACK_POP;
						list<int>::iterator iter=lst->begin();
						for(int i=0;i<iopr1;i++){
							iter++;
						}
						STACK_PUSH(*iter);
					}else if(builtin_name=="get_boollist"){
						list<int> *lst=(reinterpret_cast<list<int> *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
						iopr1=STACK_GETINT; STACK_POP;
						list<int>::iterator iter=lst->begin();
						for(int i=0;i<iopr1;i++){
							iter++;
						}
						STACK_PUSH(*iter);
					}else if(builtin_name=="get_funlist"){
						list<int> *lst=(reinterpret_cast<list<int> *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
						//cout<<endl<<lst->size()<<endl;
						iopr1=STACK_GETINT; STACK_POP;
						list<int>::iterator iter=lst->begin();
						for(int i=0;i<iopr1;i++){
							iter++;
						}
						STACK_PUSH(*iter);
					}
                }else{
					//フレームを作成
					vector< pair<string,int> > *vars=new vector< pair<string,int> >();
					//引数の準備
					for(int i=callee->Args->size()-1;i>=0;i--){
						(*vars).push_back(pair<string,int>(callee->Args->at(i).first,reinterpret_cast<int>(STACK_GETINT)));
						STACK_POP;
					}
					//ローカル変数の準備
					for(int i=callee->LocalVariables->size()-1;i>=0;i--){
						(*vars).push_back(pair<string,int>(callee->LocalVariables->at(i).first,0)); //ローカル変数はすべて0に初期化される
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
        	flameback=OPERAND_GET;
            localindex=OPERAND_GET;
            counter=flameback;
			currentflame=Environment.back();
			for(i=0;i<flameback;i++){
				currentflame=currentflame->StaticLink;
			}
			STACK_PUSH((*(currentflame->Variables))[localindex].second);
            break;
        case ret:
            Environment.pop_back();
            break;
        case iret:
            iopr1=STACK_GETINT; STACK_POP;
            if(Environment.size()==2){
				//ブートストラップコードへのreturn...
				return iopr1;
            }
            Environment.pop_back();
            if(!Environment.empty()){
                STACK_PUSH(iopr1);
            }
            break;
        case istorelocal:
            flameback=OPERAND_GET;
            localindex=OPERAND_GET; //localindex
            iopr2=STACK_GETINT; STACK_POP; //値
            //cout<<"localindex:"<<localindex<<endl;
            currentflame=Environment.back();
			for(i=0;i<flameback;i++){
				currentflame=currentflame->StaticLink;
			}
			(*(currentflame->Variables))[localindex].second=iopr2;
            break;
		case makeclosure:
			{
			//オペランドにpoolindexをとり、クロージャオブジェクトを生成し、そのpoolindexをプッシュ
			iopr1=OPERAND_GET; //poolindex
			ClosureObject *cobj=new ClosureObject(iopr1,reinterpret_cast<FunctionAST*>(CodeInfo->PublicConstantPool.GetReference(iopr1))->ParentFlame);
			STACK_PUSH(CodeInfo->PublicConstantPool.SetReference(cobj));
			//cout<<"makeclosure: "<<iopr1<<","<<STACK_GETINT<<endl;
			}
			break;
		case skip:
			iopr1=OPERAND_GET;
			Environment.back()->PC+=iopr1;
			break;
		case iffalse_skip:
			iopr1=OPERAND_GET;
			bopr1=(STACK_GETINT)==0?false:true;  STACK_POP;
			if(!bopr1){
				Environment.back()->PC+=iopr1;
			}
			break;
		case back:
			iopr1=OPERAND_GET;
			Environment.back()->PC-=iopr1;
			break;
		case makelist:
			{
				list<int> *newlist=new list<int>();
				iopr1=OPERAND_GET; //リストサイズ
				for(int i=0;i<iopr1;i++){
					iopr2=STACK_GETINT; STACK_POP; //リストの要素
					newlist->push_back(iopr2);
				}
				STACK_PUSH(CodeInfo->PublicConstantPool.SetReference(newlist));
				break;
			}
		case makedata:
			{
				map<string,int> *newmap=new map<string,int>();
				string originalname=*(reinterpret_cast<string *>(CodeInfo->PublicConstantPool.GetReference(OPERAND_GET)));
				iopr1=OPERAND_GET; //メンバ数
				for(int i=0;i<iopr1;i++){
					string membername=STACK_GETSTR; STACK_POP;
					iopr2=STACK_GETINT; STACK_POP; //リストの要素
					(*newmap)[membername]=iopr2;
				}
				STACK_PUSH(CodeInfo->PublicConstantPool.SetReference(new DataObject(originalname,newmap)));
				break;
			}
		case iloadbyindex:
			{
				iopr1=STACK_GETINT; STACK_POP;
				list<int> *lst=(reinterpret_cast<list<int> *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
				list<int>::iterator iter=lst->begin();
				for(int i=0;i<iopr1;i++){
					iter++;
				}
				STACK_PUSH(*iter);
			}
			break;
		case iloadbyname:
			{
				string name=*(reinterpret_cast<string *>(CodeInfo->PublicConstantPool.GetReference(OPERAND_GET)));
				DataObject *obj=(reinterpret_cast<DataObject *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
				map<string,int>::iterator iter;
				for(iter=obj->MemberMap->begin();iter!=obj->MemberMap->end();iter++){
					if(iter->first==name){
						STACK_PUSH(iter->second);
						break;
					}
				}
			}
			break;
		case istorebyindex:
			{
				iopr1=STACK_GETINT; STACK_POP;
				list<int> *lst=(reinterpret_cast<list<int> *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
				list<int>::iterator iter=lst->begin();
				for(int i=0;i<iopr1;i++){
					iter++;
				}
				(*iter)=STACK_GETINT; STACK_POP;
			}
			break;
		case istorefield:
			{
				string name=*(reinterpret_cast<string *>(CodeInfo->PublicConstantPool.GetReference(OPERAND_GET)));
				DataObject *obj=(reinterpret_cast<DataObject *>(CodeInfo->PublicConstantPool.GetReference(STACK_GETINT))); STACK_POP;
				map<string,int>::iterator iter;
				for(iter=obj->MemberMap->begin();iter!=obj->MemberMap->end();iter++){
					if(iter->first==name){
						iter->second=STACK_GETINT;
						break;
					}
				}
			}
			break;
        default:
            error("不正な命令です");
            break;
        }
    }

}

