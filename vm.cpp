#include "vm.h"
#include "utility.h"
#include <string>
#include "ast.h"
#include "basic_object.h"
#include <map>
#include <memory>


#define STACK_POP CurrentFlame->OperandStack.pop()
#define STACK_PUSH(x) CurrentFlame->OperandStack.push((x))
#define STACK_GET CurrentFlame->OperandStack.top()
#define OPERAND_GET (*(CurrentFlame->CodePtr))[CurrentFlame->PC++]
#define GET_CONSTANT(x) ExecutableData->PublicConstantPool.GetValue((x))

using namespace std;


void VM::Init()
{
    //トップレベルのフレームを作成
    shared_ptr< vector< pair<string,VMValue> > > toplevel_vars=make_shared<vector< pair<string,VMValue> > >();
	shared_ptr<Flame> tl_flame=make_shared<Flame>(toplevel_vars,ExecutableData->Bootstrap,nullptr,nullptr);

    //ローカル変数の準備
	for(int i=ExecutableData->LocalVariables->size()-1;i>=0;i--){
		VMValue v;
		(*toplevel_vars).push_back(pair<string,VMValue>(ExecutableData->LocalVariables->at(i).first,v)); //ローカル変数はのちに正しく初期化される
	}

    for(unsigned int i=0;i<ExecutableData->ChildPoolIndex.size();i++){
		static_pointer_cast<FunctionObject>(ExecutableData->PublicConstantPool.GetValue(ExecutableData->ChildPoolIndex[i]).ref_value)->ParentFlame=tl_flame;
	}
    CurrentFlame=tl_flame;
}

