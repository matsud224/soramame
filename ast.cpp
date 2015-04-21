#include <vector>
#include <string>
#include "ast.h"
#include "vm.h"
#include "utility.h"
#include <algorithm>
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

vector<int> WhileStatementAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

	list_tmp=Condition->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

    vector<StatementAST *>::iterator iter;
	list_tmp=Body->FindChildFunction();
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

	list_tmp=callee->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	for(iter=args->begin();iter!=args->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

	return result_list;
}

vector<int> ListRefExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;
	vector<ExprAST*>::iterator iter;

	list_tmp=target->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	list_tmp=IndexExpression->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	return result_list;
}

vector<int> DataMemberRefExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;
	vector<ExprAST*>::iterator iter;

	list_tmp=target->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	return result_list;
}


void VariableExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
	bytecodes->push_back(iloadlocal);
	bytecodes->push_back(FlameBack);
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
		if(typeid(*LHS)==typeid(ListRefExprAST)){
			RHS->Codegen(bytecodes,geninfo);
			ListRefExprAST* lhs_cast=dynamic_cast<ListRefExprAST*>(LHS);
			//一旦スタックにロードして、それに対して代入
			bytecodes->push_back(iloadlocal);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->FlameBack);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->LocalIndex);
			lhs_cast->IndexExpression->Codegen(bytecodes,geninfo);
			bytecodes->push_back(istorebyindex);
		}else if(typeid(*LHS)==typeid(DataMemberRefExprAST)){
			RHS->Codegen(bytecodes,geninfo);
			DataMemberRefExprAST* lhs_cast=dynamic_cast<DataMemberRefExprAST*>(LHS);
			//一旦スタックにロードして、それに対して代入
			bytecodes->push_back(iloadlocal);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->FlameBack);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->LocalIndex);
			bytecodes->push_back(istorefield);
			bytecodes->push_back((new StringValExprAST(geninfo,lhs_cast->MemberName))->PoolIndex);
		}else{
			RHS->Codegen(bytecodes,geninfo);

			bytecodes->push_back(istorelocal);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(LHS)->FlameBack);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(LHS)->LocalIndex);
		}

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
        if(LHS->TypeInfo->GetName()=="bool" || LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpeq);
        }
    }else if(Operator=="!="){
        if(LHS->TypeInfo->GetName()=="bool" || LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpne);
        }
    }else if(Operator=="<"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmplt);
        }
    }else if(Operator==">"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpgt);
        }
    }else if(Operator=="<="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmple);
        }
    }else if(Operator==">="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpge);
        }
    }
    return;
}

void CallExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
	if(callee==NULL){
		//計算済み
		bytecodes->push_back(ipush);
		bytecodes->push_back(CalculatedValue);
		return;
	}
    vector<ExprAST *>::reverse_iterator iter;
    for(iter=args->rbegin();iter!=args->rend();iter++){
        (*iter)->Codegen(bytecodes,geninfo);
    }
    callee->Codegen(bytecodes,geninfo);
    bytecodes->push_back(invoke);
}

void ListRefExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
	target->Codegen(bytecodes,geninfo);
	IndexExpression->Codegen(bytecodes,geninfo);
	bytecodes->push_back(iloadbyindex);
}

void DataMemberRefExprAST::Codegen(vector<int>* bytecodes,CodegenInfo *geninfo)
{
	target->Codegen(bytecodes,geninfo);
	bytecodes->push_back(iloadbyname);
	bytecodes->push_back((new StringValExprAST(geninfo,MemberName))->PoolIndex);
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
    	//非voidな関数でreturnされないときは-1を返す
    	codes.push_back(ipush);
    	codes.push_back(-1);
        codes.push_back(iret);
    }

    bytecodes=codes;

    if(Name=="<anonymous>"){
        bytecodes_given->push_back(makeclosure);
        bytecodes_given->push_back(PoolIndex);
    }
	/*
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
		"iffalse_skip",
		"back",
		"makelist"
	};
    cout<<endl<<"関数:"<<Name<<"のコード"<<endl;
    for(unsigned int i=0;i<bytecodes.size();i++){
        cout<<bytecodes[i]<<" ("<< ((bytecodes[i]>=0 && bytecodes[i]<= 21)?bytecode_names[bytecodes[i]]:"undefined") <<")"<<endl;
    }
    cout<<endl;*/
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
                //cout <<(geninfo->OperatorList[op1]->GetAssociativity()==Left)<<endl<<(geninfo->OperatorList[op1]->GetPrecedence())<<(geninfo->OperatorList[op2]->GetPrecedence())<<endl;
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

void WhileStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	vector<int> *condcode=new vector<int>();
	Condition->Codegen(condcode,geninfo);
    vector<int> *bodycode=new vector<int>();
	Body->Codegen(bodycode,geninfo);

	condcode->push_back(iffalse_skip);
	condcode->push_back(bodycode->size()+2);
	bodycode->push_back(back);
	bodycode->push_back(bodycode->size()+2+condcode->size()-1);

	bytecodes->insert(bytecodes->end(),condcode->begin(),condcode->end());
	bytecodes->insert(bytecodes->end(),bodycode->begin(),bodycode->end());
}


void VariableDefStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	if(InitialValue!=NULL){
		InitialValue->Codegen(bytecodes,geninfo);
		bytecodes->push_back(istorelocal);
		bytecodes->push_back(FlameBack);
		bytecodes->push_back(LocalIndex);
	}
}

void ExpressionStatementAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	Expression->Codegen(bytecodes,geninfo);
}


void VariableDefStatementAST::CheckType(vector<Environment> *env, CodegenInfo* geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars)
{
	FlameBack=0;
	LocalIndex=CurrentLocalVars->size();
	CurrentLocalVars->push_back(*Variable);
	env->back().Items.push_back({*Variable,LocalIndex});

	if(InitialValue!=NULL){
		if(InitialValue->IsBuilt()==false){
			InitialValue=dynamic_cast<UnBuiltExprAST*>(InitialValue)->BuildAST(geninfo);
		}
		InitialValue->CheckType(env,geninfo,CurrentLocalVars);
		if(InitialValue->TypeInfo==NULL){
			//オーバーロードの解決ができなかった
			if(Variable->second==NULL){
				//型指定がなかった場合にはオーバーロード解決のヒントがないため、エラー
				error("オーバーロードを解決できません：変数宣言に型を付与することでオーバーロードを指定できます");
			}
			//オーバーロードのヒントを与える
			InitialValue->TypeInfo=Variable->second;
			InitialValue->CheckType(env,geninfo,CurrentLocalVars);
		}
		if(Variable->second==NULL){
			//型が未指定だったとき
			Variable->second=InitialValue->TypeInfo;
			CurrentLocalVars->back()=*Variable;
			env->back().Items.back()={*Variable,LocalIndex};
		}else if(InitialValue->TypeInfo->GetName() != Variable->second->GetName()){
			error("初期化できません。型が一致しません。");
		}
	}
	return;
}

