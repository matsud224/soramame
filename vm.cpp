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
#define OPERAND_GET ((*(CurrentFlame->CodePtr))[CurrentFlame->PC++].immediate)
#define GET_CONSTANT(x) VM::PublicConstantPool.GetValue((x))
#define NEXT_LABEL *((*(CurrentFlame->CodePtr))[CurrentFlame->PC++].label)

using namespace std;





map<pair<string,string>, void (*)(shared_ptr<Flame>) > VM::BuiltinFunctionList; //関数名と型名から関数を引っ張ってくる
ConstantPool VM::PublicConstantPool;

shared_ptr<Flame> VM::GetInitialFlame(shared_ptr<Executable> execdata)
{
    //トップレベルのフレームを作成
    shared_ptr< vector< pair<string,VMValue> > > toplevel_vars=make_shared<vector< pair<string,VMValue> > >();
	shared_ptr<Flame> tl_flame=make_shared<Flame>(toplevel_vars,execdata->Bootstrap_labels,nullptr,nullptr,nullptr);

    //ローカル変数の準備
	for (int i = 0; i < execdata->LocalVariables->size(); i++){
		VMValue v;
		(*toplevel_vars).push_back(pair<string,VMValue>(Var2Str(execdata->LocalVariables->at(i)),v)); //ローカル変数はのちに正しく初期化される
	}

    return tl_flame;
}

void VM::BytecodeToLabels(shared_ptr< vector<int> > bc,shared_ptr< vector<label_or_immediate> > labels){
	const void **insttable=VM::Run(nullptr,false).primitive.insttable;
	label_or_immediate temp;

	for (auto i = 0; i<bc->size(); ){
		switch (bc->at(i)){
		case makedata:
		case loadlocal:
		case storelocal:
			//引数2つの命令
			temp.label=insttable[bc->at(i++)];
			labels->push_back(temp);
			temp.immediate=bc->at(i++);
			labels->push_back(temp);
			temp.immediate=bc->at(i++);
			labels->push_back(temp);
			break;
		case ipush:
		case bpush:
		case ldc:
		case makeclosure:
		case skip:
		case iffalse_skip:
		case back:
		case makelist:
		case loadfield:
		case storefield:
		case maketuple:
			//引数1つの命令
			temp.label=insttable[bc->at(i++)];
			labels->push_back(temp);
			temp.immediate=bc->at(i++);
			labels->push_back(temp);
			break;
		case pushim1:
		case pushi0:
		case pushi1:
		case pushi2:
		case pushi3:
		case pushi4:
		case pushi5:
		case pushnull:
		case iadd:
		case dadd:
		case bor:
		case isub:
		case dsub:
		case imul:
		case dmul:
		case band:
		case idiv:
		case ddiv:
		case imod:
		case ineg:
		case dneg:
		case bnot:
		case ilshift:
		case irshift:
		case icmpeq:
		case icmpne:
		case icmplt:
		case icmple:
		case icmpgt:
		case icmpge:
		case dcmpeq:
		case dcmpne:
		case dcmplt:
		case dcmple:
		case dcmpgt:
		case dcmpge:
		case bcmpeq:
		case bcmpne:
		case invoke:
		case invoke_tail:
		case invoke_async:
		case ret:
		case ret_withvalue:
		case loadlocal00:
		case loadlocal01:
		case loadlocal02:
		case loadlocal03:
		case loadlocal04:
		case loadlocal05:
		case loadbyindex:
		case storelocal00:
		case storelocal01:
		case storelocal02:
		case storelocal03:
		case storelocal04:
		case storelocal05:
		case storebyindex:
		case makecontinuation:
		case resume_continuation:
		case makechannel:
		case channel_send:
		case channel_receive:
		case dup:
		case clean:
		case makevector:
		case loadbyindex_vec:
		case storebyindex_vec:
			//引数なしの命令
			temp.label=insttable[bc->at(i++)];
			labels->push_back(temp);
			break;
		default:
			error("UNDEFINED(" + IntToString(bc->at(i)) + ")");
			break;
		}
	}
}