VMValue VM::Run(bool currflame_only){
    VMValue v;
    int iopr1,iopr2;
    bool bopr1,bopr2;
    double dopr1,dopr2;
    int flameback,localindex;

	shared_ptr<Flame> current_flame_ptr=CurrentFlame;
    shared_ptr<Flame> currentflame;
	int i;int bytecode;

    while(true){
		v=VMValue();
        if(CurrentFlame==nullptr){
			v.int_value=0;
			return v;
        }
        if(currflame_only && CurrentFlame==current_flame_ptr->DynamicLink){
			v.int_value=0;
			return v;
        }
        bytecode=OPERAND_GET;
        switch(bytecode){
        case ipush:
            v.int_value=OPERAND_GET;
            STACK_PUSH(v);
            break;
		case bpush:
			v.bool_value=OPERAND_GET==0?false:true;
			STACK_PUSH(v);
			break;
		case pushnull:
			v.ref_value=nullptr;
			STACK_PUSH(v);
			break;
		case ldc:
			v=GET_CONSTANT(OPERAND_GET);
			STACK_PUSH(v);
			break;
        case iadd:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1+iopr2;
            STACK_PUSH(v);
            break;
		case dadd:
            dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr1+dopr2;
            STACK_PUSH(v);
            break;
        case bor:
            bopr1=STACK_GET.bool_value;  STACK_POP;
            bopr2=STACK_GET.bool_value;  STACK_POP;
            v.bool_value=bopr1 || bopr2;
            STACK_PUSH(v);
            break;
        case isub:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1-iopr2;
            STACK_PUSH(v);
			break;
		case dsub:
            dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr1-dopr2;
            STACK_PUSH(v);
			break;
        case imul:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1*iopr2;
            STACK_PUSH(v);
            break;
		case dmul:
            dopr1=STACK_GET.double_value; STACK_POP;
			dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr1*dopr2;
            STACK_PUSH(v);
            break;
        case band:
            bopr1=STACK_GET.bool_value; STACK_POP;
            bopr2=STACK_GET.bool_value; STACK_POP;
            v.bool_value=bopr1 && bopr2;
            STACK_PUSH(v);
            break;
        case idiv:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1/iopr2;
            STACK_PUSH(v);
            break;
		case ddiv:
            dopr1=STACK_GET.double_value; STACK_POP;
			dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr1/dopr2;
            STACK_PUSH(v);
            break;
        case imod:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1%iopr2;
            STACK_PUSH(v);
            break;
        case ineg:
            iopr1=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1*(-1);
            STACK_PUSH(v);
            break;
		case dneg:
            dopr1=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr1*(-1.0);
            STACK_PUSH(v);
            break;
        case bnot:
            bopr1=STACK_GET.bool_value; STACK_POP;
            v.bool_value=!bopr1;
            STACK_PUSH(v);
            break;
        case ilshift:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1<<iopr2;
            STACK_PUSH(v);
            break;
        case irshift:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr1>>iopr2;
            STACK_PUSH(v);
            break;
		case icmpeq:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr1==iopr2;
            STACK_PUSH(v);
			break;
		case icmpne:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr1!=iopr2;
            STACK_PUSH(v);
			break;
		case icmplt:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr1<iopr2;
            STACK_PUSH(v);
			break;
		case icmple:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr1<=iopr2;
            STACK_PUSH(v);
			break;
		case icmpgt:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr1>iopr2;
            STACK_PUSH(v);
			break;
		case icmpge:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr1>=iopr2;
            STACK_PUSH(v);
			break;
		case dcmpeq:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr1==dopr2;
            STACK_PUSH(v);
			break;
		case dcmpne:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr1!=dopr2;
            STACK_PUSH(v);
			break;
		case dcmplt:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr1<dopr2;
            STACK_PUSH(v);
			break;
		case dcmple:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr1<=dopr2;
            STACK_PUSH(v);
			break;
		case dcmpgt:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr1>dopr2;
            STACK_PUSH(v);
			break;
		case dcmpge:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr1>=dopr2;
            STACK_PUSH(v);
			break;
		case bcmpeq:
			bopr1=STACK_GET.bool_value; STACK_POP;
            bopr2=STACK_GET.bool_value; STACK_POP;
            v.bool_value=bopr1==bopr2;
            STACK_PUSH(v);
			break;
		case bcmpne:
			bopr1=STACK_GET.bool_value; STACK_POP;
            bopr2=STACK_GET.bool_value; STACK_POP;
            v.bool_value=bopr1!=bopr2;
            STACK_PUSH(v);
			break;
        case invoke:
            {
            	shared_ptr<ClosureObject> cobj=static_pointer_cast<ClosureObject>(STACK_GET.ref_value);STACK_POP;
                shared_ptr<FunctionObject> callee=cobj->FunctionRef;
				bool is_tail=OPERAND_GET==0?false:true;
				//cout<<callee->Name<<endl;
				//cout<<callee->isBuiltin<<endl;

                if(callee->isBuiltin){
					//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
					string builtin_name=callee->Name;
					string typestr=callee->TypeInfo->GetName();

					ExecutableData->BuiltinFunctionList[pair<string,string>(builtin_name,typestr)](shared_from_this());

                }else{
					//フレームを作成
					shared_ptr< vector< pair<string,VMValue> > > vars=make_shared<vector< pair<string,VMValue> > >();
					//引数の準備
					for(int i=callee->Args->size()-1;i>=0;i--){
						(*vars).push_back(pair<string,VMValue>(callee->Args->at(i).first,STACK_GET));
						STACK_POP;
					}
					//ローカル変数の準備
					for(int i=callee->LocalVariables->size()-1;i>=0;i--){
						VMValue v;v.int_value=0;
						(*vars).push_back(pair<string,VMValue>(callee->LocalVariables->at(i).first,v)); //ローカル変数はすべて0に初期化される
					}
					shared_ptr<Flame> inv_flame=make_shared<Flame>(vars,callee->bytecodes,is_tail?CurrentFlame->DynamicLink:CurrentFlame,cobj->ParentFlame);
					for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
						//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
						static_pointer_cast<FunctionObject>(ExecutableData->PublicConstantPool.GetValue(callee->ChildPoolIndex->at(i)).ref_value)->ParentFlame=inv_flame;
					}
					CurrentFlame=inv_flame;
                }
            }
            break;
        case loadlocal:
        	flameback=OPERAND_GET;
            localindex=OPERAND_GET;
			currentflame=CurrentFlame;
			for(i=0;i<flameback;i++){
				currentflame=currentflame->StaticLink;
			}
			STACK_PUSH((*(currentflame->Variables))[localindex].second);
            break;
        case ret:
            CurrentFlame=CurrentFlame->DynamicLink;
            break;
        case ret_withvalue:
            v=STACK_GET; STACK_POP;
            if(CurrentFlame->DynamicLink->DynamicLink==nullptr){
				//ブートストラップコードへのreturn...
				return v;
            }
            CurrentFlame=CurrentFlame->DynamicLink;
            if(!(CurrentFlame==nullptr)){
                STACK_PUSH(v);
            }
            break;
		case makeclosure:
			{
			//オペランドにpoolindexをとり、クロージャオブジェクトを生成
			iopr1=OPERAND_GET; //poolindex
			shared_ptr<ClosureObject> cobj=make_shared<ClosureObject>(static_pointer_cast<FunctionObject>(ExecutableData->PublicConstantPool.GetValue(iopr1).ref_value),static_pointer_cast<FunctionObject>(ExecutableData->PublicConstantPool.GetValue(iopr1).ref_value)->ParentFlame);
			v.ref_value=cobj;
			STACK_PUSH(v);
			}
			break;
		case skip:
			iopr1=OPERAND_GET;
			CurrentFlame->PC+=iopr1;
			break;
		case iffalse_skip:
			iopr1=OPERAND_GET;
			bopr1=STACK_GET.bool_value;  STACK_POP;
			if(!bopr1){
				CurrentFlame->PC+=iopr1;
			}
			break;
		case back:
			iopr1=OPERAND_GET;
			CurrentFlame->PC-=iopr1;
			break;
		case makelist:
			{
				auto newlist=make_shared< list<VMValue> >();
				iopr1=OPERAND_GET; //リストサイズ
				for(int i=0;i<iopr1;i++){
					VMValue lv;
					lv=STACK_GET; STACK_POP; //リストの要素
					newlist->push_back(lv);
				}
				VMValue v2;v2.ref_value=newlist;
				STACK_PUSH(v2);
			}
			break;
		case makedata:
			{
				shared_ptr<map<string,VMValue> > newmap=make_shared< map<string,VMValue> >();
				string originalname=*(static_pointer_cast<string>(GET_CONSTANT(OPERAND_GET).ref_value));
				iopr1=OPERAND_GET; //メンバ数
				for(int i=0;i<iopr1;i++){
					string membername=*(static_pointer_cast<string>(GET_CONSTANT(STACK_GET.int_value).ref_value)); STACK_POP;
					VMValue vtemp=STACK_GET; STACK_POP; //リストの要素
					(*newmap)[membername]=vtemp;
				}
				shared_ptr<DataObject> newdata=make_shared<DataObject>(originalname,newmap);
				VMValue v2;v2.ref_value=newdata;
				STACK_PUSH(v2);
			}
			break;
		case loadbyindex:
			{
				iopr1=STACK_GET.int_value; STACK_POP;
				shared_ptr<list<VMValue> > lst=static_pointer_cast<list<VMValue> >(STACK_GET.ref_value); STACK_POP;
				list<VMValue>::iterator iter=lst->begin();
				for(int i=0;i<iopr1;i++){
					iter++;
				}
				STACK_PUSH(*iter);
			}
			break;
		case loadfield:
			{
				string name=*(static_pointer_cast<string>(ExecutableData->PublicConstantPool.GetValue(OPERAND_GET).ref_value));
				shared_ptr<DataObject> obj=(static_pointer_cast<DataObject>(STACK_GET.ref_value)); STACK_POP;
				map<string,VMValue>::iterator iter;
				for(iter=obj->MemberMap->begin();iter!=obj->MemberMap->end();iter++){
					if(iter->first==name){
						STACK_PUSH(iter->second);
						break;
					}
				}
			}
			break;
		case storelocal:
            flameback=OPERAND_GET;
            localindex=OPERAND_GET; //localindex
            v=STACK_GET; STACK_POP; //値
            //cout<<"localindex:"<<localindex<<endl;
            currentflame=CurrentFlame;
			for(i=0;i<flameback;i++){
				currentflame=currentflame->StaticLink;
			}
			(*(currentflame->Variables))[localindex].second=v;
            break;
		case storebyindex:
			{
				iopr1=STACK_GET.int_value; STACK_POP;
				auto lst=(static_pointer_cast<list<VMValue> >(STACK_GET.ref_value)); STACK_POP;
				list<VMValue>::iterator iter=lst->begin();
				for(int i=0;i<iopr1;i++){
					iter++;
				}
				(*iter)=STACK_GET; STACK_POP;
			}
			break;
		case storefield:
			{
				string name=*(static_pointer_cast<string>(ExecutableData->PublicConstantPool.GetValue(OPERAND_GET).ref_value));
				shared_ptr<DataObject> obj=(static_pointer_cast<DataObject>(STACK_GET.ref_value)); STACK_POP;
				map<string,VMValue>::iterator iter;
				for(iter=obj->MemberMap->begin();iter!=obj->MemberMap->end();iter++){
					if(iter->first==name){
						iter->second=STACK_GET;
						break;
					}
				}
			}
			break;
        default:
            error("不正な命令です...code("+IntToString(bytecode)+")");
            break;
        }
    }

}