TypeAST* VariableExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	//変数を介しての呼び出しのとき

	//名前が一致するものがあるか上位のフレームに遡りながら探す
	int currentenv;
	int i;
	FlameBack=0;
	int candidate_count=0;

	if(TypeInfo==NULL){
		//オーバーロードの数を調べる(トップレベルの関数について)
		vector<FunctionAST*>::iterator iter;
		for(iter=geninfo->TopLevelFunction.begin();iter!=geninfo->TopLevelFunction.end();iter++){
			if(Name==(*iter)->Name){
				//名前が一致
				candidate_count++;
			}
		}
		if(candidate_count>1){
			//オーバーロードを解決できなかった...(型情報を与えてもらって、再度呼んでもらう)
			TypeInfo=NULL;
			return NULL;
		}
	}

	for(currentenv=(static_cast<int>(env->size())-1);currentenv>=0;currentenv--){
		for(i=(*env)[currentenv].Items.size()-1;i>=0;i--){
			if(Name==(*env)[currentenv].Items[i].VariableInfo.first){
				//名前が一致

				if(TypeInfo!=NULL){
					//オーバーロード・指定された型との一致を調べる(返り値については調べない)
					if(typeid(FunctionTypeAST)==typeid(*TypeInfo)){
						if(typeid(*((*env)[currentenv].Items[i].VariableInfo.second))!=typeid(FunctionTypeAST)){
							error("定義重複：関数ではありません");
						}
						vector<TypeAST*> typelist=dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList;
						vector<TypeAST*> typelist2=dynamic_cast<FunctionTypeAST*>((*env)[currentenv].Items[i].VariableInfo.second)->TypeList;
						if(typelist.size()!=typelist2.size()){continue;}
						bool fail=false;
						for(int i=0;i<typelist.size()-1;i++){
							if(typelist[i]->GetName()!=typelist2[i]->GetName()){
								fail=true;
								break;
							}
						}
						if(fail){
							continue;
						}
					}
				}


				TypeInfo=(*env)[currentenv].Items[i].VariableInfo.second;

				/*if((*env)[currentenv].Items[i].LocalIndex==-1){
					(*env)[currentenv].Items[i].LocalIndex=CurrentLocalVars->size();
					CurrentLocalVars->push_back((*env)[currentenv].Items[i].VariableInfo);
				}*/

				LocalIndex=(*env)[currentenv].Items[i].LocalIndex;

				if(currentenv==0 && geninfo->TopLevelFunction.size()>LocalIndex){
					is_toplevel_func=true;
				}else{
					is_toplevel_func=false;
				}

				return TypeInfo;
			}
		}
		if((*env)[currentenv].is_internalblock==false){
			FlameBack++;
		}
	}

	error(Name+"は未定義またはスコープ外または型の不一致です");

	return TypeInfo;
}

TypeAST* UnaryExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	//循環防止の為
	if(TypeInfo!=NULL){
		return TypeInfo;
	}

	TypeAST *oprandt=Operand->CheckType(env,geninfo,CurrentLocalVars);
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

