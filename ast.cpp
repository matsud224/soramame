#include <vector>
#include <string>
#include "ast.h"
#include "vm.h"
#include "utility.h"
#include <iostream>
#include <stack>
#include <queue>
#include <typeinfo>
#include "color_text.h"

class CodegenInfo;


vector<int> FunctionAST::FindChildFunction()
{
	vector<int> list_tmp;

    ChildPoolIndex=new vector<int>();

	list_tmp=Body->FindChildFunction();
	ChildPoolIndex->insert(ChildPoolIndex->end(),list_tmp.begin(),list_tmp.end());

    //自身のインデックスを返す
    vector<int> result; result.push_back(PoolIndex);
    return result;
}

vector<int> IfStatementAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

	list_tmp=Condition->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

    vector<StatementAST *>::iterator iter;
	list_tmp=ThenBody->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
	list_tmp=ElseBody->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

    return result_list;
}

vector<int> ReturnStatementAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

	if(Expression){
		list_tmp=Expression->FindChildFunction();
	}
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	return result_list;
}

vector<int> VariableDefStatementAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

	if(InitialValue){
		list_tmp=InitialValue->FindChildFunction();
	}
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	return result_list;
}

vector<int> ExpressionStatementAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

	list_tmp=Expression->FindChildFunction();

	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	return result_list;
}

vector<int> UnBuiltExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;
	vector<ExprAST*>::iterator iter;

	for(iter=ExprList->begin();iter!=ExprList->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

	return result_list;
}

vector<int> CallExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;
	vector<ExprAST*>::iterator iter;

	for(iter=args->begin();iter!=args->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

	return result_list;
}



void VariableExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
	bytecodes->push_back(iloadlocal);
	bytecodes->push_back(LocalIndex);

    return;
}

void UnaryExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    Operand->Codegen(bytecodes,geninfo);
    if(Operator=="!"){
        if(TypeInfo->GetName()=="bool"){
            bytecodes->push_back(bnot);
        }
    }
    return;
}

void BinaryExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    //代入だけ特殊で、左辺を評価しない
    if(Operator=="="){
        RHS->Codegen(bytecodes,geninfo);

		bytecodes->push_back(istorelocal);
		bytecodes->push_back(dynamic_cast<VariableExprAST *>(LHS)->LocalIndex);

        return;
    }

    LHS->Codegen(bytecodes,geninfo);
    RHS->Codegen(bytecodes,geninfo);

    if(Operator=="+"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(iadd);
        }
    }else if(Operator=="-"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(isub);
        }
    }else if(Operator=="*"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(imul);
        }
    }else if(Operator=="/"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(idiv);
        }
    }else if(Operator=="<<"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(ilshift);
        }
    }else if(Operator==">>"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(irshift);
        }
    }else if(Operator=="%"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(imod);
        }
    }else if(Operator=="&&"){
        if(TypeInfo->GetName()=="bool"){
            bytecodes->push_back(band);
        }
    }else if(Operator=="||"){
        if(TypeInfo->GetName()=="bool"){
            bytecodes->push_back(bor);
        }
    }else if(Operator=="=="){
        if(TypeInfo->GetName()=="bool" || TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpeq);
        }
    }else if(Operator=="!="){
        if(TypeInfo->GetName()=="bool" || TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpne);
        }
    }else if(Operator=="<"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmplt);
        }
    }else if(Operator==">"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpgt);
        }
    }else if(Operator=="<="){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmple);
        }
    }else if(Operator==">="){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpge);
        }
    }
    return;
}

void CallExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    vector<ExprAST *>::reverse_iterator iter;
    for(iter=args->rbegin();iter!=args->rend();iter++){
        (*iter)->Codegen(bytecodes,geninfo);
    }
    if(PoolIndex!=-1){
		//クロージャ直呼び出し
		bytecodes->push_back(ipush);
        bytecodes->push_back(PoolIndex);
        bytecodes->push_back(invoke);
    }else{
        bytecodes->push_back(iloadlocal);
        bytecodes->push_back(LocalIndex);
        bytecodes->push_back(invoke);
    }
}