//第一引数がnullptrの時はinsttableを返す
VMValue VM::Run(shared_ptr<Flame> CurrentFlame,bool currflame_only){
	static const void *insttable[]={
	    &&lb_ipush,&&lb_bpush,&&lb_pushnull,&&lb_pushim1,&&lb_pushi0,&&lb_pushi1,&&lb_pushi2,&&lb_pushi3,&&lb_pushi4,&&lb_pushi5,
	    &&lb_ldc,
		&&lb_iadd,&&lb_dadd,
		&&lb_isub,&&lb_dsub,
		&&lb_imul,&&lb_dmul,
		&&lb_idiv,&&lb_ddiv,
		&&lb_band,&&lb_bor,
		&&lb_imod,
		&&lb_ineg,&&lb_bnot,&&lb_dneg,
		&&lb_ilshift,&&lb_irshift,
		&&lb_invoke,&&lb_invoke_tail,&&lb_invoke_async,
		&&lb_loadlocal,&&lb_loadbyindex,&&lb_loadfield,
		&&lb_loadlocal00,&&lb_loadlocal01,&&lb_loadlocal02,&&lb_loadlocal03,&&lb_loadlocal04,&&lb_loadlocal05,
		&&lb_ret,&&lb_ret_withvalue,
		&&lb_storelocal,&&lb_storefield,&&lb_storebyindex,
		&&lb_storelocal00,&&lb_storelocal01,&&lb_storelocal02,&&lb_storelocal03,&&lb_storelocal04,&&lb_storelocal05,
		&&lb_makeclosure,
		&&lb_skip,&&lb_iffalse_skip,&&lb_back,
		&&lb_icmpeq,&&lb_icmpne,&&lb_icmplt,&&lb_icmple,&&lb_icmpgt,&&lb_icmpge,
		&&lb_dcmpeq,&&lb_dcmpne,&&lb_dcmplt,&&lb_dcmple,&&lb_dcmpgt,&&lb_dcmpge,
		&&lb_bcmpeq,&&lb_bcmpne,
		&&lb_makelist,&&lb_makedata,
		&&lb_makecontinuation,&&lb_resume_continuation,
		&&lb_makechannel,&&lb_channel_send,&&lb_channel_receive,
		&&lb_dup,&&lb_clean,
		&&lb_makevector,&&lb_loadbyindex_vec,&&lb_storebyindex_vec,&&lb_maketuple
	};
    VMValue v;

    if(CurrentFlame==nullptr){
		v.primitive.insttable=insttable;
		return v;
	}

    int iopr1,iopr2;
    bool bopr1,bopr2;
    double dopr1,dopr2;
    int flameback,localindex;
	//int maxsize=0;
	int i;
	shared_ptr<Flame> initial_flame=CurrentFlame; //呼び出し時のフレーム


	try{
		loop_start:
			goto NEXT_LABEL;
		lb_ipush:
			v.primitive.int_value = OPERAND_GET;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushim1:
			v.primitive.int_value = -1;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushi0:
			v.primitive.int_value = 0;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushi1:
			v.primitive.int_value = 1;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushi2:
			v.primitive.int_value = 2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushi3:
			v.primitive.int_value = 3;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushi4:
			v.primitive.int_value = 4;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushi5:
			v.primitive.int_value = 5;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_bpush:
			v.primitive.bool_value = OPERAND_GET == 0 ? false : true;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_pushnull:
			v.ref_value = nullptr;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_ldc:
			v = GET_CONSTANT(OPERAND_GET);
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_iadd:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.int_value = iopr1 + iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dadd:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.double_value = dopr1 + dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_bor:
			bopr1 = STACK_GET.primitive.bool_value;  STACK_POP;
			bopr2 = STACK_GET.primitive.bool_value;  STACK_POP;
			v.primitive.bool_value = bopr1 || bopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_isub:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.int_value = iopr1 - iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dsub:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.double_value = dopr1 - dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_imul:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.int_value = iopr1*iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dmul:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.double_value = dopr1*dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_band:
			bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
			bopr2 = STACK_GET.primitive.bool_value; STACK_POP;
			v.primitive.bool_value = bopr1 && bopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_idiv:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			if (iopr2 == 0){ throw range_error("0除算"); }
			v.primitive.int_value = iopr1 / iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_ddiv:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.double_value = dopr1 / dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_imod:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			if (iopr2 == 0){ throw range_error("0除算"); }
			v.primitive.int_value = iopr1%iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_ineg:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.int_value = iopr1*(-1);
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dneg:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.double_value = dopr1*(-1.0);
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_bnot:
			bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
			v.primitive.bool_value = !bopr1;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_ilshift:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.int_value = iopr1 << iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_irshift:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.int_value = iopr1 >> iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_icmpeq:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.bool_value = iopr1 == iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_icmpne:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.bool_value = iopr1 != iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_icmplt:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.bool_value = iopr1 < iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_icmple:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.bool_value = iopr1 <= iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_icmpgt:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.bool_value = iopr1 > iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_icmpge:
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			iopr2 = STACK_GET.primitive.int_value; STACK_POP;
			v.primitive.bool_value = iopr1 >= iopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dcmpeq:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.bool_value = dopr1 == dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dcmpne:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.bool_value = dopr1 != dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dcmplt:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.bool_value = dopr1 < dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dcmple:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.bool_value = dopr1 <= dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dcmpgt:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.bool_value = dopr1 > dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_dcmpge:
			dopr1 = STACK_GET.primitive.double_value; STACK_POP;
			dopr2 = STACK_GET.primitive.double_value; STACK_POP;
			v.primitive.bool_value = dopr1 >= dopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_bcmpeq:
			bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
			bopr2 = STACK_GET.primitive.bool_value; STACK_POP;
			v.primitive.bool_value = bopr1 == bopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_bcmpne:
			bopr1 = STACK_GET.primitive.bool_value; STACK_POP;
			bopr2 = STACK_GET.primitive.bool_value; STACK_POP;
			v.primitive.bool_value = bopr1 != bopr2;
			STACK_PUSH(v);
			goto NEXT_LABEL;
		lb_invoke:
		{
			shared_ptr<ClosureObject> cobj = static_pointer_cast<ClosureObject>(STACK_GET.ref_value); STACK_POP;
			if (cobj == nullptr){ throw runtime_error("NullPointerException"); }
			shared_ptr<FunctionObject> callee = cobj->FunctionRef;

			if (callee->isBuiltin){
				//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
				string builtin_name = callee->Name;
				string typestr = callee->TypeInfo->GetName();

				VM::BuiltinFunctionList[pair<string, string>(builtin_name, typestr)](CurrentFlame);
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
				shared_ptr<Flame> inv_flame = make_shared<Flame>(vars, callee->bytecode_labels, CurrentFlame, cobj->ParentFlame,callee);

				CurrentFlame = inv_flame;

			}
		}
		goto NEXT_LABEL;
		lb_invoke_async:
		{
			shared_ptr<ClosureObject> cobj = static_pointer_cast<ClosureObject>(STACK_GET.ref_value); STACK_POP;
			if (cobj == nullptr){ throw runtime_error("NullPointerException"); }
			shared_ptr<FunctionObject> callee = cobj->FunctionRef;

			if (callee->isBuiltin){
				//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
				string builtin_name = callee->Name;
				string typestr = callee->TypeInfo->GetName();

				thread t(VM::BuiltinFunctionList[pair<string, string>(builtin_name, typestr)], CurrentFlame);
				t.detach();
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
				shared_ptr<Flame> inv_flame = make_shared<Flame>(vars, callee->bytecode_labels, CurrentFlame, cobj->ParentFlame,callee);


				//cout<<"thread started!"<<endl;
				inv_flame->DynamicLink = nullptr;
				thread t(VM::Run, inv_flame, true);
				t.detach();
			}
		}
		goto NEXT_LABEL;
		lb_invoke_tail:
		{
			shared_ptr<ClosureObject> cobj = static_pointer_cast<ClosureObject>(STACK_GET.ref_value); STACK_POP;
			if (cobj == nullptr){ throw runtime_error("NullPointerException"); }
			shared_ptr<FunctionObject> callee = cobj->FunctionRef;

			if (callee->isBuiltin){
				//ビルトイン関数の場合は、フレームを作らず、直に値をスタックに置く
				string builtin_name = callee->Name;
				string typestr = callee->TypeInfo->GetName();

				VM::BuiltinFunctionList[pair<string, string>(builtin_name, typestr)](CurrentFlame);

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
				shared_ptr<Flame> inv_flame = make_shared<Flame>(vars, callee->bytecode_labels, CurrentFlame->DynamicLink , cobj->ParentFlame,callee);

				CurrentFlame = inv_flame;

			}
		}
		goto NEXT_LABEL;
		lb_ret:
			CurrentFlame = CurrentFlame->DynamicLink;
			if (CurrentFlame == nullptr){
				v.primitive.int_value = 0;
				return v;
			}
			goto NEXT_LABEL;
		lb_ret_withvalue:
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
			goto NEXT_LABEL;
		lb_makeclosure:
		{
			//オペランドにpoolindexをとり、クロージャオブジェクトを生成
			iopr1 = OPERAND_GET; //poolindex
			shared_ptr<ClosureObject> cobj = make_shared<ClosureObject>(static_pointer_cast<FunctionObject>(VM::PublicConstantPool.GetValue(iopr1).ref_value), CurrentFlame);
			v.ref_value = cobj;
			STACK_PUSH(v);
			v.ref_value=nullptr;
		}
		goto NEXT_LABEL;
		lb_skip:
			iopr1 = OPERAND_GET;
			CurrentFlame->PC += iopr1;
			goto NEXT_LABEL;
		lb_iffalse_skip:
			iopr1 = OPERAND_GET;
			bopr1 = STACK_GET.primitive.bool_value;  STACK_POP;
			if (!bopr1){
				CurrentFlame->PC += iopr1;
			}
			goto NEXT_LABEL;
		lb_back:
			iopr1 = OPERAND_GET;
			CurrentFlame->PC -= iopr1;
			goto NEXT_LABEL;
		lb_makelist:
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
		}
		goto NEXT_LABEL;
		lb_makedata:
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
		}
		goto NEXT_LABEL;
		lb_loadlocal:
		{
			flameback = OPERAND_GET;
			localindex = OPERAND_GET;
			shared_ptr<Flame> fp = CurrentFlame;
			for (i = 0; i < flameback; i++){
				fp = fp->StaticLink;
			}
			STACK_PUSH((*(fp->Variables))[localindex].second);

		}
		goto NEXT_LABEL;
		lb_loadlocal00:
		{
			STACK_PUSH((*(CurrentFlame->Variables))[0].second);
		}
		goto NEXT_LABEL;
		lb_loadlocal01:
		{
			STACK_PUSH((*(CurrentFlame->Variables))[1].second);
		}
		goto NEXT_LABEL;
		lb_loadlocal02:
		{
			STACK_PUSH((*(CurrentFlame->Variables))[2].second);
		}
		goto NEXT_LABEL;
		lb_loadlocal03:
		{
			STACK_PUSH((*(CurrentFlame->Variables))[3].second);
		}
		goto NEXT_LABEL;
		lb_loadlocal04:
		{
			STACK_PUSH((*(CurrentFlame->Variables))[4].second);
		}
		goto NEXT_LABEL;
		lb_loadlocal05:
		{
			STACK_PUSH((*(CurrentFlame->Variables))[5].second);
		}
		goto NEXT_LABEL;
		lb_loadbyindex:
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
		goto NEXT_LABEL;
		lb_loadfield:
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
		goto NEXT_LABEL;
		lb_storelocal:
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
		goto NEXT_LABEL;
		lb_storelocal00:
		{
			v = STACK_GET; STACK_POP; //値
			(*(CurrentFlame->Variables))[0].second = v;
		}
		goto NEXT_LABEL;
		lb_storelocal01:
		{
			v = STACK_GET; STACK_POP; //値
			(*(CurrentFlame->Variables))[1].second = v;
		}
		goto NEXT_LABEL;
		lb_storelocal02:
		{
			v = STACK_GET; STACK_POP; //値
			(*(CurrentFlame->Variables))[2].second = v;
		}
		goto NEXT_LABEL;
		lb_storelocal03:
		{
			v = STACK_GET; STACK_POP; //値
			(*(CurrentFlame->Variables))[3].second = v;
		}
		goto NEXT_LABEL;
		lb_storelocal04:
		{
			v = STACK_GET; STACK_POP; //値
			(*(CurrentFlame->Variables))[4].second = v;
		}
		goto NEXT_LABEL;
		lb_storelocal05:
		{
			v = STACK_GET; STACK_POP; //値
			(*(CurrentFlame->Variables))[5].second = v;
		}
		goto NEXT_LABEL;
		lb_storebyindex:
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
		goto NEXT_LABEL;
		lb_storefield:
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
		goto NEXT_LABEL;
		lb_makecontinuation:
		{
			vector<pair<int, vector<VMValue> > > snapshot;
			for (shared_ptr<Flame> f = CurrentFlame; f != nullptr; f = f->DynamicLink){
				snapshot.push_back(pair<int, vector<VMValue> >(f->PC, f->OperandStack));
			}
			snapshot.front().first += 3; //PCを適切な位置にする
			v.ref_value = make_shared<ContinuationObject>(snapshot, CurrentFlame);
			STACK_PUSH(v);
			v.ref_value=nullptr;
		}
		goto NEXT_LABEL;
		lb_resume_continuation:
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
		goto NEXT_LABEL;
		lb_makechannel:
		{
			int capacity=STACK_GET.primitive.int_value; STACK_POP;
			if (capacity < 0){ throw runtime_error("InvalidArgumentException"); }
			v.ref_value = make_shared<ChannelObject>(capacity);
			STACK_PUSH(v);
			v.ref_value=nullptr;
		}
		goto NEXT_LABEL;
		lb_channel_send:
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
		goto NEXT_LABEL;
		lb_channel_receive:
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
		goto NEXT_LABEL;
		lb_dup:
		{
			v = STACK_GET;
			STACK_PUSH(v);
		}
		goto NEXT_LABEL;
		lb_clean:
		{
			CurrentFlame->OperandStack.clear();
		}
		goto NEXT_LABEL;
		lb_makevector:
		{
			int capacity=STACK_GET.primitive.int_value; STACK_POP;
			if (capacity < 0){ throw runtime_error("InvalidArgumentException"); }
			v.ref_value = make_shared<vector<VMValue> >(capacity);
			STACK_PUSH(v);
			v.ref_value=nullptr;
		}
		goto NEXT_LABEL;
		lb_loadbyindex_vec:
		{
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			shared_ptr<vector<VMValue> > lst = static_pointer_cast<vector<VMValue>>(STACK_GET.ref_value); STACK_POP;
			if (lst->size() <= iopr1){ throw out_of_range("Out of range"); }
			STACK_PUSH((*lst)[iopr1]);
		}
		goto NEXT_LABEL;
		lb_storebyindex_vec:
		{
			iopr1 = STACK_GET.primitive.int_value; STACK_POP;
			auto lst = (static_pointer_cast<vector<VMValue>>(STACK_GET.ref_value)); STACK_POP;
			if (lst->size() <= iopr1){ throw out_of_range("Out of range"); }
			(*lst)[iopr1] = STACK_GET; STACK_POP;
		}
		goto NEXT_LABEL;
		lb_maketuple:
		{
			iopr1 = OPERAND_GET; //リストサイズ
			auto newlist = make_shared< vector<VMValue> >(iopr1);

			for (int i = 0; i < iopr1; i++){
				VMValue lv;
				lv = STACK_GET; STACK_POP; //リストの要素
				(*newlist)[i]=lv;
			}
			VMValue v2; v2.ref_value = newlist;
			STACK_PUSH(v2);
		}
		goto NEXT_LABEL;
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