TypeAST* BinaryExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	TypeAST *lhst=LHS->CheckType(env,geninfo,CurrentLocalVars);
	TypeAST *rhst=RHS->CheckType(env,geninfo,CurrentLocalVars);
	if(rhst==NULL){
		if(Operator=="="){
			//オーバーロードの解決に失敗
			RHS->TypeInfo=LHS->TypeInfo;
			rhst=RHS->CheckType(env,geninfo,CurrentLocalVars);
		}else{
			error("オーバーロードの解決ができません");
		}
	}

	TypeInfo=NULL;

	//組み込み型の型チェック
	if(Operator=="+" || Operator=="-" || Operator=="*" || Operator=="/"){
		if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="float")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}

		TypeInfo=lhst; //オペランドの型を元に自らの型を決める
	}else if(Operator=="%" || Operator=="<<" || Operator==">>"){
		if(*lhst!=*rhst || (lhst->GetName()!="int")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=lhst;
	}else if(Operator=="&&" || Operator=="||"){
		if(*lhst!=*rhst || (lhst->GetName()!="bool")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=lhst;
	}else if(Operator=="<" || Operator==">" || Operator=="<=" || Operator==">="){
		if(*lhst!=*rhst || (lhst->GetName()!="int")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=new BasicTypeAST("bool");
	}else if(Operator=="==" || Operator=="!="){
		if(*lhst!=*rhst || (lhst->GetName()!="int" && lhst->GetName()!="bool")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=new BasicTypeAST("bool");
	}else if(Operator=="="){
		if(*lhst!=*rhst){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}else if(typeid(*LHS)!=typeid(VariableExprAST) && typeid(*LHS)!=typeid(ListRefExprAST) && typeid(*LHS)!=typeid(DataMemberRefExprAST)){
			error("代入式の左辺が変数ではありません。");
		}
		TypeInfo=lhst;
	}

	if(TypeInfo==NULL){
		error("未知の二項演算子です:"+Operator);
	}

	return TypeInfo;
}

void ReturnStatementAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	if(Expression==NULL){return;}
	if(Expression->IsBuilt()==false){
		Expression=dynamic_cast<UnBuiltExprAST*>(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo,CurrentLocalVars);
}

void IfStatementAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	if(Condition->IsBuilt()==false){
		Condition=dynamic_cast<UnBuiltExprAST*>(Condition)->BuildAST(geninfo);
	}
	Condition->CheckType(env,geninfo,CurrentLocalVars);
	if(Condition->TypeInfo->GetName()!="bool"){
		error("条件式の型がboolではありません");
	}

	ThenBody->CheckType(env,geninfo,false,CurrentLocalVars);
	ElseBody->CheckType(env,geninfo,false,CurrentLocalVars);

	return;
}

void WhileStatementAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	if(Condition->IsBuilt()==false){
		Condition=dynamic_cast<UnBuiltExprAST*>(Condition)->BuildAST(geninfo);
	}
	Condition->CheckType(env,geninfo,CurrentLocalVars);
	if(Condition->TypeInfo->GetName()!="bool"){
		error("条件式の型がboolではありません");
	}

	Body->CheckType(env,geninfo,false,CurrentLocalVars);

	return;
}


void ExpressionStatementAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	if(Expression->IsBuilt()==false){
		Expression=dynamic_cast<UnBuiltExprAST*>(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo,CurrentLocalVars);
}

TypeAST* FunctionAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	LocalVariables=new vector< pair<string,TypeAST*> >();

	vector< pair<string,TypeAST *> >::iterator argiter;
	for(argiter=Args->begin();argiter!=Args->end();argiter++){
		LocalVariables->push_back(*argiter);
	}

	Body->CheckType(env,geninfo,true,LocalVariables);

	vector<StatementAST *>::iterator iter2;

	for(iter2=Body->Body->begin();iter2!=Body->Body->end();iter2++){
		if(typeid(*(*iter2))==typeid(ReturnStatementAST)){
			//return文の場合特別扱い。返す値の型とこの関数の返り値の型が不一致ならばエラー
			if(dynamic_cast<ReturnStatementAST *>(*iter2)->Expression==NULL){
				if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName()=="!!undefined!!"){
					//推論が必要
					dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.pop_back();
					dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.push_back(new BasicTypeAST("void"));
				}else if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName() != "void"){
					error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
				}
			}else if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName()=="!!undefined!!"){
				//推論が必要
				dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.pop_back();
				dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.push_back(dynamic_cast<ReturnStatementAST *>(*iter2)->Expression->TypeInfo);
			}else if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName() != dynamic_cast<ReturnStatementAST *>(*iter2)->Expression->TypeInfo->GetName()){
				error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
			}
		}
	}

	if(dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.back()->GetName()=="!!undefined!!"){
		dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.pop_back();
		dynamic_cast<FunctionTypeAST*>(TypeInfo)->TypeList.push_back(new BasicTypeAST("void"));
	}

	return TypeInfo; //自らの型を一応返しておく
}

TypeAST* CallExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	//引数の型を順に決めていく
	vector<TypeAST*> argtype;
	vector<ExprAST *>::iterator iter2;
	for(iter2=args->begin();iter2!=args->end();iter2++){
		if((*iter2)->IsBuilt()==false){
			(*iter2)=(dynamic_cast<UnBuiltExprAST*>(*iter2))->BuildAST(geninfo);
		}
		(*iter2)->CheckType(env,geninfo,CurrentLocalVars);
		argtype.push_back((*iter2)->TypeInfo);
	}

	argtype.push_back(new BasicTypeAST("void"));


	if(callee->IsBuilt()==false){
		callee=dynamic_cast<UnBuiltExprAST*>(callee)->BuildAST(geninfo);
	}
	callee->TypeInfo=new FunctionTypeAST(argtype);
	callee->CheckType(env,geninfo,CurrentLocalVars);

	if(callee->TypeInfo==NULL){
		error("再帰呼び出しをするには型情報を記述してください");
	}

	if(typeid(FunctionTypeAST) != typeid(*(callee->TypeInfo))){
		error("calleeが関数ではありません");
	}



	vector<TypeAST*> currentarg=dynamic_cast<FunctionTypeAST *>(callee->TypeInfo)->TypeList;
	if(args->size()+1==currentarg.size()){ //+1するのは、argsには戻り値の型が含まれていないから
		for(unsigned int j=0;j<args->size();j++){
			if(*(args->at(j)->TypeInfo) != *(currentarg[j])){
				goto type_error;
			}
		}
		TypeInfo=currentarg.back(); //関数の型ではなく関数の戻り値の型を代入

		return TypeInfo;
	}
	type_error:
	error("引数リストの型とcalleeの引数の型が一致しません");
}

