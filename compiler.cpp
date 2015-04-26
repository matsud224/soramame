#include "compiler.h"
#include "ast.h"
#include "builtinfunctions.h"
#include <typeinfo>
#include <memory>

void Compiler::RegisterBuiltinFunction(string name,void (*funcptr)(shared_ptr<VM>),vector< pair<string,shared_ptr<TypeAST> > >* arg,shared_ptr<TypeAST>  rettype,bool ctfeable){
	genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,name,arg,rettype,ctfeable));
	genInfo->BuiltinFunctionList[pair<string,string>(name,genInfo->TopLevelFunction.back()->TypeInfo->GetName())]=funcptr;
}

void Compiler::ASTgen()
{
    //演算子の準備
    genInfo->OperatorList["+"]=new OperatorInfo(Binary,Left,20);
    genInfo->OperatorList["-"]=new OperatorInfo(Binary,Left,20);
    genInfo->OperatorList["*"]=new OperatorInfo(Binary,Left,40);
    genInfo->OperatorList["/"]=new OperatorInfo(Binary,Left,40);
    genInfo->OperatorList[">>"]=new OperatorInfo(Binary,Left,10);
    genInfo->OperatorList["<<"]=new OperatorInfo(Binary,Left,10);
    genInfo->OperatorList["%"]=new OperatorInfo(Binary,Left,40);
    genInfo->OperatorList["&&"]=new OperatorInfo(Binary,Left,5);
    genInfo->OperatorList["||"]=new OperatorInfo(Binary,Left,5);
    genInfo->OperatorList["!"]=new OperatorInfo(Unary,Right,70);

	genInfo->OperatorList["-"]=new OperatorInfo(Unary,Right,70);

    genInfo->OperatorList["<"]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList[">"]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList["<="]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList[">="]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList["=="]=new OperatorInfo(Binary,Left,6);
    genInfo->OperatorList["!="]=new OperatorInfo(Binary,Left,6);

	genInfo->OperatorList["="]=new OperatorInfo(Binary,Left,2); //代入



    vector< pair<string,shared_ptr<TypeAST> > > arglist;

	RegisterBuiltinFunction("rand",rand_int,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("int"),false);

	RegisterBuiltinFunction("glut_mainloop",glut_mainloop,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_clear",glut_clear,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_flush",glut_flush,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_point",glut_begin_point,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_line",glut_begin_line,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_strip",glut_begin_strip,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_lineloop",glut_begin_lineloop,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_triangle",glut_begin_triangle,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_quad",glut_begin_quad,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_trianglefan",glut_begin_trianglefan,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_begin_polygon",glut_begin_polygon,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	RegisterBuiltinFunction("glut_end",glut_end,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

	vector<shared_ptr<TypeAST> > voidarg;voidarg.push_back(new BasicTypeAST("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",new FunctionTypeAST(voidarg)));
	RegisterBuiltinFunction("glut_setdisplayfunc",glut_setdispfunc,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	arglist.clear();

	vector<shared_ptr<TypeAST> > intarg;
	intarg.push_back(new BasicTypeAST("int"));
	intarg.push_back(new BasicTypeAST("int"));
	intarg.push_back(new BasicTypeAST("int"));
	intarg.push_back(new BasicTypeAST("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",new FunctionTypeAST(intarg)));
	RegisterBuiltinFunction("glut_setkeyboardfunc",glut_setkeyboardfunc,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	arglist.clear();

	intarg.clear();
	intarg.push_back(new BasicTypeAST("int"));
	intarg.push_back(new BasicTypeAST("int"));
	intarg.push_back(new BasicTypeAST("void"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",new FunctionTypeAST(intarg)));
	RegisterBuiltinFunction("glut_setmousefunc",glut_setmousefunc,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);
	arglist.clear();

    arglist.push_back(pair<string,shared_ptr<TypeAST> >("val",new BasicTypeAST("int")));
    RegisterBuiltinFunction("print",print_int,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",new BasicTypeAST("bool"));
    RegisterBuiltinFunction("print",print_bool,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("str",new BasicTypeAST("string"));
    RegisterBuiltinFunction("print",print_str,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

    RegisterBuiltinFunction("glut_openwindow",glut_openwindow,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("val",new BasicTypeAST("int"));
    RegisterBuiltinFunction("abs",abs_int,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("int"),true);

    arglist[0]=pair<string,shared_ptr<TypeAST> >("str",new BasicTypeAST("string"));
    RegisterBuiltinFunction("length",length_str,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("int"),true);

	arglist[0]=pair<string,shared_ptr<TypeAST> >("val1",new BasicTypeAST("int"));
	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val2",new BasicTypeAST("int")));
    RegisterBuiltinFunction("pow",pow_int,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("int"),true);

	RegisterBuiltinFunction("glut_vertex2i",glut_vertex2i,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

	arglist.push_back(pair<string,shared_ptr<TypeAST> >("val3",new BasicTypeAST("int")));
	RegisterBuiltinFunction("glut_color3i",glut_color3i,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

	RegisterBuiltinFunction("glut_char",glut_char,new vector< pair<string,shared_ptr<TypeAST> > >(arglist),new BasicTypeAST("void"),false);

	while(!parser->IsAccepted()){
		pair<Symbol,TokenValue> token=lexer->Get();
		parser->Put(lexer,genInfo,token);
	}

}

void Compiler::TypeCheck()
{
    vector<Environment> environment; //現在可視状態にある変数（トップレベルの関数も変数とみなす）のスタック（フレームを積み重ねていく）

    vector<shared_ptr<FunctionAST> >::iterator fun_iter;
    vector<shared_ptr<VariableDefStatementAST> >::iterator var_iter;

    Environment rootflame;
	rootflame.is_internalblock=false;

	rootflame.LocalVariablesPtr=&(genInfo->LocalVariables);

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
    environment.push_back(rootflame); //トップレベルのフレーム

    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		if((*fun_iter)->isBuiltin==false){
			(*fun_iter)->CheckType(&environment,genInfo,&(genInfo->LocalVariables));
		}
		rootflame.LocalVariablesPtr->push_back(pair<string,shared_ptr<TypeAST> >((*fun_iter)->Name,(*fun_iter)->TypeInfo));
    }
    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
        (*var_iter)->CheckType(&environment,genInfo,&(genInfo->LocalVariables));
        rootflame.LocalVariablesPtr->push_back(*((*var_iter)->Variable));
    }
}

void Compiler::RegisterChildClosure()
{
	//関数の子クロージャの登録を行う
	vector<shared_ptr<FunctionAST> >::iterator fun_iter;
	vector<shared_ptr<VariableDefStatementAST> >::iterator var_iter;
	vector<int> temp;

	for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
		temp=(*var_iter)->FindChildFunction();
		genInfo->ChildPoolIndex.insert(genInfo->ChildPoolIndex.end(),temp.begin(),temp.end());
    }

	for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		if((*fun_iter)->isBuiltin==false){
			temp=(*fun_iter)->FindChildFunction();
			genInfo->ChildPoolIndex.insert(genInfo->ChildPoolIndex.end(),temp.begin(),temp.end());
		}
    }
}


void Compiler::Codegen()
{
    vector<shared_ptr<FunctionAST> >::iterator iterf;
    vector<shared_ptr<VariableDefStatementAST> >::iterator iterv;

    for(iterf=genInfo->TopLevelFunction.begin();iterf!=genInfo->TopLevelFunction.end();iterf++){
		//組み込み関数のコード生成は行わない
        if((*iterf)->isBuiltin==false){
            (*iterf)->Codegen(NULL,genInfo);
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

	genInfo->Bootstrap.clear();
	//ブートストラップローダをつくる
	//グローバル変数の初期化
    for(iterv=genInfo->TopLevelVariableDef.begin();iterv!=genInfo->TopLevelVariableDef.end();iterv++){
		(*iterv)->Codegen(&(genInfo->Bootstrap),genInfo);
    }
    //main関数の呼び出し
    genInfo->Bootstrap.push_back(makeclosure);
	genInfo->Bootstrap.push_back(genInfo->MainFuncPoolIndex);
	genInfo->Bootstrap.push_back(invoke);
	genInfo->Bootstrap.push_back(ret);

}

void Compiler::CTFE(int loop/*繰り返し回数*/){
	vector<shared_ptr<ExprAST> > call_list;
	vector<Callshared_ptr<ExprAST>> exec_list;
	vector<shared_ptr<ExprAST> > temp;
	vector<shared_ptr<FunctionAST> >::iterator iterf;
	vector<shared_ptr<ExprAST> >::iterator citer;
	vector<Callshared_ptr<ExprAST>>::iterator iter;
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
            Callshared_ptr<ExprAST> target=dynamic_cast<Callshared_ptr<ExprAST>>(*citer);
			if(typeid(VariableExprAST)==typeid(*(target->callee))){
				if(dynamic_cast<Variableshared_ptr<ExprAST>>(target->callee)->is_toplevel_func && genInfo->TopLevelFunction[dynamic_cast<Variableshared_ptr<ExprAST>>(target->callee)->LocalIndex]->IsCTFEable(genInfo,-1)){
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
				(*iter)->CalculatedValue=0;
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
			preexec_code.push_back(dynamic_cast<Variableshared_ptr<ExprAST>>((*iter)->callee)->LocalIndex);
			preexec_code.push_back(invoke);
			preexec_code.push_back(ret);

			genInfo->Bootstrap=preexec_code;
			VM vm(genInfo);
			vm.Init();
			(*iter)->callee=NULL;
			(*iter)->CalculatedValue=vm.Run(false);

		}

		Codegen();
	}

	cout<<exec_count<<"箇所の関数呼び出しを事前実行しました"<<endl;

	return;
}
