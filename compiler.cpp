#include "compiler.h"
#include "ast_etc.h"
#include "statement.h"
#include "type.h"
#include "expression.h"
#include "builtinfunctions.h"
#include <typeinfo>
#include <memory>

void Compiler::RegisterBuiltinFunction(string name,void (*funcptr)(shared_ptr<Flame>),shared_ptr<vector< pair<string,shared_ptr<TypeAST> > > > arg,shared_ptr<TypeAST>  rettype,bool ctfeable){
	genInfo->TopLevelFunction.push_back(make_shared<FunctionAST>(genInfo,name,arg,rettype,ctfeable));
	VM::BuiltinFunctionList[pair<string,string>(name,genInfo->TopLevelFunction.back()->TypeInfo->GetName())]=funcptr;
}

void Compiler::ASTgen()
{
    //演算子の準備
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("+",OperatorInfo(Binary,Left,20)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("-",OperatorInfo(Binary,Left,20)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("*",OperatorInfo(Binary,Left,40)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("/",OperatorInfo(Binary,Left,40)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("<<",OperatorInfo(Binary,Left,10)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >(">>",OperatorInfo(Binary,Left,10)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("%",OperatorInfo(Binary,Left,40)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("&&",OperatorInfo(Binary,Left,5)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("||",OperatorInfo(Binary,Left,5)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("!",OperatorInfo(Unary,Right,70)));
	genInfo->OperatorList.insert(pair<string, OperatorInfo >("!", OperatorInfo(Binary, Right, 2)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("?",OperatorInfo(Unary,Right,70)));

	genInfo->OperatorList.insert(pair<string,OperatorInfo >("-",OperatorInfo(Unary,Right,70)));

	genInfo->OperatorList.insert(pair<string, OperatorInfo >("@+", OperatorInfo(Binary, Left, 20)));

	genInfo->OperatorList.insert(pair<string, OperatorInfo >("@?", OperatorInfo(Unary, Right, 70)));
	genInfo->OperatorList.insert(pair<string, OperatorInfo >("@>", OperatorInfo(Unary, Right, 70)));
	genInfo->OperatorList.insert(pair<string, OperatorInfo >("@<", OperatorInfo(Unary, Right, 70)));

    genInfo->OperatorList.insert(pair<string,OperatorInfo >(">",OperatorInfo(Binary,Left,8)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >(">=",OperatorInfo(Binary,Left,8)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("<",OperatorInfo(Binary,Left,8)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("<=",OperatorInfo(Binary,Left,8)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("!=",OperatorInfo(Binary,Left,8)));
    genInfo->OperatorList.insert(pair<string,OperatorInfo >("==",OperatorInfo(Binary,Left,8)));

    genInfo->OperatorList.insert(pair<string,OperatorInfo >("=",OperatorInfo(Binary,Right,2)));



    vector< pair<string,shared_ptr<TypeAST> > > arglist;

	RegisterBuiltinFunction("rand",rand_int,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("int"),false);
	RegisterBuiltinFunction("hardware_concurrency",hw_concurrency,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("int"),false);

	#ifdef USE_GLUT
	RegisterBuiltinFunction("glut_mainloop",glut_mainloop,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_clear",glut_clear,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_flush",glut_flush,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_point",glut_begin_point,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_line",glut_begin_line,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_strip",glut_begin_strip,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_lineloop",glut_begin_lineloop,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_triangle",glut_begin_triangle,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_quad",glut_begin_quad,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_trianglefan",glut_begin_trianglefan,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_begin_polygon",glut_begin_polygon,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_end",glut_end,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	RegisterBuiltinFunction("glut_postredisp", glut_postredisp, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(arglist), make_shared<BasicTypeAST>("void"), false);

	vector<shared_ptr<TypeAST> > voidarg;voidarg.push_back(make_shared<BasicTypeAST>("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",make_shared<FunctionTypeAST>(voidarg)));
	RegisterBuiltinFunction("glut_setdisplayfunc",glut_setdispfunc,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	arglist.clear();

	vector<shared_ptr<TypeAST> > intarg;
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",make_shared<FunctionTypeAST>(intarg)));
	RegisterBuiltinFunction("glut_setkeyboardfunc",glut_setkeyboardfunc,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	arglist.clear();

	intarg.clear();
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",make_shared<FunctionTypeAST>(intarg)));
	RegisterBuiltinFunction("glut_setmousefunc",glut_setmousefunc,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	arglist.clear();

	intarg.clear();
	intarg.push_back(make_shared<BasicTypeAST>("int"));
	intarg.push_back(make_shared<BasicTypeAST>("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("msec",make_shared<BasicTypeAST>("int")));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("callback",make_shared<FunctionTypeAST>(intarg)));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("value",make_shared<BasicTypeAST>("int")));
	RegisterBuiltinFunction("glut_settimerfunc",glut_settimerfunc,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	arglist.clear();
	#endif

	arglist.clear();
    arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",make_shared<BasicTypeAST>("int")));
    RegisterBuiltinFunction("print_int",print_int,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

    RegisterBuiltinFunction("sleep",sleep_msec,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",make_shared<BasicTypeAST>("double"));
    RegisterBuiltinFunction("print_double",print_double,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",make_shared<BasicTypeAST>("bool"));
    RegisterBuiltinFunction("print_bool",print_bool,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("str",make_shared<BasicTypeAST>("string"));
    RegisterBuiltinFunction("print",print_str,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

	#ifdef USE_GLUT
    RegisterBuiltinFunction("glut_openwindow",glut_openwindow,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	#endif

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",make_shared<BasicTypeAST>("int"));
    RegisterBuiltinFunction("abs",abs_int,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("int"),true);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",make_shared<BasicTypeAST>("int"));
    RegisterBuiltinFunction("i2d",int2double,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",make_shared<BasicTypeAST>("double"));
    RegisterBuiltinFunction("d2i",double2int,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("int"),true);

	RegisterBuiltinFunction("sin",math_sin,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);
	RegisterBuiltinFunction("cos",math_cos,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);
	RegisterBuiltinFunction("tan",math_tan,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);
	RegisterBuiltinFunction("asin",math_asin,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);
	RegisterBuiltinFunction("acos",math_acos,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);
	RegisterBuiltinFunction("atan",math_atan,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);
	RegisterBuiltinFunction("sqrt",math_sqrt,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("double"),true);

	arglist[0]=pair<string,shared_ptr<TypeAST> >("val1",make_shared<BasicTypeAST>("int"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val2",make_shared<BasicTypeAST>("int")));
    RegisterBuiltinFunction("pow",pow_int,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("int"),true);

	#ifdef USE_GLUT
	RegisterBuiltinFunction("glut_vertex2i",glut_vertex2i,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val3",make_shared<BasicTypeAST>("int")));
	RegisterBuiltinFunction("glut_color3i",glut_color3i,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);

	RegisterBuiltinFunction("glut_char",glut_char,make_shared<vector< pair<string,shared_ptr<TypeAST> > > >(arglist),make_shared<BasicTypeAST>("void"),false);
	#endif

	//演算子から関数呼び出しへの橋渡し（引数などの情報は無視される）
	RegisterBuiltinFunction("!op_append_list", op_append_list, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);
	RegisterBuiltinFunction("!op_append_str", op_append_str, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);
	RegisterBuiltinFunction("!op_cdr", op_cdr, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);
	RegisterBuiltinFunction("!op_car", op_car, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);
	RegisterBuiltinFunction("!op_cons", op_cons, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);
	RegisterBuiltinFunction("!op_length_list", op_length_list, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);
	RegisterBuiltinFunction("!op_length_str", op_length_str, make_shared<vector< pair<string, shared_ptr<TypeAST> > > >(), make_shared<BasicTypeAST>("<...>"), true);


	while(!parser->IsAccepted()){
		pair<Symbol,TokenValue> token=lexer->Get();
		//cout <<"token: "<<Parser::Symbol2Str(token.first)<<endl;
		parser->Put(lexer,genInfo,token);
	}

}

void Compiler::TypeCheck()
{
    shared_ptr<vector<Environment> > environment=make_shared<vector<Environment> >(); //現在可視状態にある変数（トップレベルの関数も変数とみなす）のスタック（フレームを積み重ねていく）

    vector<shared_ptr<FunctionAST> >::iterator fun_iter;
    vector<shared_ptr<VariableDefStatementAST> >::iterator var_iter;

    Environment rootflame;
	rootflame.is_internalblock=false;

	rootflame.LocalVariablesPtr=genInfo->LocalVariables;

	int cnt=0;
    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		EnvItem ei;ei.VariableInfo=pair<string,shared_ptr<TypeAST> >((*fun_iter)->Name,(*fun_iter)->TypeInfo);ei.LocalIndex=cnt;
		rootflame.Items.push_back(ei);
		cnt++;
    }
    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
        EnvItem ei;ei.VariableInfo=*((*var_iter)->Variable);ei.LocalIndex=cnt;
        rootflame.Items.push_back(ei);
        cnt++;
    }
    environment->push_back(rootflame); //トップレベルのフレーム

    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		if((*fun_iter)->isBuiltin==false){
			(*fun_iter)->CheckType(environment,genInfo,genInfo->LocalVariables);
		}
		rootflame.LocalVariablesPtr->push_back(pair<string,shared_ptr<TypeAST> >((*fun_iter)->Name,(*fun_iter)->TypeInfo));
    }
    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
        (*var_iter)->CheckType(environment,genInfo,genInfo->LocalVariables);
        //rootflame.LocalVariablesPtr->push_back(*((*var_iter)->Variable));
    }
}


void Compiler::Codegen()
{
    vector<shared_ptr<FunctionAST> >::iterator iterf;
    vector<shared_ptr<VariableDefStatementAST> >::iterator iterv;

    for(iterf=genInfo->TopLevelFunction.begin();iterf!=genInfo->TopLevelFunction.end();iterf++){
		//組み込み関数のコード生成は行わない
        if((*iterf)->isBuiltin==false){
            (*iterf)->Codegen(nullptr,genInfo);
        }
    }

    //main関数（引数なしで、戻り値がvoidであるもの）を探す
    int main_index=-1;
    for(iterf=genInfo->TopLevelFunction.begin();iterf!=genInfo->TopLevelFunction.end();iterf++){
        if((*iterf)->Name=="main" && (*iterf)->TypeInfo->GetName()=="fun()=>void"){
            main_index=(*iterf)->PoolIndex;
        }
    }
    if(main_index==-1){
        error("適切なmain関数が見つかりません。");
    }
    genInfo->MainFuncPoolIndex=main_index;

	genInfo->Bootstrap->clear();
	//ブートストラップローダをつくる

	//トップレベルの関数の初期化
	int counter=0;
    for(iterf=genInfo->TopLevelFunction.begin();iterf!=genInfo->TopLevelFunction.end();iterf++){
		//トップレベル関数はもうコード生成が済んでいるのでCodegenはダメ
		genInfo->Bootstrap->push_back(makeclosure);
        genInfo->Bootstrap->push_back((*iterf)->PoolIndex);
        genInfo->Bootstrap->push_back(storelocal);
		genInfo->Bootstrap->push_back(0);
		genInfo->Bootstrap->push_back(counter++);
    }
    genInfo->Bootstrap->push_back(clean);
	//グローバル変数の初期化
    for(iterv=genInfo->TopLevelVariableDef.begin();iterv!=genInfo->TopLevelVariableDef.end();iterv++){
		(*iterv)->Codegen(genInfo->Bootstrap,genInfo);
		genInfo->Bootstrap->push_back(clean);
    }
    //main関数の呼び出し
    genInfo->Bootstrap->push_back(makeclosure);
	genInfo->Bootstrap->push_back(genInfo->MainFuncPoolIndex);
	genInfo->Bootstrap->push_back(invoke);
	genInfo->Bootstrap->push_back(0);
	genInfo->Bootstrap->push_back(0);
	genInfo->Bootstrap->push_back(ret);

}

void Compiler::CTFE(int loop/*繰り返し回数*/){
	/*vector<shared_ptr<ExprAST> > call_list;
	vector< shared_ptr<CallExprAST> > exec_list;
	vector<shared_ptr<ExprAST> > temp;
	vector<shared_ptr<FunctionAST> >::iterator iterf;
	vector<shared_ptr<ExprAST> >::iterator citer;
	vector<shared_ptr<CallExprAST> >::iterator iter;
	int exec_count=0;
	for(int i=0;i<loop;i++){
		call_list.clear();
		exec_list.clear();
		for(iterf=genInfo->TopLevelFunction.begin();iterf!=genInfo->TopLevelFunction.end();iterf++){
			if(!(*iterf)->isBuiltin){
				temp=(*iterf)->GetCallExprList();
				call_list.insert(call_list.end(),temp.begin(),temp.end());
			}
		}

		for(citer=call_list.begin();citer!=call_list.end();citer++){
            shared_ptr<CallExprAST> target=dynamic_pointer_cast<CallExprAST>(*citer);
			if(typeid(VariableExprAST)==typeid(*(target->callee))){
				if(dynamic_pointer_cast<VariableExprAST >(target->callee)->is_toplevel_func && genInfo->TopLevelFunction[dynamic_pointer_cast<VariableExprAST >(target->callee)->LocalIndex]->IsCTFEable(genInfo,-1)){
					vector<shared_ptr<ExprAST> >::iterator aiter;
					for(aiter=target->args->begin();aiter!=target->args->end();aiter++){
						if((*aiter)->IsCTFEable(genInfo,-1)==false || (*aiter)->IsConstant()==false){
							goto next_c;
						}
					}
					//事前計算対象に選ばれた
					exec_list.push_back(target);
				}
			}
			next_c:
			1+1;//dummy expression
		}

		if(exec_list.size()==0){
			break;
		}

		vector<int> preexec_code; //事前実行する関数呼び出しをズラッと並べたもの
		for(iter=exec_list.begin();iter!=exec_list.end();iter++){
			exec_count++;

			if((*iter)->TypeInfo->GetName()=="void"){
				VMValue v;v.primitive.int_value=0;
				(*iter)->CalculatedValue=v;
				continue;
			}

			preexec_code.clear();
			//トップレベルの関数（組み込み関数も含める）のCodegenInfo.ToplevelFuncList
			//のインデックスとLocalIndexは一致すると仮定している(closureがはいってる)
			for(vector<shared_ptr<ExprAST> >::reverse_iterator eiter=(*iter)->args->rbegin();eiter!=(*iter)->args->rend();eiter++){
				preexec_code.push_back(ipush);
				preexec_code.push_back((*eiter)->GetVMValue(genInfo));
			}

			preexec_code.push_back(iloadlocal);
			preexec_code.push_back(0); //FlameBack
			preexec_code.push_back(dynamic_cast<Variableshared_ptr<ExprAST> >((*iter)->callee)->LocalIndex);
			preexec_code.push_back(invoke);
			preexec_code.push_back(0);
			preexec_code.push_back(0);
			preexec_code.push_back(ret);

			genInfo->Bootstrap=preexec_code;
			VM vm(genInfo);
			vm.Init();
			(*iter)->callee=nullptr;
			(*iter)->CalculatedValue=vm.Run(false);

		}

		Codegen();
	}

	cout<<exec_count<<"箇所の関数呼び出しを事前実行しました"<<endl;

	return;*/
}