//自身が関数ならばbodyのコード生成を行う。
//自身がクロージャならそれに加えてスタックに自身のポインタを置くコードを書く
void FunctionAST::Codegen(vector<int> *bytecodes_given,CodegenInfo *geninfo)
{
    vector<int> codes;

    Body->Codegen(&codes,geninfo);

    //最後にreturnを挿入（return文が省略された時のため）
    //戻り値の型で場合分け
    TypeAST *rettype=dynamic_cast<FunctionTypeAST *>(TypeInfo)->TypeList.back();
    if(rettype->GetName()=="void"){
        codes.push_back(ret);
    }else{
        codes.push_back(iret);
    }

    bytecodes=codes;

    if(Name=="<closure>"){
        bytecodes_given->push_back(makeclosure);
        bytecodes_given->push_back(PoolIndex);
    }

    string bytecode_names[]={
		"ipush",
		"iadd",
		"isub",
		"imul",
		"idiv",
		"band","bor",
		"imod",
		"ineg","bnot",
		"ilshift","irshift",
		"invoke",
		"iloadlocal", //localという名前だが、結局はフレームを遡っていくのでグローバル変数に行き着くかもしれない
		"ret","iret",
		"istorelocal",
		"makeclosure",
		"skip",
		"iffalse_skip"
	};
    cout<<endl<<"関数:"<<Name<<"のコード"<<endl;
    for(unsigned int i=0;i<bytecodes.size();i++){
        cout<<bytecodes[i]<<" ("<< ((bytecodes[i]>=0 && bytecodes[i]<= 19)?bytecode_names[bytecodes[i]]:"undefined") <<")"<<endl;
    }
    cout<<endl;
    return;
}

void IntValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo){
    bytecodes->push_back(ipush);
    bytecodes->push_back(Value);
    return;
}


void BoolValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    if(Value){
        bytecodes->push_back(ipush);
        bytecodes->push_back(1);
    }else{
        bytecodes->push_back(ipush);
        bytecodes->push_back(0);
    }
}

void StringValExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
    bytecodes->push_back(ipush);
    bytecodes->push_back(PoolIndex);
}

void ReturnStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	if(Expression!=NULL){
		Expression->Codegen(bytecodes,geninfo);
		bytecodes->push_back(iret);
	}else{
        bytecodes->push_back(ret);
    }
}

ExprAST* UnBuiltExprAST::BuildAST(CodegenInfo* geninfo)
{
    //式の解析を操車場アルゴリズムで行う
    //一旦RPNにして、それからASTにする
    queue<ExprAST *> output;
    stack<OperatorAST *> operatorstack;
	int input_pos=0;

    while(input_pos < ExprList->size()){
        if(typeid(OperatorAST) == typeid(*(ExprList->at(input_pos)))){
            if(geninfo->OperatorList.count(dynamic_cast<OperatorAST*>(ExprList->at(input_pos))->Operator)==0){
                //未登録の演算子
                error("未定義の演算子です:"+dynamic_cast<OperatorAST*>(ExprList->at(input_pos))->Operator);
            }
            while(!operatorstack.empty()){
                string op1=dynamic_cast<OperatorAST*>(ExprList->at(input_pos))->Operator;
                string op2=operatorstack.top()->Operator;
                if((geninfo->OperatorList[op1]->GetAssociativity()==Left && geninfo->OperatorList[op1]->GetPrecedence()<=geninfo->OperatorList[op2]->GetPrecedence()) || (geninfo->OperatorList[op1]->GetPrecedence()<geninfo->OperatorList[op2]->GetPrecedence())){
                    output.push(operatorstack.top());
                    operatorstack.pop();
                }else{
                    break;
                }
            }
            operatorstack.push(dynamic_cast<OperatorAST*>(ExprList->at(input_pos)));

        }else{
        	//演算子以外のもの
        	if(ExprList->at(input_pos)->IsBuilt()){
				output.push(ExprList->at(input_pos));
        	}else{
        		//AST生成がまだなら再帰的にAST構築
        		output.push(dynamic_cast<UnBuiltExprAST*>(ExprList->at(input_pos))->BuildAST(geninfo));
        	}
        }

        input_pos++;
    }
    while(!operatorstack.empty()){
        output.push(operatorstack.top());
        operatorstack.pop();
    }

    //この時点でoutputにRPN形式で式が入っている
    stack<ExprAST *> calcstack; //スタックを使いRPNを展開していく
    while(!output.empty()){
        calcstack.push(output.front());
        output.pop();
        if(typeid(*(calcstack.top()))==typeid(OperatorAST)){
            OperatorAST *op=dynamic_cast<OperatorAST *>(calcstack.top()); calcstack.pop();
            ExprAST *operand1,*operand2;
            if(geninfo->OperatorList[op->Operator]->GetUnaryOrBinary()==Binary){
                operand2=calcstack.top(); calcstack.pop();
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(new BinaryExprAST(op->Operator,operand1,operand2)); //マージ
            }else{
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(new UnaryExprAST(op->Operator,operand1)); //マージ
            }
        }
    }

    if(calcstack.size()!=1){
        error("expressionに問題があります。");
    }

    return calcstack.top();
}

void IfStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	Condition->Codegen(bytecodes,geninfo);
    vector<int> *thencode=new vector<int>();
	ThenBody->Codegen(thencode,geninfo);

    vector<int> *elsecode=new vector<int>();
	ElseBody->Codegen(elsecode,geninfo);

    if(ElseBody->Body->size()==0){
		bytecodes->push_back(iffalse_skip);
		bytecodes->push_back(thencode->size());
		bytecodes->insert(bytecodes->end(),thencode->begin(),thencode->end());
    }else{
		thencode->push_back(skip);
		thencode->push_back(elsecode->size());

		bytecodes->push_back(iffalse_skip);
		bytecodes->push_back(thencode->size());
		bytecodes->insert(bytecodes->end(),thencode->begin(),thencode->end());
		bytecodes->insert(bytecodes->end(),elsecode->begin(),elsecode->end());
    }
}

void VariableDefStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	if(InitialValue!=NULL){
		InitialValue->Codegen(bytecodes,geninfo);
		bytecodes->push_back(istorelocal);
		bytecodes->push_back(LocalIndex);
	}
}

void ExpressionStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	Expression->Codegen(bytecodes,geninfo);
}


void VariableDefStatementAST::CheckType(vector< vector< pair<string, TypeAST* > >* >* env, CodegenInfo* geninfo)
{
	LocalIndex=-1;
	for(int i=0;i<(*env)[(static_cast<int>(env->size())-1)]->size();i++){
		LocalIndex++;
		if(Variable->first==(*(*env)[(static_cast<int>(env->size())-1)])[i].first){
			//名前が一致
			goto out_for;
		}
	}
	out_for:

	if(InitialValue!=NULL){
		if(InitialValue->IsBuilt()==false){
			InitialValue=dynamic_cast<UnBuiltExprAST*>(InitialValue)->BuildAST(geninfo);
		}
		InitialValue->CheckType(env,geninfo);
		if(InitialValue->TypeInfo->GetName() != Variable->second->GetName()){
			error("初期化できません。型が一致しません。");
		}
	}
	return;
}

TypeAST* VariableExprAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	//名前が一致するものがあるか上位のフレームに遡りながら探す
	bool found=false;
	int currentflame;
	unsigned int i;
	LocalIndex=-1;
	for(currentflame=(static_cast<int>(env->size())-1);currentflame>=0;currentflame--){
		for(i=0;i<(*env)[currentflame]->size();i++){
			LocalIndex++;
			if(Name==(*(*env)[currentflame])[i].first){
				//名前が一致
				TypeInfo=(*(*env)[currentflame])[i].second; //自身の型を決定
				found=true;
				goto out_for;
			}
		}
	}
	out_for:
	if(!found){
		error("変数:"+Name+"は未定義またはスコープ外です。");
	}
	//cout<<Name<<":"<<localindex<<endl;
	return TypeInfo;
}

TypeAST* UnaryExprAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	//循環防止の為
	if(TypeInfo!=NULL){
		return TypeInfo;
	}

	TypeAST *oprandt=Operand->CheckType(env,geninfo);
	if(Operator=="!"){
		if(oprandt->GetName()!="bool"){
			error("型に問題があります。単項演算子:"+Operator+" オペランド:"+oprandt->GetName());
		}
		TypeInfo=oprandt;
	}

	if(TypeInfo==NULL){
		error("未知の単項演算子です:"+Operator);
	}

	return TypeInfo;
}

