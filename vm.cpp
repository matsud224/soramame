#include "vm.h"
#include "utility.h"
#include <string>
#include "ast_etc.h"
#include "statement.h"
#include "type.h"
#include "expression.h"
#include "basic_object.h"
#include "common.h"
#include <map>
#include <thread>
#include <memory>
#include <mutex>
#include <array>
#include <condition_variable>

#define STACK_POP CurrentFlame->OperandStack.pop_back();
#define STACK_PUSH(x) CurrentFlame->OperandStack.push_back((x))
#define STACK_GET CurrentFlame->OperandStack.back()
#define OPERAND_GET (*(CurrentFlame->CodePtr))[CurrentFlame->PC++]
#define GET_CONSTANT(x) VM::PublicConstantPool.GetValue((x))

using namespace std;





map<pair<string,string>, void (*)(shared_ptr<Flame>) > VM::BuiltinFunctionList; //関数名と型名から関数を引っ張ってくる
ConstantPool VM::PublicConstantPool;

shared_ptr<Flame> VM::GetInitialFlame(shared_ptr<Executable> execdata)
{
    //トップレベルのフレームを作成
    shared_ptr< vector< pair<string,VMValue> > > toplevel_vars=make_shared<vector< pair<string,VMValue> > >();
	shared_ptr<Flame> tl_flame=make_shared<Flame>(toplevel_vars,execdata->Bootstrap,nullptr,nullptr,nullptr);

    //ローカル変数の準備
	for (int i = 0; i < execdata->LocalVariables->size(); i++){
		VMValue v;
		(*toplevel_vars).push_back(pair<string,VMValue>(Var2Str(execdata->LocalVariables->at(i)),v)); //ローカル変数はのちに正しく初期化される
	}

    return tl_flame;
}