TypeAST* ListRefExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	if(IndexExpression->IsBuilt()==false){
		IndexExpression=dynamic_cast<UnBuiltExprAST*>(IndexExpression)->BuildAST(geninfo);
	}
	IndexExpression->CheckType(env,geninfo,CurrentLocalVars);
	if(IndexExpression->TypeInfo->GetName()!="int"){
		error("添字は整数で指定してください");
	}

	if(target->IsBuilt()==false){
		target=dynamic_cast<UnBuiltExprAST*>(target)->BuildAST(geninfo);
	}
	target->CheckType(env,geninfo,CurrentLocalVars);

	if(typeid(ListTypeAST) != typeid(*(target->TypeInfo)) && typeid(TupleTypeAST) != typeid(*(target->TypeInfo))){
		error("添字を指定できるのはリストまたはタプルです");
	}

	if(typeid(ListTypeAST) == typeid(*(target->TypeInfo))){
		TypeInfo=dynamic_cast<ListTypeAST*>(target->TypeInfo)->ContainType;
	}else if(typeid(TupleTypeAST) == typeid(*(target->TypeInfo))){
		if(typeid(*IndexExpression) != typeid(IntValExprAST)){
			error("タプルの添字は整数定数を指定してください");
		}
		if(IndexExpression->GetVMValue(geninfo)<0 || IndexExpression->GetVMValue(geninfo)>=dynamic_cast<TupleTypeAST*>(target->TypeInfo)->ContainTypeList.size()){
			error("タプルの範囲外です");
		}
		TypeInfo=dynamic_cast<TupleTypeAST*>(target->TypeInfo)->ContainTypeList[IndexExpression->GetVMValue(geninfo)];
	}

	return TypeInfo;
}

TypeAST* DataMemberRefExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars){
	if(target->IsBuilt()==false){
		target=dynamic_cast<UnBuiltExprAST*>(target)->BuildAST(geninfo);
	}
	target->CheckType(env,geninfo,CurrentLocalVars);

	vector< pair<string,TypeAST*> >::iterator miter;
	vector<DataDefAST*>::iterator diter;
	for(diter=geninfo->TopLevelDataDef.begin();diter!=geninfo->TopLevelDataDef.end();diter++){
		if((*diter)->Name==target->TypeInfo->GetName()){
			break;
		}
	}
	if(diter!=geninfo->TopLevelDataDef.end()){
		for(miter=(*diter)->MemberList.begin();miter!=(*diter)->MemberList.end();miter++){
			if(miter->first==MemberName){
				break;
			}
		}
		if(miter==(*diter)->MemberList.end()){
			error("メンバ"+MemberName+"はありません");
		}else{
			TypeInfo=miter->second;
			return TypeInfo;
		}
	}

	vector<GroupDefAST*>::iterator giter;
	for(giter=geninfo->TopLevelGroupDef.begin();giter!=geninfo->TopLevelGroupDef.end();giter++){
		if((*giter)->Name==target->TypeInfo->GetName()){
			break;
		}
	}
	if(giter!=geninfo->TopLevelGroupDef.end()){
		for(miter=(*giter)->MemberList.begin();miter!=(*giter)->MemberList.end();miter++){
			if(miter->first==MemberName){
				break;
			}
		}
		if(miter==(*giter)->MemberList.end()){
			error("メンバ"+MemberName+"はありません");
		}else{
			TypeInfo=miter->second;
			return TypeInfo;
		}
	}
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

void BlockAST::CheckType(vector<Environment> *env, CodegenInfo* geninfo,bool addargs,vector< pair<string,TypeAST*> > *CurrentLocalVars/*現在の関数のローカル変数領域へのポインタ*/)
{
	Environment newflame;
	newflame.LocalVariablesPtr=CurrentLocalVars;

	if(addargs){
		//引数を追加する場合、つまり関数ブロックである場合（if,whileでは呼ばれない）
		newflame.is_internalblock=false;
		for(int i=0;i<CurrentLocalVars->size();i++){
			newflame.Items.push_back({CurrentLocalVars->at(i),i});
		}
	}else{
		newflame.is_internalblock=true;
	}

	env->push_back(newflame);

	vector<StatementAST *>::iterator iter2;
	for(iter2=Body->begin();iter2!=Body->end();iter2++){
		(*iter2)->CheckType(env,geninfo,CurrentLocalVars);
	}

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

void ListValExprAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	/*if(PoolIndex!=-1){
		bytecodes->push_back(ipush);
		bytecodes->push_back(PoolIndex);
		//定数としてリストが生成されたが，例えばクロージャなら内部のコード生成が必要
		//でも、bytecodesに余計なコードが入るといけないので新たなベクタにコードを吐かせてそれは捨てる
		vector<int> dummy;
		list<ExprAST*>::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			(*iter)->Codegen(&dummy,geninfo);
		}
	}else{*/
		list<ExprAST*>::reverse_iterator riter;
		for(riter=Value->rbegin();riter!=Value->rend();riter++){
			(*riter)->Codegen(bytecodes,geninfo);
		}
		bytecodes->push_back(makelist);
		bytecodes->push_back(Value->size());
	//}
}

void TupleValExprAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	/*if(PoolIndex!=-1){
		bytecodes->push_back(ipush);
		bytecodes->push_back(PoolIndex);
		//定数としてリストが生成されたが，例えばクロージャなら内部のコード生成が必要
		//でも、bytecodesに余計なコードが入るといけないので新たなベクタにコードを吐かせてそれは捨てる
		vector<int> dummy;
		list<ExprAST*>::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			(*iter)->Codegen(&dummy,geninfo);
		}
	}else{*/
		list<ExprAST*>::reverse_iterator riter;
		for(riter=Value->rbegin();riter!=Value->rend();riter++){
			(*riter)->Codegen(bytecodes,geninfo);
		}
		bytecodes->push_back(makelist);
		bytecodes->push_back(Value->size());
	//}
}

