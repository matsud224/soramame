#include "compiler.h"
#include "ast.h"
#include <typeinfo>

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

    genInfo->OperatorList["<"]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList[">"]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList["<="]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList[">="]=new OperatorInfo(Binary,Left,8);
    genInfo->OperatorList["=="]=new OperatorInfo(Binary,Left,6);
    genInfo->OperatorList["!="]=new OperatorInfo(Binary,Left,6);

	genInfo->OperatorList["="]=new OperatorInfo(Binary,Left,2); //代入



    vector< pair<string,TypeAST *> > arglist;

	genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"rand",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int"),false));

    arglist.push_back(pair<string,TypeAST *>("val",new BasicTypeAST("int")));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"print",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("void"),false));

    arglist[0]=pair<string,TypeAST *>("val",new BasicTypeAST("bool"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"print",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("void"),false));

    arglist[0]=pair<string,TypeAST *>("str",new BasicTypeAST("string"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"print",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("void"),false));

    arglist[0]=pair<string,TypeAST *>("val",new BasicTypeAST("int"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"abs",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int"),true));

    arglist[0]=pair<string,TypeAST *>("str",new BasicTypeAST("string"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"strlen",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int"),true));

	arglist[0]=pair<string,TypeAST *>("val1",new BasicTypeAST("int"));
	arglist.push_back(pair<string,TypeAST *>("val2",new BasicTypeAST("int")));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"pow",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int"),true));

	arglist[0]=pair<string,TypeAST *>("list",new ListTypeAST(new BasicTypeAST("int")));
	arglist[1]=pair<string,TypeAST *>("index",new BasicTypeAST("int"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"get_intlist",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int"),true));

	arglist[0]=pair<string,TypeAST *>("list",new ListTypeAST(new BasicTypeAST("bool")));
	arglist[1]=pair<string,TypeAST *>("index",new BasicTypeAST("int"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"get_boollist",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("bool"),true));

	vector<TypeAST*> fargs;
	fargs.push_back(new BasicTypeAST("void"));
	arglist[0]=pair<string,TypeAST *>("list",new ListTypeAST(new FunctionTypeAST(fargs)));
	arglist[1]=pair<string,TypeAST *>("index",new BasicTypeAST("int"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"get_funlist",new vector< pair<string,TypeAST *> >(arglist),new FunctionTypeAST(fargs),true));


	while(!parser->IsAccepted()){
		pair<Symbol,TokenValue> token=lexer->Get();
		//cout<< CYAN "取得したトークン：" <<Parser::Symbol2Str(token.first)<<RESET<<endl;
		parser->Put(lexer,genInfo,token);
	}

}

void Compiler::TypeCheck()
{
    vector<Environment> environment; //現在可視状態にある変数（トップレベルの関数も変数とみなす）のスタック（フレームを積み重ねていく）

    vector<FunctionAST *>::iterator fun_iter;
    vector<VariableDefStatementAST *>::iterator var_iter;

    Environment rootflame;
	rootflame.is_internalblock=false;

	rootflame.LocalVariablesPtr=&(genInfo->LocalVariables);

	int cnt=0;
    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		rootflame.Items.push_back({pair<string,TypeAST *>((*fun_iter)->Name,(*fun_iter)->TypeInfo),cnt});
		cnt++;
    }
    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
        rootflame.Items.push_back({*((*var_iter)->Variable),cnt});
        cnt++;
    }
    environment.push_back(rootflame); //トップレベルのフレーム

    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		if((*fun_iter)->isBuiltin==false){
			(*fun_iter)->CheckType(&environment,genInfo,&(genInfo->LocalVariables));
		}
		rootflame.LocalVariablesPtr->push_back(pair<string,TypeAST*>((*fun_iter)->Name,(*fun_iter)->TypeInfo));
    }
    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
        (*var_iter)->CheckType(&environment,genInfo,&(genInfo->LocalVariables));
        rootflame.LocalVariablesPtr->push_back(*((*var_iter)->Variable));
    }
}

void Compiler::RegisterChildClosure()
{
	//関数の子クロージャの登録を行う
	vector<FunctionAST*>::iterator fun_iter;
	vector<VariableDefStatementAST*>::iterator var_iter;
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
    vector<FunctionAST *>::iterator iterf;
    vector<VariableDefStatementAST *>::iterator iterv;

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
	vector<ExprAST*> call_list;
	vector<CallExprAST*> exec_list;
	vector<ExprAST*> temp;
	vector<FunctionAST*>::iterator iterf;
	vector<ExprAST*>::iterator citer;
	vector<CallExprAST*>::iterator iter;
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
            CallExprAST* target=dynamic_cast<CallExprAST*>(*citer);
			if(typeid(VariableExprAST)==typeid(*(target->callee))){
				if(dynamic_cast<VariableExprAST*>(target->callee)->is_toplevel_func && genInfo->TopLevelFunction[dynamic_cast<VariableExprAST*>(target->callee)->LocalIndex]->IsCTFEable(genInfo,-1)){
					vector<ExprAST*>::iterator aiter;
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
			for(vector<ExprAST*>::reverse_iterator eiter=(*iter)->args->rbegin();eiter!=(*iter)->args->rend();eiter++){
				preexec_code.push_back(ipush);
				preexec_code.push_back((*eiter)->GetVMValue(genInfo));
			}

			preexec_code.push_back(iloadlocal);
			preexec_code.push_back(0); //FlameBack
			preexec_code.push_back(dynamic_cast<VariableExprAST*>((*iter)->callee)->LocalIndex);
			preexec_code.push_back(invoke);
			preexec_code.push_back(ret);

			genInfo->Bootstrap=preexec_code;
			VM vm(genInfo);
			(*iter)->callee=NULL;
			(*iter)->CalculatedValue=vm.Run();

		}

		Codegen();
	}

	cout<<exec_count<<"箇所の関数呼び出しを事前実行しました"<<endl;

	return;
}