VMValue VM::Run(shared_ptr<Flame> CurrentFlame,bool currflame_only){
    VMValue v;
    int iopr1,iopr2;
    bool bopr1,bopr2;
    double dopr1,dopr2;
    int flameback,localindex;
	//int maxsize=0;
	int i;int bytecode;
	shared_ptr<Flame> initial_flame=CurrentFlame; //呼び出し時のフレーム

	try{
		while (true){
			/*if(CurrentFlame!=nullptr && CurrentFlame->FunctionInfo!=nullptr){
				cout<<CurrentFlame->FunctionInfo->Name <<" : "<<CurrentFlame->PC<<endl;
			}*/
			//v = VMValue();

			/*if(maxsize<CurrentFlame->OperandStack.size()){
				maxsize=CurrentFlame->OperandStack.size();
				cout<<"MAX UPDATED: "<<maxsize<<endl;
			}*/
			/*if (currflame_only && CurrentFlame == initial_flame->DynamicLink){
				v.primitive.int_value = 0;
				return v;
			}*/
			bytecode = OPERAND_GET;
			switch (bytecode){
			case ipush:
				v.primitive.int_value = OPERAND_GET;
				STACK_PUSH(v);
				break;
			case pushim1:
				v.primitive.int_value = -1;
				STACK_PUSH(v);
				break;
			case pushi0:
				v.primitive.int_value = 0;
				STACK_PUSH(v);
				break;
			case pushi1:
				v.primitive.int_value = 1;
				STACK_PUSH(v);
				break;
			case pushi2:
				v.primitive.int_value = 2;
				STACK_PUSH(v);
				break;
			case pushi3:
				v.primitive.int_value = 3;
				STACK_PUSH(v);
				break;
			case pushi4:
				v.primitive.int_value = 4;
				STACK_PUSH(v);
				break;
			case pushi5:
				v.primitive.int_value = 5;
				STACK_PUSH(v);
				break;
			case bpush:
				v.primitive.bool_value = OPERAND_GET == 0 ? false : true;
				STACK_PUSH(v);
				break;
			case pushnull:
				v.ref_value = nullptr;
				STACK_PUSH(v);
				break;
			case ldc:
				v = GET_CONSTANT(OPERAND_GET);
				STACK_PUSH(v);
				break;
			case iadd:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.int_value = iopr1 + iopr2;
				STACK_PUSH(v);
				break;
			case dadd:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.double_value = dopr1 + dopr2;
				STACK_PUSH(v);
				break;
			case bor:
				bopr1 = STACK_GET.primitive.bool_value;  STACK_POP;
				bopr2 = STACK_GET.primitive.bool_value;  STACK_POP;
				v.primitive.bool_value = bopr1 || bopr2;
				STACK_PUSH(v);
				break;
			case isub:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.int_value = iopr1 - iopr2;
				STACK_PUSH(v);
				break;
			case dsub:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.double_value = dopr1 - dopr2;
				STACK_PUSH(v);
				break;
			case imul:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.int_value = iopr1*iopr2;
				STACK_PUSH(v);
				break;
			case dmul:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.double_value = dopr1*dopr2;
				STACK_PUSH(v);
				break;
			case band:
				bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
				bopr2 = STACK_GET.primitive.bool_value; STACK_POP;
				v.primitive.bool_value = bopr1 && bopr2;
				STACK_PUSH(v);
				break;
			case idiv:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				if (iopr2 == 0){ throw range_error("0除算"); }
				v.primitive.int_value = iopr1 / iopr2;
				STACK_PUSH(v);
				break;
			case ddiv:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.double_value = dopr1 / dopr2;
				STACK_PUSH(v);
				break;
			case imod:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				if (iopr2 == 0){ throw range_error("0除算"); }
				v.primitive.int_value = iopr1%iopr2;
				STACK_PUSH(v);
				break;
			case ineg:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.int_value = iopr1*(-1);
				STACK_PUSH(v);
				break;
			case dneg:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.double_value = dopr1*(-1.0);
				STACK_PUSH(v);
				break;
			case bnot:
				bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
				v.primitive.bool_value = !bopr1;
				STACK_PUSH(v);
				break;
			case ilshift:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.int_value = iopr1 << iopr2;
				STACK_PUSH(v);
				break;
			case irshift:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.int_value = iopr1 >> iopr2;
				STACK_PUSH(v);
				break;
			case icmpeq:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.bool_value = iopr1 == iopr2;
				STACK_PUSH(v);
				break;
			case icmpne:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.bool_value = iopr1 != iopr2;
				STACK_PUSH(v);
				break;
			case icmplt:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.bool_value = iopr1 < iopr2;
				STACK_PUSH(v);
				break;
			case icmple:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.bool_value = iopr1 <= iopr2;
				STACK_PUSH(v);
				break;
			case icmpgt:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.bool_value = iopr1 > iopr2;
				STACK_PUSH(v);
				break;
			case icmpge:
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				iopr2 = STACK_GET.primitive.int_value; STACK_POP;
				v.primitive.bool_value = iopr1 >= iopr2;
				STACK_PUSH(v);
				break;
			case dcmpeq:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.bool_value = dopr1 == dopr2;
				STACK_PUSH(v);
				break;
			case dcmpne:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.bool_value = dopr1 != dopr2;
				STACK_PUSH(v);
				break;
			case dcmplt:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.bool_value = dopr1 < dopr2;
				STACK_PUSH(v);
				break;
			case dcmple:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.bool_value = dopr1 <= dopr2;
				STACK_PUSH(v);
				break;
			case dcmpgt:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.bool_value = dopr1 > dopr2;
				STACK_PUSH(v);
				break;
			case dcmpge:
				dopr1 = STACK_GET.primitive.double_value; STACK_POP;
				dopr2 = STACK_GET.primitive.double_value; STACK_POP;
				v.primitive.bool_value = dopr1 >= dopr2;
				STACK_PUSH(v);
				break;
			case bcmpeq:
				bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
				bopr2 = STACK_GET.primitive.bool_value; STACK_POP;
				v.primitive.bool_value = bopr1 == bopr2;
				STACK_PUSH(v);
				break;
			case bcmpne:
				bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
				bopr2 = STACK_GET.primitive.bool_value; STACK_POP;
				v.primitive.bool_value = bopr1 != bopr2;
				STACK_PUSH(v);
				break;
			case invoke:
			{
				shared_ptr<ClosureObject> cobj = static_pointer_cast<ClosureObject>(STACK_GET.ref_value); STACK_POP;
				if (cobj == nullptr){ throw runtime_error("NullPointerException"); }
				shared_ptr<FunctionObject> callee = cobj->FunctionRef;

				//cout<<callee->Name << endl;
				bool is_tail = OPERAND_GET == 0 ? false : true;
				bool is_async = OPERAND_GET == 0 ? false : true;

				if (callee->isBuiltin){
					//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
					string builtin_name = callee->Name;
					string typestr = callee->TypeInfo->GetName();

					if (is_async){
						thread t(VM::BuiltinFunctionList[pair<string, string>(builtin_name, typestr)], CurrentFlame);
						t.detach();
					}
					else{
						VM::BuiltinFunctionList[pair<string, string>(builtin_name, typestr)](CurrentFlame);
					}
				}
				else{
					shared_ptr< vector< pair<string, VMValue> > > vars = make_shared<vector< pair<string, VMValue> > >();
					vars->reserve(callee->LocalVariables->size());
					//引数の準備
					for (auto iter = callee->Args->rbegin(); iter != callee->Args->rend(); iter++){
						(*vars).push_back(pair<string, VMValue>(Var2Str(*iter), STACK_GET));
						STACK_POP;
					}
					//ローカル変数の準備
					for (int i = callee->Args->size(); i < callee->LocalVariables->size(); i++){
						VMValue v; v.primitive.int_value = 0;
						(*vars).push_back(pair<string, VMValue>(Var2Str(callee->LocalVariables->at(i)), v)); //ローカル変数はすべて0に初期化される
					}

					//フレームを作成
					shared_ptr<Flame> inv_flame = make_shared<Flame>(vars, callee->bytecodes, is_tail ? CurrentFlame->DynamicLink : CurrentFlame, cobj->ParentFlame,callee);

					if (is_async){
						//cout<<"thread started!"<<endl;
						inv_flame->DynamicLink = nullptr;
						thread t(VM::Run, inv_flame, true);
						t.detach();

					}
					else{
						CurrentFlame = inv_flame;
					}
				}
			}
			break;
			case ret:
				CurrentFlame = CurrentFlame->DynamicLink;
				if (CurrentFlame == nullptr){
					v.primitive.int_value = 0;
					return v;
				}
				break;
			case ret_withvalue:
				v = STACK_GET; STACK_POP;
				/*if (CurrentFlame->DynamicLink->DynamicLink == nullptr){
					//ブートストラップコードへのreturn...
					return v;
				}*/
				CurrentFlame = CurrentFlame->DynamicLink;
				if (!(CurrentFlame == nullptr)){
					STACK_PUSH(v);
				}else{
					v.primitive.int_value = 0;
					return v;
				}
				break;
			case makeclosure:
			{
				//オペランドにpoolindexをとり、クロージャオブジェクトを生成
				iopr1 = OPERAND_GET; //poolindex
				shared_ptr<ClosureObject> cobj = make_shared<ClosureObject>(static_pointer_cast<FunctionObject>(VM::PublicConstantPool.GetValue(iopr1).ref_value), CurrentFlame);
				v.ref_value = cobj;
				STACK_PUSH(v);
				v.ref_value=nullptr;
			}
			break;
			case skip:
				iopr1 = OPERAND_GET;
				CurrentFlame->PC += iopr1;
				break;
			case iffalse_skip:
				iopr1 = OPERAND_GET;
				bopr1 = STACK_GET.primitive.bool_value;  STACK_POP;
				if (!bopr1){
					CurrentFlame->PC += iopr1;
				}
				break;
			case back:
				iopr1 = OPERAND_GET;
				CurrentFlame->PC -= iopr1;
				break;
			case makelist:
			{
				auto newlist = make_shared< list<VMValue> >();
				iopr1 = OPERAND_GET; //リストサイズ
				for (int i = 0; i < iopr1; i++){
					VMValue lv;
					lv = STACK_GET; STACK_POP; //リストの要素
					newlist->push_back(lv);
				}
				VMValue v2; v2.ref_value = newlist;
				STACK_PUSH(v2);
				v2.ref_value=nullptr;
			}
			break;
			case makedata:
			{
				shared_ptr<map<string, VMValue> > newmap = make_shared< map<string, VMValue> >();
				string originalname = *(static_pointer_cast<string>(GET_CONSTANT(OPERAND_GET).ref_value));
				iopr1 = OPERAND_GET; //メンバ数
				for (int i = 0; i < iopr1; i++){
					string membername = *(static_pointer_cast<string>(GET_CONSTANT(STACK_GET.primitive.int_value).ref_value)); STACK_POP;
					VMValue vtemp = STACK_GET; STACK_POP; //リストの要素
					(*newmap)[membername] = vtemp;
				}
				shared_ptr<DataObject> newdata = make_shared<DataObject>(originalname, newmap);
				VMValue v2; v2.ref_value = newdata;
				STACK_PUSH(v2);
				v2.ref_value=nullptr;
			}
			break;
			case loadlocal:
			{
				flameback = OPERAND_GET;
				localindex = OPERAND_GET;
				shared_ptr<Flame> fp = CurrentFlame;
				for (i = 0; i < flameback; i++){
					fp = fp->StaticLink;
				}
				STACK_PUSH((*(fp->Variables))[localindex].second);

			}
			break;
			case loadlocal00:
			{
				STACK_PUSH((*(CurrentFlame->Variables))[0].second);
			}
			break;
			case loadlocal01:
			{
				STACK_PUSH((*(CurrentFlame->Variables))[1].second);
			}
			break;
			case loadlocal02:
			{
				STACK_PUSH((*(CurrentFlame->Variables))[2].second);
			}
			break;
			case loadlocal03:
			{
				STACK_PUSH((*(CurrentFlame->Variables))[3].second);
			}
			break;
			case loadlocal04:
			{
				STACK_PUSH((*(CurrentFlame->Variables))[4].second);
			}
			break;
			case loadlocal05:
			{
				STACK_PUSH((*(CurrentFlame->Variables))[5].second);
			}
			break;
			case loadbyindex:
			{
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				shared_ptr<list<VMValue> > lst = static_pointer_cast<list<VMValue>>(STACK_GET.ref_value); STACK_POP;
				if (lst->size() <= iopr1){ throw out_of_range("Out of range"); }
				list<VMValue>::iterator iter = lst->begin();
				for (int i = 0; i < iopr1; i++){
					iter++;
				}
				STACK_PUSH(*iter);
			}
			break;
			case loadfield:
			{
				string name = *(static_pointer_cast<string>(VM::PublicConstantPool.GetValue(OPERAND_GET).ref_value));
				shared_ptr<DataObject> obj = (static_pointer_cast<DataObject>(STACK_GET.ref_value)); STACK_POP;
				map<string, VMValue>::iterator iter;
				for (iter = obj->MemberMap->begin(); iter != obj->MemberMap->end(); iter++){
					if (iter->first == name){
						STACK_PUSH(iter->second);
						break;
					}
				}
			}
			break;
			case storelocal:
			{
				flameback = OPERAND_GET;
				localindex = OPERAND_GET; //localindex
				v = STACK_GET; STACK_POP; //値
				//cout<<"localindex:"<<localindex<<endl;
				shared_ptr<Flame> fp = CurrentFlame;
				for (i = 0; i < flameback; i++){
					fp = fp->StaticLink;
				}
				(*(fp->Variables))[localindex].second = v;

			}
			break;
			case storelocal00:
			{
				v = STACK_GET; STACK_POP; //値
				(*(CurrentFlame->Variables))[0].second = v;
			}
			break;
			case storelocal01:
			{
				v = STACK_GET; STACK_POP; //値
				(*(CurrentFlame->Variables))[1].second = v;
			}
			break;
			case storelocal02:
			{
				v = STACK_GET; STACK_POP; //値
				(*(CurrentFlame->Variables))[2].second = v;
			}
			break;
			case storelocal03:
			{
				v = STACK_GET; STACK_POP; //値
				(*(CurrentFlame->Variables))[3].second = v;
			}
			break;
			case storelocal04:
			{
				v = STACK_GET; STACK_POP; //値
				(*(CurrentFlame->Variables))[4].second = v;
			}
			break;
			case storelocal05:
			{
				v = STACK_GET; STACK_POP; //値
				(*(CurrentFlame->Variables))[5].second = v;
			}
			break;
			case storebyindex:
			{
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				auto lst = (static_pointer_cast<list<VMValue>>(STACK_GET.ref_value)); STACK_POP;
				if (lst->size() <= iopr1){ throw out_of_range("Out of range"); }
				list<VMValue>::iterator iter = lst->begin();
				for (int i = 0; i < iopr1; i++){
					iter++;
				}
				(*iter) = STACK_GET; STACK_POP;
			}
			break;
			case storefield:
			{
				string name = *(static_pointer_cast<string>(VM::PublicConstantPool.GetValue(OPERAND_GET).ref_value));
				shared_ptr<DataObject> obj = (static_pointer_cast<DataObject>(STACK_GET.ref_value)); STACK_POP;
				map<string, VMValue>::iterator iter;
				for (iter = obj->MemberMap->begin(); iter != obj->MemberMap->end(); iter++){
					if (iter->first == name){
						iter->second = STACK_GET; STACK_POP;
						break;
					}
				}
			}
			break;
			case makecontinuation:
			{
				vector<pair<int, vector<VMValue> > > snapshot;
				for (shared_ptr<Flame> f = CurrentFlame; f != nullptr; f = f->DynamicLink){
					snapshot.push_back(pair<int, vector<VMValue> >(f->PC, f->OperandStack));
				}
				snapshot.front().first += 5; //PCを適切な位置にする
				v.ref_value = make_shared<ContinuationObject>(snapshot, CurrentFlame);
				STACK_PUSH(v);
				v.ref_value=nullptr;
			}
			break;
			case resume_continuation:
			{
				shared_ptr<ContinuationObject> contobj = (static_pointer_cast<ContinuationObject>(STACK_GET.ref_value)); STACK_POP;
				if (contobj == nullptr){ throw runtime_error("NullPointerException"); }
				VMValue arg = STACK_GET; STACK_POP;
				shared_ptr<Flame> copyflame_current=nullptr;
				int index = 0;
				for (shared_ptr<Flame> f = contobj->StartFlame; f != nullptr; f = f->DynamicLink, index++){
					auto item = contobj->Snapshot[index];
					shared_ptr<Flame> copyflame=make_shared<Flame>(f->Variables,f->CodePtr,nullptr,f->StaticLink,f->FunctionInfo);
					copyflame->PC = item.first;
					copyflame->OperandStack = item.second;
					if(copyflame_current!=nullptr){
						copyflame_current->DynamicLink=copyflame;
					}else{
						copyflame_current=copyflame;
						CurrentFlame=copyflame;
					}
					copyflame_current=copyflame;
				}

				if(copyflame_current!=nullptr){
					copyflame_current->DynamicLink=nullptr;
				}

				CurrentFlame->OperandStack.push_back(arg);
			}
			break;
			case makechannel:
			{
				int capacity=STACK_GET.primitive.int_value; STACK_POP;
				if (capacity < 0){ throw runtime_error("InvalidArgumentException"); }
				v.ref_value = make_shared<ChannelObject>(capacity);
				STACK_PUSH(v);
				v.ref_value=nullptr;
			}
			break;
			case channel_send:
			{
				shared_ptr<ChannelObject> chan = static_pointer_cast<ChannelObject>(STACK_GET.ref_value); STACK_POP;
				if (chan == nullptr){ throw runtime_error("NullPointerException"); }

				unique_lock<mutex> lock(chan->mtx);
				//printf("send\n");
//printf("capa:%d\n",chan->Capacity);
				if(chan->SentValues.size()<chan->Capacity){
					//バッファへ入れる
					chan->SentValues.push(STACK_GET);
					STACK_POP;

					if (chan->Receivers.size() > 0){
						//寝ているスレッド（受信者）を起こす
						auto rcv = chan->Receivers.front();
						chan->Receivers.pop();
						*(rcv.second) = true;
						rcv.first->notify_one();
					}
				}else if(chan->SentValues.size()==chan->Capacity){
					//バッファへ入れる
					chan->SentValues.push(STACK_GET);
					STACK_POP;

					if (chan->Receivers.size() > 0){
						//寝ているスレッド（受信者）を起こす
						auto rcv = chan->Receivers.front();
						chan->Receivers.pop();
						*(rcv.second) = true;
						rcv.first->notify_one();
					}

					//バッファがいっぱいなので寝る
				re_wait4:
					bool is_ready = false;
					shared_ptr<condition_variable> cond = make_shared<condition_variable>();
					chan->Senders.push(pair<shared_ptr<condition_variable>, bool*>(cond, &is_ready));
					chan->Sender_ProduceValue.push(false);
					cond->wait(lock, [&]{return is_ready; });

					/*if (chan->SentValues.size() == chan->Capacity){
						goto re_wait4;
					}*/


				}else{
					//バッファがいっぱいなので寝る
				re_wait2:
					bool is_ready = false;
					shared_ptr<condition_variable> cond = make_shared<condition_variable>();
					chan->Senders.push(pair<shared_ptr<condition_variable>, bool*>(cond, &is_ready));
					chan->Sender_ProduceValue.push(true);
					cond->wait(lock, [&]{return is_ready; });

					if (chan->SentValues.size() > chan->Capacity){
						goto re_wait2;
					}

					//バッファへ入れる
					chan->SentValues.push(STACK_GET);
					STACK_POP;
				}
			}
			break;
			case channel_receive:
			{
				shared_ptr<ChannelObject> chan = static_pointer_cast<ChannelObject>(STACK_GET.ref_value); STACK_POP;
				if (chan == nullptr){ throw runtime_error("NullPointerException"); }

				unique_lock<mutex> lock(chan->mtx);

				//printf("receive\n");
//printf("capa:%d\n",chan->Capacity);

				if (chan->SentValues.size() == 0){
					//バッファに値がない
					if (chan->Senders.size() > 0){
						//寝ているスレッド（送信者）を起こす

						//Senderが値をプッシュしてくれるなら
						if(chan->Sender_ProduceValue.front()==true){
							auto snd = chan->Senders.front();
							chan->Senders.pop();
							chan->Sender_ProduceValue.pop();
							*(snd.second) = true;
							snd.first->notify_one();
						}
					}
				re_wait:
					bool is_ready = false;
					shared_ptr<condition_variable> cond = make_shared<condition_variable>();
					chan->Receivers.push(pair<shared_ptr<condition_variable>, bool*>(cond, &is_ready));
					cond->wait(lock, [&]{return is_ready; });

					if (chan->SentValues.size() == 0){
						goto re_wait;
					}

					//受け取る
					v = chan->SentValues.front();
					chan->SentValues.pop();
					STACK_PUSH(v);
				}else{
					//受け取る
					v = chan->SentValues.front();
					chan->SentValues.pop();
					STACK_PUSH(v);
					redo9:
					if (chan->Senders.size() > 0){
						//寝ているスレッド（送信者）を起こす
							auto snd = chan->Senders.front();
							chan->Senders.pop();

							*(snd.second) = true;
							snd.first->notify_one();

						if(chan->Sender_ProduceValue.front()==false){
							chan->Sender_ProduceValue.pop();
							goto redo9;
						}

						chan->Sender_ProduceValue.pop();
					}
				}

			}
			break;
			case dup:
			{
				v = STACK_GET;
				STACK_PUSH(v);
			}
			break;
			case clean:
			{
				CurrentFlame->OperandStack.clear();
			}
			break;
			case makevector:
			{
				int capacity=STACK_GET.primitive.int_value; STACK_POP;
				if (capacity < 0){ throw runtime_error("InvalidArgumentException"); }
				v.ref_value = make_shared<vector<VMValue> >(capacity);
				STACK_PUSH(v);
				v.ref_value=nullptr;
			}
			break;
			case loadbyindex_vec:
			{
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				shared_ptr<vector<VMValue> > lst = static_pointer_cast<vector<VMValue>>(STACK_GET.ref_value); STACK_POP;
				if (lst->size() <= iopr1){ throw out_of_range("Out of range"); }
				STACK_PUSH((*lst)[iopr1]);
			}
			break;
			case storebyindex_vec:
			{
				iopr1 = STACK_GET.primitive.int_value; STACK_POP;
				auto lst = (static_pointer_cast<vector<VMValue>>(STACK_GET.ref_value)); STACK_POP;
				if (lst->size() <= iopr1){ throw out_of_range("Out of range"); }
				(*lst)[iopr1] = STACK_GET; STACK_POP;
			}
			break;
			default:
				error("不正な命令です(" + IntToString(bytecode) + ")");
				break;
			}
		}
	}
	catch (exception& ex){
		cerr << endl << "Runtime Error!  " << ex.what() << endl;
		cerr << "------------------------------------" << endl;
		for (auto sf = CurrentFlame; sf != nullptr;sf=sf->StaticLink){
			for(auto x=sf->Variables->begin();x!=sf->Variables->end();x++)
			{
				cerr << (*x).first << " = " << (*x).second.primitive.int_value << endl;
			}
			cerr << endl;
		}
		cerr << "------------------------------------" << endl<<endl;

		cerr << "------------------------------------" << endl;
		for (auto df = CurrentFlame; df->FunctionInfo!=nullptr; df = df->DynamicLink){
				cerr << df->FunctionInfo->Name << " : " << df->FunctionInfo->TypeInfo->GetName() << (df->FunctionInfo->isBuiltin ? "(builtin)" : "") << "  PC = " << df->PC << endl;
		}
		cerr << "------------------------------------" << endl;

		error("");
	}

}