void DataValExprAST::Codegen(vector<int>* bytecodes, CodegenInfo* geninfo)
{
	vector< pair<string,ExprAST*> >::reverse_iterator riter;
	for(riter=InitValue->rbegin();riter!=InitValue->rend();riter++){
		(*riter).second->Codegen(bytecodes,geninfo);
		bytecodes->push_back(ipush);
		bytecodes->push_back((new StringValExprAST(geninfo,(*riter).first))->PoolIndex);
	}
	bytecodes->push_back(makedata);
	bytecodes->push_back((new StringValExprAST(geninfo,TypeInfo->GetName()))->PoolIndex);
	bytecodes->push_back(InitValue->size());
}

vector<int> ListValExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

    list<ExprAST *>::iterator iter;
    for(iter=Value->begin();iter!=Value->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

TypeAST* ListValExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars)
{
	list<ExprAST*>::iterator iter;
    for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsBuilt()==false){
			(*iter)=(dynamic_cast<UnBuiltExprAST*>(*iter))->BuildAST(geninfo);
		}
		(*iter)->CheckType(env,geninfo,CurrentLocalVars);
		if(TypeInfo==NULL){
			TypeInfo=(*iter)->TypeInfo;
		}else{
			if(TypeInfo->GetName()!=(*iter)->TypeInfo->GetName()){
				error("リストの要素の型がバラバラです。");
			}
		}
    }
	TypeInfo=new ListTypeAST(TypeInfo); //要素の型のリスト

	/*
	if(IsConstant()){
		list<int> *const_list=new list<int>();
		list<ExprAST*>::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			const_list->push_back((*iter)->GetVMValue(geninfo));
		}
		PoolIndex=geninfo->PublicConstantPool.SetReference(const_list);
		//cout<<"#"<<PoolIndex<<" : <list>"<<TypeInfo->GetName()<<endl;
	}
	*/

    return TypeInfo;
}