TypeAST* BinaryExprAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	TypeAST *lhst=LHS->CheckType(env,geninfo);
	TypeAST *rhst=RHS->CheckType(env,geninfo);
	TypeInfo=NULL;

	//組み込み型の型チェック
	if(Operator=="+" || Operator=="-" || Operator=="*" || Operator=="/"){
		if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float")){
			error("型に問題があります。二項演算子:"+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}

		TypeInfo=lhst; //オペランドの型を元に自らの型を決める
	}else if(Operator=="%" || Operator=="<<" || Operator==">>"){
		if(*lhst!=*rhst || (lhst->GetName()!="int")){
			error("型に問題があります。二項演算子:"+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=lhst;
	}else if(Operator=="&&" || Operator=="||"){
		if(*lhst!=*rhst || (lhst->GetName()!="bool")){
			error("型に問題があります。二項演算子:"+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=lhst;
	}else if(Operator=="<" || Operator==">" || Operator=="<=" || Operator==">="){
		if(*lhst!=*rhst || (lhst->GetName()!="int")){
			error("型に問題があります。二項演算子:"+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=new BasicTypeAST("bool");
	}else if(Operator=="==" || Operator=="!="){
		if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="bool")){
			error("型に問題があります。二項演算子:"+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=new BasicTypeAST("bool");
	}else if(Operator=="="){
		if(*lhst!=*rhst){
			error("型に問題があります。二項演算子:"+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}else if(typeid(*LHS)!=typeid(VariableExprAST)){
			error("代入式の左辺が変数ではありません。");
		}
		TypeInfo=lhst;
	}

	if(TypeInfo==NULL){
		error("未知の二項演算子です:"+Operator);
	}

	return TypeInfo;
}

void ReturnStatementAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	if(Expression==NULL){return;}
	if(Expression->IsBuilt()==false){
		Expression=dynamic_cast<UnBuiltExprAST*>(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo);
}

void IfStatementAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	if(Condition->IsBuilt()==false){
		Condition=dynamic_cast<UnBuiltExprAST*>(Condition)->BuildAST(geninfo);
	}
	Condition->CheckType(env,geninfo);
	if(Condition->TypeInfo->GetName()!="bool"){
		error("条件式の型がboolではありません");
	}

	ThenBody->CheckType(env,geninfo);
	ElseBody->CheckType(env,geninfo);

	return;
}

void ExpressionStatementAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	if(Expression->IsBuilt()==false){
		Expression=dynamic_cast<UnBuiltExprAST*>(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo);
}

TypeAST* FunctionAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	//引数を環境に追加
	vector< pair<string,TypeAST *> >::iterator argiter;
	for(argiter=Args->begin();argiter!=Args->end();argiter++){
		Body->LocalVariables->push_back(*argiter);
	}

	Body->CheckType(env,geninfo);

	vector<StatementAST *>::iterator iter2;

	for(iter2=Body->Body->begin();iter2!=Body->Body->end();iter2++){
		if(typeid(*(*iter2))==typeid(ReturnStatementAST)){
			//return文の場合特別扱い。返す値の型とこの関数の返り値の型が不一致ならばエラー
			if(dynamic_cast<ReturnStatementAST *>(*iter2)->Expression==NULL){
				if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName() != "void"){
					error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
				}
			}else if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName() != dynamic_cast<ReturnStatementAST *>(*iter2)->Expression->TypeInfo->GetName()){
				error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
			}
		}
	}

	return TypeInfo; //自らの型を一応返しておく
}

TypeAST* CallExprAST::CheckType(vector< vector< pair<string,TypeAST *> > *> *env,CodegenInfo *geninfo){
	if(PoolIndex!=-1){
		//クロージャ直呼び出しのとき
		TypeAST* func_t=reinterpret_cast<FunctionAST *>(geninfo->PublicConstantPool.GetReference(PoolIndex))->CheckType(env,geninfo);
		this->TypeInfo=dynamic_cast<FunctionTypeAST*>(func_t)->TypeList.back();
	}else{
		//変数を介しての呼び出しのとき

		//引数の型を順に決めていく
		vector<ExprAST *>::iterator iter2;
		for(iter2=args->begin();iter2!=args->end();iter2++){
			if((*iter2)->IsBuilt()==false){
				(*iter2)=(dynamic_cast<UnBuiltExprAST*>(*iter2))->BuildAST(geninfo);
			}
			(*iter2)->CheckType(env,geninfo);
		}

		//名前が一致するものがあるか上位のフレームに遡りながら探す
		bool found=false;
		int currentflame;
		unsigned int i;
		LocalIndex=-1;
		for(currentflame=(static_cast<int>(env->size())-1);currentflame>=0;currentflame--){
			for(i=0;i<(*env)[currentflame]->size();i++){
				LocalIndex++;
				if(callee==(*(*env)[currentflame])[i].first && typeid(*((*(*env)[currentflame])[i].second))==typeid(FunctionTypeAST)){
					//名前が一致
					found=true;
					vector<TypeAST*> currentarg=dynamic_cast<FunctionTypeAST *>((*(*env)[currentflame])[i].second)->TypeList;
					if(args->size()+1==currentarg.size()){ //+1するのは、argsには戻り値の型が含まれていないから
						for(unsigned int j=0;j<args->size();j++){
							if(*(args->at(j)->TypeInfo) != *(currentarg[j])){
								goto nextfunc1;
							}
						}
						TypeInfo=currentarg.back(); //関数の型ではなく関数の戻り値の型を代入
						if(currentflame==0){
							//トップレベルに関数がある場合はビルトイン関数を疑う
							for(unsigned int k=0;k<geninfo->TopLevelFunction.size();k++){
								if(geninfo->TopLevelFunction[k]->Name==callee){
									isBuiltin=geninfo->TopLevelFunction[k]->isBuiltin;
								}
							}
						}
						return TypeInfo;
					}
				}

				nextfunc1:
				if(TypeInfo==NULL){}; //ラベルの後に式がないとエラーとなるため、無意味な式をひとつ
			}
		}

		if(found){
			error("関数定義と型が一致しません");
		}else{
			error(callee+"は未定義またはスコープ外です");
		}
	}

	return TypeInfo;
}

void BlockAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
    vector<int> codes;

    vector<StatementAST *>::iterator itere;
    for(itere=Body->begin();itere!=Body->end();itere++){
        (*itere)->Codegen(&codes,geninfo);
    }

    bytecodes->insert(bytecodes->end(),codes.begin(),codes.end());

    return;
}

void BlockAST::CheckType(vector< vector< pair<string, TypeAST* > >* >* env, CodegenInfo* geninfo)
{
	vector< pair<string,TypeAST *> > *fflame=new vector< pair<string,TypeAST *> >();
	//引数を環境に追加(Localvariablesに事前に登録してあった変数を登録（ブロックないの変数定義はこれから追加していく）)
	vector< pair<string,TypeAST *> >::iterator argiter;
	for(argiter=LocalVariables->begin();argiter!=LocalVariables->end();argiter++){
		fflame->push_back(*argiter);
	}
	env->push_back(fflame);
	vector<StatementAST *>::iterator iter2;

	LocalVariables=new vector< pair<string,TypeAST*> >();

	for(iter2=Body->begin();iter2!=Body->end();iter2++){
		if(typeid(*(*iter2))==typeid(VariableDefStatementAST)){
			env->back()->push_back(*(dynamic_cast<VariableDefStatementAST *>(*iter2)->Variable));
		}
	}

	for(iter2=Body->begin();iter2!=Body->end();iter2++){
		if(typeid(*(*iter2))==typeid(VariableDefStatementAST)){
			//変数宣言を見つけたら変数リストへ随時追加していく
			LocalVariables->push_back(*(dynamic_cast<VariableDefStatementAST *>(*iter2)->Variable));
		}

		(*iter2)->CheckType(env,geninfo);
	}

	delete env->back();
	env->pop_back(); //最後尾のフレームを削除
}

vector<int> BlockAST::FindChildFunction()
{
	vector<int> list_tmp;
	vector<int> result;
    vector<StatementAST *>::iterator iter;

    for(iter=Body->begin();iter!=Body->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result.insert(result.end(),list_tmp.begin(),list_tmp.end());
    }

    return result;
}
