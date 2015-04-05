#include "compiler.h"
#include "ast.h"

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

	genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"rand",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int")));

    arglist.push_back(pair<string,TypeAST *>("val",new BasicTypeAST("int")));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"printint",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("void")));

    arglist[0]=pair<string,TypeAST *>("val",new BasicTypeAST("bool"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"printbool",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("void")));

    arglist[0]=pair<string,TypeAST *>("str",new BasicTypeAST("string"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"print",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("void")));

    arglist[0]=pair<string,TypeAST *>("val",new BasicTypeAST("int"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"abs",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int")));

    arglist[0]=pair<string,TypeAST *>("str",new BasicTypeAST("string"));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"strlen",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int")));

	arglist[0]=pair<string,TypeAST *>("val1",new BasicTypeAST("int"));
	arglist.push_back(pair<string,TypeAST *>("val2",new BasicTypeAST("int")));
    genInfo->TopLevelFunction.push_back(new FunctionAST(genInfo,"pow",new vector< pair<string,TypeAST *> >(arglist),new BasicTypeAST("int")));


	while(!parser->IsAccepted()){
		pair<Symbol,TokenValue> token=lexer->Get();
		//cout<< CYAN "取得したトークン：" <<Parser::Symbol2Str(token.first)<<RESET<<endl;
		parser->Put(lexer,genInfo,token);
	}

}

void Compiler::TypeCheck()
{
    vector< vector< pair<string,TypeAST *> > *> environment; //現在可視状態にある変数（トップレベルの関数も変数とみなす）のスタック（フレームを積み重ねていく）

    vector<FunctionAST *>::iterator fun_iter;
    vector<VariableDefStatementAST *>::iterator var_iter;

    vector< pair<string,TypeAST *> > *rootflame=new vector< pair<string,TypeAST *> >();

    //型検査前に、トップレベルのシンボルをすべて登録しておく
    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
		rootflame->push_back(pair<string,TypeAST *>((*var_iter)->Variable->first,(*var_iter)->Variable->second));
    }
    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		rootflame->push_back(pair<string,TypeAST *>((*fun_iter)->Name,(*fun_iter)->TypeInfo));
    }
    environment.push_back(rootflame); //トップレベルのフレーム

    for(var_iter=genInfo->TopLevelVariableDef.begin();var_iter!=genInfo->TopLevelVariableDef.end();var_iter++){
        (*var_iter)->CheckType(&environment,genInfo);
    }
    for(fun_iter=genInfo->TopLevelFunction.begin();fun_iter!=genInfo->TopLevelFunction.end();fun_iter++){
		if((*fun_iter)->isBuiltin==false){
			(*fun_iter)->CheckType(&environment,genInfo);
		}
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
        if((*iterf)->Name=="main" && (*iterf)->TypeInfo->GetName()=="()=>void"){
            main_index=(*iterf)->PoolIndex;
        }
    }
    if(main_index==-1){
        error("適切なmain関数が見つかりません。");
    }
    genInfo->MainFuncPoolIndex=main_index;


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