vector<int> TupleValExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

    list<ExprAST *>::iterator iter;
    for(iter=Value->begin();iter!=Value->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

TypeAST* TupleValExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars)
{
	list<ExprAST*>::iterator iter;
	vector<TypeAST*> typelist;
    for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsBuilt()==false){
			(*iter)=(dynamic_cast<UnBuiltExprAST*>(*iter))->BuildAST(geninfo);
		}
		(*iter)->CheckType(env,geninfo,CurrentLocalVars);

		typelist.push_back((*iter)->TypeInfo);
    }
	TypeInfo=new TupleTypeAST(typelist); //要素の型のリスト

	/*if(IsConstant()){
		list<int> *const_list=new list<int>();
		list<ExprAST*>::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			const_list->push_back((*iter)->GetVMValue(geninfo));
		}
		PoolIndex=geninfo->PublicConstantPool.SetReference(const_list);
		//cout<<"#"<<PoolIndex<<" : <tuple>"<<TypeInfo->GetName()<<endl;
	}*/

    return TypeInfo;
}

vector<int> DataValExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

    vector< pair<string,ExprAST *> >::iterator iter;
    for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		list_tmp=(*iter).second->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

TypeAST* DataValExprAST::CheckType(vector<Environment> *env,CodegenInfo *geninfo,vector< pair<string,TypeAST*> > *CurrentLocalVars)
{
	vector< pair<string,ExprAST*> >::iterator iter;
	vector< pair<string,TypeAST*> >::iterator iter2;
	vector< DataDefAST* >::iterator iter3;
	vector< pair<string,ExprAST*> > *data_temp=new vector< pair<string,ExprAST*> >();

	bool tfound=false;
	DataDefAST* templatedatadef;
	for(iter3=geninfo->TopLevelDataDef.begin();iter3!=geninfo->TopLevelDataDef.end();iter3++){
		if((*iter3)->Name==TypeInfo->GetName()){
			tfound=true;
			templatedatadef=(*iter3);
			break;
		}
	}
	if(!tfound){error("型"+TypeInfo->GetName()+"は定義されていません");}

	vector<TypeAST*> typelist;
	for(iter2=templatedatadef->MemberList.begin();iter2!=templatedatadef->MemberList.end();iter2++){
		data_temp->push_back(pair<string,ExprAST*>((*iter2).first,NULL));
		typelist.push_back((*iter2).second);
	}

    for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		if((*iter).second->IsBuilt()==false){
			(*iter).second=dynamic_cast<UnBuiltExprAST*>((*iter).second)->BuildAST(geninfo);
		}
		(*iter).second->CheckType(env,geninfo,CurrentLocalVars);

		vector< pair<string,ExprAST*> >::iterator iter4;
		bool found=false;
		int index=0;
		for(iter4=data_temp->begin();iter4!=data_temp->end();iter4++){
			if((*iter4).first==(*iter).first){
				found=true;
				if(typelist[index]->GetName()!=(*iter).second->TypeInfo->GetName()){
					error("データ初期化の型が定義と一致しません");
				}
				(*iter4).second=(*iter).second;
				break;
			}
			index++;
		}
		if(!found){error("宣言されていないメンバ"+(*iter).first+"を初期化しようとしました");}
    }

    //全要素をもち、初期化されていないものはNULLになっている
    InitValue=data_temp;

    return TypeInfo;
}

bool ListValExprAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	list<ExprAST*>::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool TupleValExprAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	list<ExprAST*>::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool DataValExprAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	vector< pair<string,ExprAST*> >::iterator iter;
	for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		if((*iter).second->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool FunctionAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	if(isBuiltin){
		return is_builtin_CTFEable;
	}

	if(Name=="<anonymous>"){
		return false; //クロージャはフレームへのポインタを持つため事前実行は不可
	}

	//curr_fun_indexにはPoolIndexでなく、CodegenInfo.TopLevelFunctionListでのインデックスを指定すべき
	vector<FunctionAST*>::iterator found=find(cgi->TopLevelFunction.begin(),cgi->TopLevelFunction.end(),const_cast<FunctionAST*>(this));
	size_t index=distance(cgi->TopLevelFunction.begin(),found);
	return Body->IsCTFEable(cgi,index);
}

bool CallExprAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	if(callee==NULL){
		return true;
	}
	vector<ExprAST*>::iterator iter;
	for(iter=args->begin();iter!=args->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}

	return callee->IsCTFEable(cgi,curr_fun_index);
};

