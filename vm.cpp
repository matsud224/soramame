#include "vm.h"
#include "utility.h"
#include <string>
#include "ast.h"
#include "basic_object.h"
#include <map>
#include <memory>


#define STACK_POP Environment.back()->OperandStack.pop()
#define STACK_PUSH(x) Environment.back()->OperandStack.push((x))
#define STACK_GET Environment.back()->OperandStack.top()
#define OPERAND_GET (*(Environment.back()->CodePtr))[Environment.back()->PC++]
#define GET_CONSTANT(x) ExecutableData->PublicConstantPool.GetValue((x))

using namespace std;


void VM::Init()
{
    //トップレベルのフレームを作成
    shared_ptr< vector< pair<string,VMValue> > > toplevel_vars=make_shared<vector< pair<string,VMValue> > >();
	shared_ptr<Flame> tl_flame=make_shared<Flame>(toplevel_vars,ExecutableData->Bootstrap,nullptr);

    /*for(unsigned int i=0;i<CodeInfo->TopLevelFunction.size();i++){
		shared_ptr<ClosureObject> cobj=make_shared<ClosureObject>(static_pointer_cast<FunctionObject>(CodeInfo->PublicConstantPool.GetValue(CodeInfo->TopLevelFunction[i]->PoolIndex).ref_value), tl_flame);
		VMValue v;v.ref_value=cobj;
        (*toplevel_vars).push_back(pair<string,VMValue>(CodeInfo->TopLevelFunction[i]->Name,v));
    }
	for(unsigned int i=0;i<CodeInfo->TopLevelVariableDef.size();i++){
		VMValue v;//初期化はのちに行われるのでゴミをつめておく
        (*toplevel_vars).push_back(pair<string,VMValue>(CodeInfo->TopLevelVariableDef[i]->Variable->first,v)); //値はとりあえず0にしておく
    }*/

    //ローカル変数の準備
	for(int i=ExecutableData->LocalVariables->size()-1;i>=0;i--){
		VMValue v;v.int_value=0;
		(*toplevel_vars).push_back(pair<string,VMValue>(ExecutableData->LocalVariables->at(i).first,v)); //ローカル変数はすべて0に初期化される
	}

    for(unsigned int i=0;i<ExecutableData->ChildPoolIndex.size();i++){
		static_pointer_cast<FunctionObject>(ExecutableData->PublicConstantPool.GetValue(ExecutableData->ChildPoolIndex[i]).ref_value)->ParentFlame=tl_flame;
	}
    Environment.push_back(tl_flame);
}

VMValue VM::Run(bool currflame_only){
    VMValue v;
    int iopr1,iopr2;
    bool bopr1,bopr2;
    double dopr1,dopr2;
    int flameback,localindex;

	int current_flame_depth=Environment.size();
    shared_ptr<Flame> currentflame;
    int counter;
	int i;int bytecode;

    while(true){
		v=VMValue();
        if(Environment.size()==0){
			v.int_value=0;
			return v;
        }
        if(currflame_only && Environment.size()<current_flame_depth){
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
            v.int_value=iopr2-iopr1;
            STACK_PUSH(v);
			break;
		case dsub:
            dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr2-dopr1;
            STACK_PUSH(v);
			break;
        case imul:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2*iopr1;
            STACK_PUSH(v);
            break;
		case dmul:
            dopr1=STACK_GET.double_value; STACK_POP;
			dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr2*dopr1;
            STACK_PUSH(v);
            break;
        case band:
            bopr1=STACK_GET.bool_value; STACK_POP;
            bopr2=STACK_GET.bool_value; STACK_POP;
            v.bool_value=bopr2 && bopr1;
            STACK_PUSH(v);
            break;
        case idiv:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2/iopr1;
            STACK_PUSH(v);
            break;
		case ddiv:
            dopr1=STACK_GET.double_value; STACK_POP;
			dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr2/dopr1;
            STACK_PUSH(v);
            break;
        case imod:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2%iopr1;
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
            v.int_value=iopr2<<iopr1;
            STACK_PUSH(v);
            break;
        case irshift:
            iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2>>iopr1;
            STACK_PUSH(v);
            break;
		case icmpeq:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr2==iopr1;
            STACK_PUSH(v);
			break;
		case icmpne:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr2!=iopr1;
            STACK_PUSH(v);
			break;
		case icmplt:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.bool_value=iopr2<iopr1;
            STACK_PUSH(v);
			break;
		case icmple:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2<=iopr1;
            STACK_PUSH(v);
			break;
		case icmpgt:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2>iopr1;
            STACK_PUSH(v);
			break;
		case icmpge:
			iopr1=STACK_GET.int_value; STACK_POP;
            iopr2=STACK_GET.int_value; STACK_POP;
            v.int_value=iopr2>=iopr1;
            STACK_PUSH(v);
			break;
		case dcmpeq:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr2==dopr1;
            STACK_PUSH(v);
			break;
		case dcmpne:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr2!=dopr1;
            STACK_PUSH(v);
			break;
		case dcmplt:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.bool_value=dopr2<dopr1;
            STACK_PUSH(v);
			break;
		case dcmple:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr2<=dopr1;
            STACK_PUSH(v);
			break;
		case dcmpgt:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr2>dopr1;
            STACK_PUSH(v);
			break;
		case dcmpge:
			dopr1=STACK_GET.double_value; STACK_POP;
            dopr2=STACK_GET.double_value; STACK_POP;
            v.double_value=dopr2>=dopr1;
            STACK_PUSH(v);
			break;
		case bcmpeq:
			bopr1=STACK_GET.bool_value; STACK_POP;
            bopr2=STACK_GET.bool_value; STACK_POP;
            v.bool_value=bopr2==bopr1;
            STACK_PUSH(v);
			break;
		case bcmpne:
			bopr1=STACK_GET.bool_value; STACK_POP;
            bopr2=STACK_GET.bool_value; STACK_POP;
            v.bool_value=bopr2!=bopr1;
            STACK_PUSH(v);
			break;
        case invoke:
            {
            	shared_ptr<ClosureObject> cobj=static_pointer_cast<ClosureObject>(STACK_GET.ref_value);STACK_POP;
                shared_ptr<FunctionObject> callee=cobj->FunctionRef;
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
					shared_ptr<Flame> inv_flame=make_shared<Flame>(vars,callee->bytecodes,cobj->ParentFlame);
					for(unsigned int i=0;i<callee->ChildPoolIndex->size();i++){
						//コンスタントプール内のクロージャに生成元のフレームを覚えさせる
						static_pointer_cast<FunctionObject>(ExecutableData->PublicConstantPool.GetValue(callee->ChildPoolIndex->at(i)).ref_value)->ParentFlame=inv_flame;
					}
					Environment.push_back(inv_flame);
                }
            }
            break;
        case loadlocal:
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
        case ret_withvalue:
            v=STACK_GET; STACK_POP;
            if(Environment.size()==2){
				//ブートストラップコードへのreturn...
				return v;
            }
            Environment.pop_back();
            if(!Environment.empty()){
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
			Environment.back()->PC+=iopr1;
			break;
		case iffalse_skip:
			iopr1=OPERAND_GET;
			bopr1=STACK_GET.bool_value;  STACK_POP;
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
            currentflame=Environment.back();
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