bool ListRefExprAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	return target->IsCTFEable(cgi,curr_fun_index) && IndexExpression->IsCTFEable(cgi,curr_fun_index);
};

bool DataMemberRefExprAST::IsCTFEable(CodegenInfo *cgi,int curr_fun_index){
	return target->IsCTFEable(cgi,curr_fun_index);
};

bool IfStatementAST::IsCTFEable(CodegenInfo* cgi,int curr_fun_index){
	return Condition->IsCTFEable(cgi,curr_fun_index)&&ThenBody->IsCTFEable(cgi,curr_fun_index)&&(ElseBody==NULL?true:ElseBody->IsCTFEable(cgi,curr_fun_index));
}

bool WhileStatementAST::IsCTFEable(CodegenInfo* cgi,int curr_fun_index){
	return Condition->IsCTFEable(cgi,curr_fun_index)&&Body->IsCTFEable(cgi,curr_fun_index);
}

bool ReturnStatementAST::IsCTFEable(CodegenInfo* cgi,int curr_fun_index){
	return (Expression==NULL?true:Expression->IsCTFEable(cgi,curr_fun_index));
}

bool VariableDefStatementAST::IsCTFEable(CodegenInfo* cgi,int curr_fun_index){
	return InitialValue==NULL?true:InitialValue->IsCTFEable(cgi,curr_fun_index);
}

bool ExpressionStatementAST::IsCTFEable(CodegenInfo* cgi,int curr_fun_index){
	return Expression->IsCTFEable(cgi,curr_fun_index);
}

bool BlockAST::IsCTFEable(CodegenInfo* cgi,int curr_fun_index){
	vector<StatementAST*>::iterator iter;
	for(iter=Body->begin();iter!=Body->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}


vector<ExprAST*> IfStatementAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	temp=Condition->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	temp=ThenBody->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	if(ElseBody!=NULL){
		temp=ElseBody->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}

	return result;
}

vector<ExprAST*> WhileStatementAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	temp=Condition->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	temp=Body->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}


vector<ExprAST*> ReturnStatementAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	if(Expression!=NULL){
		temp=Expression->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<ExprAST*> VariableDefStatementAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	if(InitialValue!=NULL){
		temp=InitialValue->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<ExprAST*> ExpressionStatementAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	temp=Expression->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}

vector<ExprAST*> BlockAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	vector<StatementAST*>::iterator iter;
	for(iter=Body->begin();iter!=Body->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}

	return result;
}

vector<ExprAST*> UnBuiltExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	return result;
}

vector<ExprAST*> IntValExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	return result;
}

vector<ExprAST*> BoolValExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	return result;
}

vector<ExprAST*> StringValExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	return result;
}

vector<ExprAST*> ListValExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	list<ExprAST*>::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<ExprAST*> TupleValExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	list<ExprAST*>::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<ExprAST*> DataValExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;

	vector< pair<string,ExprAST*> >::iterator iter;
	for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		temp=(*iter).second->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<ExprAST*> OperatorAST::GetCallExprList()
{
	vector<ExprAST*> result;
	return result;
}

vector<ExprAST*> FunctionAST::GetCallExprList()
{
	vector<ExprAST*> result;
	result=Body->GetCallExprList();
	return result;
}

vector<ExprAST*> VariableExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	return result;
}

vector<ExprAST*> CallExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;
	vector<ExprAST*>::iterator iter;
	for(iter=args->begin();iter!=args->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}

	if(callee!=NULL){
		temp=callee->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	//自身を追加
	if(callee!=NULL){
		result.push_back(this);
	}

	return result;
}

vector<ExprAST*> ListRefExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;
	vector<ExprAST*>::iterator iter;

	temp=target->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	temp=IndexExpression->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}

vector<ExprAST*> DataMemberRefExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;
	vector<ExprAST*>::iterator iter;

	temp=target->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}

vector<ExprAST*> UnaryExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;
	temp=Operand->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	return result;
}

vector<ExprAST*> BinaryExprAST::GetCallExprList()
{
	vector<ExprAST*> result;
	vector<ExprAST*> temp;
	temp=LHS->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	temp=RHS->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	return result;
}

