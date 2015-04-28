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
#include <memory>

class CodegenInfo;


//型情報を元に初期値を返します
shared_ptr<ExprAST> GetInitValue(shared_ptr<TypeAST>  type,shared_ptr<CodegenInfo> cgi){
	if(typeid(BasicTypeAST)==typeid(*type)){
		string name=type->GetName();
		if(name=="int"){
			return make_shared<IntValExprAST>(0);
		}else if(name=="string"){
			return make_shared<StringValExprAST>(cgi,"");
		}else if(name=="bool"){
			return make_shared<BoolValExprAST>(false);
		}
		error("unknown type.");
	}else if(typeid(ListTypeAST)==typeid(*type)){
		auto emptylist=make_shared< list< shared_ptr<ExprAST> > >();
		auto emptylistval=make_shared< ListValExprAST >(cgi,emptylist);
		return emptylistval;
	}else if(typeid(TupleTypeAST)==typeid(*type)){
		shared_ptr<list<shared_ptr<ExprAST> > > initlist=make_shared<list<shared_ptr<ExprAST> > >();
		shared_ptr<TupleTypeAST>  tt=dynamic_pointer_cast<TupleTypeAST>(type);
		vector<shared_ptr<TypeAST> >::iterator iter;
		for(iter=tt->ContainTypeList.begin();iter!=tt->ContainTypeList.end();iter++){
			initlist->push_back(GetInitValue((*iter),cgi));
		}
		return make_shared<TupleValExprAST>(cgi,initlist);
	}else{
		error("初期化できません");
	}
}

vector<int> FunctionAST::FindChildFunction()
{
	vector<int> list_tmp;

    //ChildPoolIndex=make_shared<vector<int> >();

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

    vector<shared_ptr<StatementAST> >::iterator iter;
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

    vector<shared_ptr<StatementAST> >::iterator iter;
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
	vector<shared_ptr<ExprAST> >::iterator iter;

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
	vector<shared_ptr<ExprAST> >::iterator iter;

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
	vector<shared_ptr<ExprAST> >::iterator iter;

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
	vector<shared_ptr<ExprAST> >::iterator iter;

	list_tmp=target->FindChildFunction();
	result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());

	return result_list;
}


void VariableExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
	bytecodes->push_back(loadlocal);
	bytecodes->push_back(FlameBack);
	bytecodes->push_back(LocalIndex);

    return;
}

void UnaryExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
    Operand->Codegen(bytecodes,geninfo);
    if(Operator=="!"){
        if(TypeInfo->GetName()=="bool"){
            bytecodes->push_back(bnot);
        }
    }else if(Operator=="-"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(ineg);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dneg);
        }
    }
    return;
}

void BinaryExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
	/*
    //代入だけ特殊で、左辺を評価しない
    if(Operator=="="){
		if(typeid(*LHS)==typeid(ListRefExprAST)){
			RHS->Codegen(bytecodes,geninfo);
			shared_ptr<ListRefExprAST> lhs_cast=dynamic_cast<shared_ptr<ListRefExprAST> >(LHS);
			//一旦スタックにロードして、それに対して代入
			bytecodes->push_back(loadlocal);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->FlameBack);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->LocalIndex);
			lhs_cast->IndexExpression->Codegen(bytecodes,geninfo);
			bytecodes->push_back(storebyindex);
		}else if(typeid(*LHS)==typeid(DataMemberRefExprAST)){
			RHS->Codegen(bytecodes,geninfo);
			shared_ptr<DataMemberRefExprAST> lhs_cast=dynamic_cast<shared_ptr<DataMemberRefExprAST> >(LHS);
			//一旦スタックにロードして、それに対して代入
			bytecodes->push_back(loadlocal);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->FlameBack);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(lhs_cast->target)->LocalIndex);
			bytecodes->push_back(storefield);
			bytecodes->push_back((make_shared<StringValExprAST>(geninfo,lhs_cast->MemberName))->PoolIndex);
		}else{
			RHS->Codegen(bytecodes,geninfo);

			bytecodes->push_back(storelocal);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(LHS)->FlameBack);
			bytecodes->push_back(dynamic_cast<VariableExprAST *>(LHS)->LocalIndex);
		}

        return;
    }
	*/

    LHS->Codegen(bytecodes,geninfo);
    RHS->Codegen(bytecodes,geninfo);

    if(Operator=="+"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(iadd);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dadd);
        }
    }else if(Operator=="-"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(isub);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dsub);
        }
    }else if(Operator=="*"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(imul);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dmul);
        }
    }else if(Operator=="/"){
        if(TypeInfo->GetName()=="int"){
            bytecodes->push_back(idiv);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(ddiv);
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
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpeq);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpeq);
        }else if(TypeInfo->GetName()=="bool"){
			bytecodes->push_back(bcmpeq);
        }
    }else if(Operator=="!="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpne);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpne);
        }else if(TypeInfo->GetName()=="bool"){
			bytecodes->push_back(bcmpne);
        }
    }else if(Operator=="<"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmplt);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmplt);
        }
    }else if(Operator==">"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpgt);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpgt);
        }
    }else if(Operator=="<="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmple);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmple);
        }
    }else if(Operator==">="){
		if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpge);
        }else if(TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpge);
        }
    }
    return;
}

void CallExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
	if(callee==nullptr){
		//計算済み
		bytecodes->push_back(ldc);
		bytecodes->push_back(geninfo->PublicConstantPool.SetValue(CalculatedValue));
		return;
	}
    vector<shared_ptr<ExprAST> >::reverse_iterator iter;
    for(iter=args->rbegin();iter!=args->rend();iter++){
        (*iter)->Codegen(bytecodes,geninfo);
    }
    callee->Codegen(bytecodes,geninfo);
    bytecodes->push_back(invoke);
}

void ListRefExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
	target->Codegen(bytecodes,geninfo);
	IndexExpression->Codegen(bytecodes,geninfo);
	bytecodes->push_back(loadbyindex);
}

void DataMemberRefExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
	target->Codegen(bytecodes,geninfo);
	bytecodes->push_back(loadfield);
	bytecodes->push_back((make_shared<StringValExprAST>(geninfo,MemberName))->PoolIndex);
}

//自身が関数ならばbodyのコード生成を行う。
//自身がクロージャならそれに加えてスタックに自身のポインタを置くコードを書く
void FunctionAST::Codegen(shared_ptr<vector<int> > bytecodes_given,shared_ptr<CodegenInfo> geninfo)
{
    Body->Codegen(bytecodes,geninfo);

    //最後にreturnを挿入（return文が省略された時のため）
    //戻り値の型で場合分け
    shared_ptr<TypeAST> rettype=dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.back();
    if(rettype->GetName()=="void"){
        bytecodes->push_back(ret);
    }else{
    	//非voidな関数でreturnされないときは-1を返す
    	bytecodes->push_back(ipush);
    	bytecodes->push_back(-1);
        bytecodes->push_back(ret_withvalue);
    }

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

void IntValExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo){
    bytecodes->push_back(ipush);
    bytecodes->push_back(Value);
    return;
}


void BoolValExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
    if(Value){
        bytecodes->push_back(bpush);
        bytecodes->push_back(1);
    }else{
        bytecodes->push_back(bpush);
        bytecodes->push_back(0);
    }
}

void StringValExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
    bytecodes->push_back(ldc);
    bytecodes->push_back(PoolIndex);
}

void ReturnStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	if(Expression!=nullptr){
		Expression->Codegen(bytecodes,geninfo);
		bytecodes->push_back(ret_withvalue);
	}else{
        bytecodes->push_back(ret);
    }
}

shared_ptr<ExprAST> UnBuiltExprAST::BuildAST(shared_ptr<CodegenInfo> geninfo)
{
    //式の解析を操車場アルゴリズムで行う
    //一旦RPNにして、それからASTにする
    queue<shared_ptr<ExprAST> > output;
    stack<shared_ptr<OperatorAST> > operatorstack;
	int input_pos=0;

    while(input_pos < ExprList->size()){
        if(typeid(OperatorAST) == typeid(*(ExprList->at(input_pos)))){
            if(geninfo->OperatorList.count(dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Operator)==0){
                //未登録の演算子
                error("未定義の演算子です:"+dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Operator);
            }
            while(!operatorstack.empty()){
                string op1=dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Operator;
                string op2=operatorstack.top()->Operator;
                bool unary_force=false;

                if(geninfo->OperatorList.count(op1)==2){
					//unary/binaryの両方が存在
					if(input_pos==0){
						//強制的にunaryとして扱う
						unary_force=true;
					}
					multimap<string,OperatorInfo>::iterator iter=geninfo->OperatorList.find(op1);
					for(int i=0;i<2;i++){
						if(unary_force && (*iter).second.UnaryOrBinary==Unary){
							op1=(*iter).first;
							break;
						}else if(!unary_force && (*iter).second.UnaryOrBinary==Binary){
							op1=(*iter).first;
							break;
						}
					}
                }else if(geninfo->OperatorList.count(op1)==1){
					multimap<string,OperatorInfo>::iterator iter=geninfo->OperatorList.find(op1);
					op1=(*iter).first;
                }else{
					error("演算子が未定義です");
                }

                if((geninfo->OperatorList.find(op1)->second.Associativity==Left && geninfo->OperatorList.find(op1)->second.Precedence<=geninfo->OperatorList.find(op2)->second.Precedence) || (geninfo->OperatorList.find(op1)->second.Precedence<geninfo->OperatorList.find(op2)->second.Precedence)){
                    output.push(operatorstack.top());
                    operatorstack.pop();
                }else{
                    break;
                }
            }
            operatorstack.push(dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos)));

        }else{
        	//演算子以外のもの
        	if(ExprList->at(input_pos)->IsBuilt()){
				output.push(ExprList->at(input_pos));
        	}else{
        		//AST生成がまだなら再帰的にAST構築
        		output.push(dynamic_pointer_cast<UnBuiltExprAST >(ExprList->at(input_pos))->BuildAST(geninfo));
        	}
        }

        input_pos++;
    }
    while(!operatorstack.empty()){
        output.push(operatorstack.top());
        operatorstack.pop();
    }

    //この時点でoutputにRPN形式で式が入っている
    stack<shared_ptr<ExprAST> > calcstack; //スタックを使いRPNを展開していく
    while(!output.empty()){
        calcstack.push(output.front());
        output.pop();
        if(typeid(*(calcstack.top()))==typeid(OperatorAST)){
            shared_ptr<OperatorAST> op=dynamic_pointer_cast<OperatorAST>(calcstack.top()); calcstack.pop();
            shared_ptr<ExprAST> operand1,operand2;

            if(geninfo->OperatorList.find(op->Operator)->second.UnaryOrBinary==Binary){
                operand2=calcstack.top(); calcstack.pop();
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(make_shared<BinaryExprAST>(op->Operator,operand1,operand2)); //マージ
            }else{
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(make_shared<UnaryExprAST>(op->Operator,operand1)); //マージ
            }
        }
    }

    if(calcstack.size()!=1){
        error("expressionに問題があります。");
    }

    return calcstack.top();
}

void IfStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	Condition->Codegen(bytecodes,geninfo);
    shared_ptr<vector<int> > thencode=make_shared<vector<int> >();
	ThenBody->Codegen(thencode,geninfo);

    shared_ptr<vector<int> > elsecode=make_shared<vector<int> >();
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

void WhileStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	shared_ptr<vector<int> > condcode=make_shared<vector<int> >();
	Condition->Codegen(condcode,geninfo);
    shared_ptr<vector<int> > bodycode=make_shared<vector<int> >();
	Body->Codegen(bodycode,geninfo);

	condcode->push_back(iffalse_skip);
	condcode->push_back(bodycode->size()+2);
	bodycode->push_back(back);
	bodycode->push_back(bodycode->size()+2+condcode->size()-1);

	bytecodes->insert(bytecodes->end(),condcode->begin(),condcode->end());
	bytecodes->insert(bytecodes->end(),bodycode->begin(),bodycode->end());
}


void VariableDefStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	if(InitialValue!=nullptr){
		InitialValue->Codegen(bytecodes,geninfo);
		bytecodes->push_back(storelocal);
		bytecodes->push_back(FlameBack);
		bytecodes->push_back(LocalIndex);
	}
}

void ExpressionStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	Expression->Codegen(bytecodes,geninfo);
}


void VariableDefStatementAST::CheckType(shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)
{
	FlameBack=0;
	LocalIndex=CurrentLocalVars->size();
	CurrentLocalVars->push_back(*Variable);
	EnvItem ei; ei.VariableInfo=*Variable; ei.LocalIndex=LocalIndex;
	env->back().Items.push_back(ei);

	if(!InitialValue){
		InitialValue=GetInitValue(Variable->second,geninfo);
	}

	if(InitialValue->IsBuilt()==false){
		InitialValue=dynamic_pointer_cast<UnBuiltExprAST>(InitialValue)->BuildAST(geninfo);
	}
	InitialValue->CheckType(env,geninfo,CurrentLocalVars);
	if(InitialValue->TypeInfo==nullptr){
		//オーバーロードの解決ができなかった
		if(Variable->second==nullptr){
			//型指定がなかった場合にはオーバーロード解決のヒントがないため、エラー
			error("オーバーロードを解決できません：変数宣言に型を付与することでオーバーロードを指定できます");
		}
		//オーバーロードのヒントを与える
		InitialValue->TypeInfo=Variable->second;
		InitialValue->CheckType(env,geninfo,CurrentLocalVars);
	}
	if(Variable->second==nullptr){
		//型が未指定だったとき
		Variable->second=InitialValue->TypeInfo;
		CurrentLocalVars->back()=*Variable;
		EnvItem ei;ei.VariableInfo=*Variable;ei.LocalIndex=LocalIndex;
		env->back().Items.back()=ei;
	}else if(InitialValue->TypeInfo->GetName() != Variable->second->GetName()){
		error("初期化できません。型が一致しません。");
	}

	return;
}

shared_ptr<TypeAST>  VariableExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	//変数を介しての呼び出しのとき

	//名前が一致するものがあるか上位のフレームに遡りながら探す
	int currentenv;
	int i;
	FlameBack=0;
	int candidate_count=0;

	if(TypeInfo==nullptr){
		//オーバーロードの数を調べる(トップレベルの関数について)
		vector<shared_ptr<FunctionAST> >::iterator iter;
		for(iter=geninfo->TopLevelFunction.begin();iter!=geninfo->TopLevelFunction.end();iter++){
			if(Name==(*iter)->Name){
				//名前が一致
				candidate_count++;
			}
		}
		if(candidate_count>1){
			//オーバーロードを解決できなかった...(型情報を与えてもらって、再度呼んでもらう)
			TypeInfo=nullptr;
			return nullptr;
		}
	}

	for(currentenv=(static_cast<int>(env->size())-1);currentenv>=0;currentenv--){
		for(i=(*env)[currentenv].Items.size()-1;i>=0;i--){
			if(Name==(*env)[currentenv].Items[i].VariableInfo.first){
				//名前が一致

				if(TypeInfo!=nullptr){
					//オーバーロード・指定された型との一致を調べる(返り値については調べない)
					if(typeid(FunctionTypeAST)==typeid(*TypeInfo)){
						if(typeid(*((*env)[currentenv].Items[i].VariableInfo.second))!=typeid(FunctionTypeAST)){
							error("定義重複：関数ではありません");
						}
						vector<shared_ptr<TypeAST> > typelist=dynamic_pointer_cast<FunctionTypeAST >(TypeInfo)->TypeList;
						vector<shared_ptr<TypeAST> > typelist2=dynamic_pointer_cast<FunctionTypeAST >((*env)[currentenv].Items[i].VariableInfo.second)->TypeList;
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

shared_ptr<TypeAST>  UnaryExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	//循環防止の為
	if(TypeInfo!=nullptr){
		return TypeInfo;
	}

	shared_ptr<TypeAST> oprandt=Operand->CheckType(env,geninfo,CurrentLocalVars);
	if(Operator=="!"){
		if(oprandt->GetName()!="bool"){
			error("型に問題があります。単項演算子:"+Operator+" オペランド:"+oprandt->GetName());
		}
		TypeInfo=oprandt;
	}

	if(TypeInfo==nullptr){
		error("未知の単項演算子です:"+Operator);
	}

	return TypeInfo;
}

shared_ptr<TypeAST>  BinaryExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	shared_ptr<TypeAST> lhst=LHS->CheckType(env,geninfo,CurrentLocalVars);
	shared_ptr<TypeAST> rhst=RHS->CheckType(env,geninfo,CurrentLocalVars);
	if(rhst==nullptr){
		if(Operator=="="){
			//オーバーロードの解決に失敗
			RHS->TypeInfo=LHS->TypeInfo;
			rhst=RHS->CheckType(env,geninfo,CurrentLocalVars);
		}else{
			error("オーバーロードの解決ができません");
		}
	}

	TypeInfo=nullptr;

	//組み込み型の型チェック
	if(Operator=="+" || Operator=="-" || Operator=="*" || Operator=="/"){
		if(lhst->GetName() != rhst->GetName() || (lhst->GetName()!="int" && lhst->GetName()!="double")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}

		TypeInfo=lhst; //オペランドの型を元に自らの型を決める
	}else if(Operator=="%" || Operator=="<<" || Operator==">>"){
		if(lhst->GetName() != rhst->GetName() || (lhst->GetName()!="int")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=lhst;
	}else if(Operator=="&&" || Operator=="||"){
		if(lhst->GetName() != rhst->GetName() || (lhst->GetName()!="bool")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=lhst;
	}else if(Operator=="<" || Operator==">" || Operator=="<=" || Operator==">="){
		if(lhst->GetName() != rhst->GetName() || (lhst->GetName()!="int" && lhst->GetName()!="double")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=make_shared<BasicTypeAST>("bool");
	}else if(Operator=="==" || Operator=="!="){
		if(lhst->GetName() != rhst->GetName() || (lhst->GetName()!="int" && lhst->GetName()!="bool" && lhst->GetName()!="double")){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}
		TypeInfo=make_shared<BasicTypeAST>("bool");
	}else if(Operator=="="){
		if(lhst->GetName() != rhst->GetName()){
			error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
		}else if(typeid(*LHS)!=typeid(VariableExprAST) && typeid(*LHS)!=typeid(ListRefExprAST) && typeid(*LHS)!=typeid(DataMemberRefExprAST)){
			error("代入式の左辺が変数ではありません。");
		}
		TypeInfo=lhst;
	}

	if(TypeInfo==nullptr){
		error("未知の二項演算子です: "+Operator);
	}

	return TypeInfo;
}

void ReturnStatementAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(Expression==nullptr){return;}
	if(Expression->IsBuilt()==false){
		Expression=dynamic_pointer_cast<UnBuiltExprAST >(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo,CurrentLocalVars);
}

void IfStatementAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(Condition->IsBuilt()==false){
		Condition=dynamic_pointer_cast<UnBuiltExprAST >(Condition)->BuildAST(geninfo);
	}
	Condition->CheckType(env,geninfo,CurrentLocalVars);
	if(Condition->TypeInfo->GetName()!="bool"){
		error("条件式の型がboolではありません");
	}

	ThenBody->CheckType(env,geninfo,false,CurrentLocalVars);
	ElseBody->CheckType(env,geninfo,false,CurrentLocalVars);

	return;
}

void WhileStatementAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(Condition->IsBuilt()==false){
		Condition=dynamic_pointer_cast<UnBuiltExprAST >(Condition)->BuildAST(geninfo);
	}
	Condition->CheckType(env,geninfo,CurrentLocalVars);
	if(Condition->TypeInfo->GetName()!="bool"){
		error("条件式の型がboolではありません");
	}

	Body->CheckType(env,geninfo,false,CurrentLocalVars);

	return;
}


void ExpressionStatementAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(Expression->IsBuilt()==false){
		Expression=dynamic_pointer_cast<UnBuiltExprAST >(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo,CurrentLocalVars);
}

shared_ptr<TypeAST>  FunctionAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	//LocalVariables=make_shared<vector< pair<string,shared_ptr<TypeAST> > > >();

	vector< pair<string,shared_ptr<TypeAST> > >::iterator argiter;
	for(argiter=Args->begin();argiter!=Args->end();argiter++){
		LocalVariables->push_back(*argiter);
	}

	Body->CheckType(env,geninfo,true,LocalVariables);

	vector<shared_ptr<StatementAST> >::iterator iter2;

	for(iter2=Body->Body->begin();iter2!=Body->Body->end();iter2++){
		if(typeid(*(*iter2))==typeid(ReturnStatementAST)){
			//return文の場合特別扱い。返す値の型とこの関数の返り値の型が不一致ならばエラー
			if(dynamic_pointer_cast<ReturnStatementAST>(*iter2)->Expression==nullptr){
				if(dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.back()->GetName()=="!!undefined!!"){
					//推論が必要
					dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.pop_back();
					dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.push_back(make_shared<BasicTypeAST>("void"));
				}else if(dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.back()->GetName() != "void"){
					error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
				}
			}else if(dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.back()->GetName()=="!!undefined!!"){
				//推論が必要
				dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.pop_back();
				dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.push_back(dynamic_pointer_cast<ReturnStatementAST>(*iter2)->Expression->TypeInfo);
			}else if(dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.back()->GetName() != dynamic_pointer_cast<ReturnStatementAST>(*iter2)->Expression->TypeInfo->GetName()){
				error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
			}
		}
	}

	if(dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.back()->GetName()=="!!undefined!!"){
		dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.pop_back();
		dynamic_pointer_cast<FunctionTypeAST>(TypeInfo)->TypeList.push_back(make_shared<BasicTypeAST>("void"));
	}

	return TypeInfo; //自らの型を一応返しておく
}

shared_ptr<TypeAST>  CallExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	//引数の型を順に決めていく
	vector<shared_ptr<TypeAST> > argtype;
	vector<shared_ptr<ExprAST> >::iterator iter2;
	for(iter2=args->begin();iter2!=args->end();iter2++){
		if((*iter2)->IsBuilt()==false){
			(*iter2)=(dynamic_pointer_cast<UnBuiltExprAST >(*iter2))->BuildAST(geninfo);
		}
		(*iter2)->CheckType(env,geninfo,CurrentLocalVars);
		argtype.push_back((*iter2)->TypeInfo);
	}

	argtype.push_back(make_shared<BasicTypeAST>("void"));


	if(callee->IsBuilt()==false){
		callee=dynamic_pointer_cast<UnBuiltExprAST >(callee)->BuildAST(geninfo);
	}

	callee->CheckType(env,geninfo,CurrentLocalVars);

	if(callee->TypeInfo!=nullptr && typeid(FunctionTypeAST) != typeid(*(callee->TypeInfo))){
		error("calleeが関数ではありません");
	}

	if(callee->TypeInfo==nullptr){
		//オーバーロードの解決ができない
		callee->TypeInfo=make_shared<FunctionTypeAST>(argtype);
		callee->CheckType(env,geninfo,CurrentLocalVars);
		if(callee->TypeInfo==nullptr){
			error("再帰呼び出しをするには型情報を記述してください");
		}
	}


	vector<shared_ptr<TypeAST> > currentarg=dynamic_pointer_cast<FunctionTypeAST>(callee->TypeInfo)->TypeList;
	if(args->size()+1==currentarg.size()){ //+1するのは、argsには戻り値の型が含まれていないから
		for(unsigned int j=0;j<args->size();j++){
			if(args->at(j)->TypeInfo->GetName() != currentarg[j]->GetName()){
				goto type_error;
			}
		}
		TypeInfo=currentarg.back(); //関数の型ではなく関数の戻り値の型を代入

		return TypeInfo;
	}
	type_error:
	error("引数リストの型とcalleeの引数の型が一致しません");
}

shared_ptr<TypeAST>  ListRefExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(IndexExpression->IsBuilt()==false){
		IndexExpression=dynamic_pointer_cast<UnBuiltExprAST >(IndexExpression)->BuildAST(geninfo);
	}
	IndexExpression->CheckType(env,geninfo,CurrentLocalVars);
	if(IndexExpression->TypeInfo->GetName()!="int"){
		error("添字は整数で指定してください");
	}

	if(target->IsBuilt()==false){
		target=dynamic_pointer_cast<UnBuiltExprAST >(target)->BuildAST(geninfo);
	}
	target->CheckType(env,geninfo,CurrentLocalVars);

	if(typeid(ListTypeAST) != typeid(*(target->TypeInfo)) && typeid(TupleTypeAST) != typeid(*(target->TypeInfo))){
		error("添字を指定できるのはリストまたはタプルです");
	}

	if(typeid(ListTypeAST) == typeid(*(target->TypeInfo))){
		TypeInfo=dynamic_pointer_cast<ListTypeAST>(target->TypeInfo)->ContainType;
	}else if(typeid(TupleTypeAST) == typeid(*(target->TypeInfo))){
		if(typeid(*IndexExpression) != typeid(IntValExprAST)){
			error("タプルの添字は整数定数を指定してください");
		}
		if(dynamic_pointer_cast<IntValExprAST>(IndexExpression)->Value<0 || dynamic_pointer_cast<IntValExprAST>(IndexExpression)->Value>=dynamic_pointer_cast<TupleTypeAST>(target->TypeInfo)->ContainTypeList.size()){
			error("タプルの範囲外です");
		}
		TypeInfo=dynamic_pointer_cast<TupleTypeAST>(target->TypeInfo)->ContainTypeList[dynamic_pointer_cast<IntValExprAST>(IndexExpression)->Value];
	}

	return TypeInfo;
}

shared_ptr<TypeAST>  DataMemberRefExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(target->IsBuilt()==false){
		target=dynamic_pointer_cast<UnBuiltExprAST >(target)->BuildAST(geninfo);
	}
	target->CheckType(env,geninfo,CurrentLocalVars);
	if(typeid(BasicTypeAST) != typeid(*(target->TypeInfo))){
		error("メンバ参照の左辺の型が不正です");
	}

	vector< pair<string,shared_ptr<TypeAST> >  >::iterator miter;
	vector<shared_ptr<DataDefAST> >::iterator diter;
	bool found=false;
	for(diter=geninfo->TopLevelDataDef.begin();diter!=geninfo->TopLevelDataDef.end();diter++){
		if((*diter)->Name==target->TypeInfo->GetName()){
			found=true;
			break;
		}
	}
	if(!found){
		error("型"+target->TypeInfo->GetName()+"は構造体型ではありません：メンバ参照の左辺を確認してください");
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

	vector<shared_ptr<GroupDefAST> >::iterator giter;
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


void BlockAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
    shared_ptr<vector<int> > codes=make_shared<vector<int> >();

    vector<shared_ptr<StatementAST> >::iterator itere;
    for(itere=Body->begin();itere!=Body->end();itere++){
        (*itere)->Codegen(codes,geninfo);
    }

    bytecodes->insert(bytecodes->end(),codes->begin(),codes->end());

    return;
}

void BlockAST::CheckType(shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo,bool addargs,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars/*現在の関数のローカル変数領域へのポインタ*/)
{
	Environment newflame;
	newflame.LocalVariablesPtr=CurrentLocalVars;

	if(addargs){
		//引数を追加する場合、つまり関数ブロックである場合（if,whileでは呼ばれない）
		newflame.is_internalblock=false;
		for(int i=0;i<CurrentLocalVars->size();i++){
			EnvItem ei;ei.VariableInfo=CurrentLocalVars->at(i);ei.LocalIndex=i;
			newflame.Items.push_back(ei);
		}
	}else{
		newflame.is_internalblock=true;
	}

	env->push_back(newflame);

	vector<shared_ptr<StatementAST> >::iterator iter2;
	for(iter2=Body->begin();iter2!=Body->end();iter2++){
		(*iter2)->CheckType(env,geninfo,CurrentLocalVars);
	}

	env->pop_back(); //最後尾のフレームを削除
}

vector<int> BlockAST::FindChildFunction()
{
	vector<int> list_tmp;
	vector<int> result;
    vector<shared_ptr<StatementAST> >::iterator iter;

    for(iter=Body->begin();iter!=Body->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result.insert(result.end(),list_tmp.begin(),list_tmp.end());
    }

    return result;
}

void ListValExprAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	/*if(PoolIndex!=-1){
		bytecodes->push_back(ipush);
		bytecodes->push_back(PoolIndex);
		//定数としてリストが生成されたが，例えばクロージャなら内部のコード生成が必要
		//でも、bytecodesに余計なコードが入るといけないので新たなベクタにコードを吐かせてそれは捨てる
		vector<int> dummy;
		list<shared_ptr<ExprAST> >::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			(*iter)->Codegen(&dummy,geninfo);
		}
	}else{*/
		list<shared_ptr<ExprAST> >::reverse_iterator riter;
		for(riter=Value->rbegin();riter!=Value->rend();riter++){
			(*riter)->Codegen(bytecodes,geninfo);
		}
		bytecodes->push_back(makelist);
		bytecodes->push_back(Value->size());
	//}
}

void TupleValExprAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	/*if(PoolIndex!=-1){
		bytecodes->push_back(ipush);
		bytecodes->push_back(PoolIndex);
		//定数としてリストが生成されたが，例えばクロージャなら内部のコード生成が必要
		//でも、bytecodesに余計なコードが入るといけないので新たなベクタにコードを吐かせてそれは捨てる
		vector<int> dummy;
		list<shared_ptr<ExprAST> >::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			(*iter)->Codegen(&dummy,geninfo);
		}
	}else{*/
		list<shared_ptr<ExprAST> >::reverse_iterator riter;
		for(riter=Value->rbegin();riter!=Value->rend();riter++){
			(*riter)->Codegen(bytecodes,geninfo);
		}
		bytecodes->push_back(makelist);
		bytecodes->push_back(Value->size());
	//}
}

void DataValExprAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	vector< pair<string,shared_ptr<ExprAST> > >::reverse_iterator riter;
	for(riter=InitValue->rbegin();riter!=InitValue->rend();riter++){
		(*riter).second->Codegen(bytecodes,geninfo);
		bytecodes->push_back(ipush);
		bytecodes->push_back((make_shared<StringValExprAST>(geninfo,(*riter).first))->PoolIndex);
	}
	bytecodes->push_back(makedata);
	bytecodes->push_back((make_shared<StringValExprAST>(geninfo,TypeInfo->GetName()))->PoolIndex);
	bytecodes->push_back(InitValue->size());
}

vector<int> ListValExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

    list<shared_ptr<ExprAST> >::iterator iter;
    for(iter=Value->begin();iter!=Value->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

shared_ptr<TypeAST>  ListValExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)
{
	list<shared_ptr<ExprAST> >::iterator iter;
    for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsBuilt()==false){
			(*iter)=(dynamic_pointer_cast<UnBuiltExprAST >(*iter))->BuildAST(geninfo);
		}
		(*iter)->CheckType(env,geninfo,CurrentLocalVars);
		if(TypeInfo==nullptr){
			TypeInfo=(*iter)->TypeInfo;
		}else{
			if(TypeInfo->GetName()!=(*iter)->TypeInfo->GetName()){
				error("リストの要素の型がバラバラです。");
			}
		}
    }
	TypeInfo=make_shared<ListTypeAST>(TypeInfo); //要素の型のリスト

	/*
	if(IsConstant()){
		list<int> *const_list=make_shared<list<int> >();
		list<shared_ptr<ExprAST> >::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			const_list->push_back((*iter)->GetVMValue(geninfo));
		}
		PoolIndex=geninfo->PublicConstantPool.SetValue(const_list);
		//cout<<"#"<<PoolIndex<<" : <list>"<<TypeInfo->GetName()<<endl;
	}
	*/

    return TypeInfo;
}

vector<int> TupleValExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

    list<shared_ptr<ExprAST> >::iterator iter;
    for(iter=Value->begin();iter!=Value->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

shared_ptr<TypeAST>  TupleValExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)
{
	list<shared_ptr<ExprAST> >::iterator iter;
	vector<shared_ptr<TypeAST> > typelist;
    for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsBuilt()==false){
			(*iter)=(dynamic_pointer_cast<UnBuiltExprAST >(*iter))->BuildAST(geninfo);
		}
		(*iter)->CheckType(env,geninfo,CurrentLocalVars);

		typelist.push_back((*iter)->TypeInfo);
    }
	TypeInfo=make_shared<TupleTypeAST>(typelist); //要素の型のリスト

	/*if(IsConstant()){
		list<int> *const_list=make_shared<list<int> >();
		list<shared_ptr<ExprAST> >::iterator iter;
		for(iter=Value->begin();iter!=Value->end();iter++){
			const_list->push_back((*iter)->GetVMValue(geninfo));
		}
		PoolIndex=geninfo->PublicConstantPool.SetValue(const_list);
		//cout<<"#"<<PoolIndex<<" : <tuple>"<<TypeInfo->GetName()<<endl;
	}*/

    return TypeInfo;
}

vector<int> DataValExprAST::FindChildFunction()
{
	vector<int> result_list;
	vector<int> list_tmp;

    vector< pair<string,shared_ptr<ExprAST> > >::iterator iter;
    for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		list_tmp=(*iter).second->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

shared_ptr<TypeAST>  DataValExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)
{
	vector< pair<string,shared_ptr<ExprAST> > >::iterator iter;
	vector< pair<string,shared_ptr<TypeAST> >  >::iterator iter2;
	vector< shared_ptr<DataDefAST> >::iterator iter3;
	shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > data_temp=make_shared< vector< pair<string,shared_ptr<ExprAST> > > >();

	bool tfound=false;
	shared_ptr<DataDefAST> templatedatadef;
	for(iter3=geninfo->TopLevelDataDef.begin();iter3!=geninfo->TopLevelDataDef.end();iter3++){
		if((*iter3)->Name==TypeInfo->GetName()){
			tfound=true;
			templatedatadef=(*iter3);
			break;
		}
	}
	if(!tfound){error("型"+TypeInfo->GetName()+"は定義されていません");}

	vector<shared_ptr<TypeAST> > typelist;
	for(iter2=templatedatadef->MemberList.begin();iter2!=templatedatadef->MemberList.end();iter2++){
		data_temp->push_back(pair<string,shared_ptr<ExprAST> >((*iter2).first,GetInitValue((*iter2).second,geninfo)));
		typelist.push_back((*iter2).second);
	}

    for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		if((*iter).second->IsBuilt()==false){
			(*iter).second=dynamic_pointer_cast<UnBuiltExprAST >((*iter).second)->BuildAST(geninfo);
		}
		(*iter).second->CheckType(env,geninfo,CurrentLocalVars);

		vector< pair<string,shared_ptr<ExprAST> > >::iterator iter4;
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

    //全要素をもち、初期化されていないものはnullptrになっている
    InitValue=data_temp;

    return TypeInfo;
}

bool ListValExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	list<shared_ptr<ExprAST> >::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool TupleValExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	list<shared_ptr<ExprAST> >::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool DataValExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	vector< pair<string,shared_ptr<ExprAST> > >::iterator iter;
	for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		if((*iter).second->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool FunctionAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	if(isBuiltin){
		return is_builtin_CTFEable;
	}

	if(Name=="<anonymous>"){
		return false; //クロージャはフレームへのポインタを持つため事前実行は不可
	}

	//curr_fun_indexにはPoolIndexでなく、CodegenInfo.TopLevelFunctionListでのインデックスを指定すべき
	vector<shared_ptr<FunctionAST> >::iterator found=find(cgi->TopLevelFunction.begin(),cgi->TopLevelFunction.end(),shared_from_this());
	size_t index=distance(cgi->TopLevelFunction.begin(),found);
	return Body->IsCTFEable(cgi,index);
}

bool CallExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	if(callee==nullptr){
		return true;
	}
	vector<shared_ptr<ExprAST> >::iterator iter;
	for(iter=args->begin();iter!=args->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}

	return callee->IsCTFEable(cgi,curr_fun_index);
};

bool ListRefExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return target->IsCTFEable(cgi,curr_fun_index) && IndexExpression->IsCTFEable(cgi,curr_fun_index);
};

bool DataMemberRefExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return target->IsCTFEable(cgi,curr_fun_index);
};

bool IfStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return Condition->IsCTFEable(cgi,curr_fun_index)&&ThenBody->IsCTFEable(cgi,curr_fun_index)&&(ElseBody==nullptr?true:ElseBody->IsCTFEable(cgi,curr_fun_index));
}

bool WhileStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return Condition->IsCTFEable(cgi,curr_fun_index)&&Body->IsCTFEable(cgi,curr_fun_index);
}

bool ReturnStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return (Expression==nullptr?true:Expression->IsCTFEable(cgi,curr_fun_index));
}

bool VariableDefStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return InitialValue==nullptr?true:InitialValue->IsCTFEable(cgi,curr_fun_index);
}

bool ExpressionStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return Expression->IsCTFEable(cgi,curr_fun_index);
}

bool BlockAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	vector<shared_ptr<StatementAST> >::iterator iter;
	for(iter=Body->begin();iter!=Body->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}


vector<shared_ptr<ExprAST> > IfStatementAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	temp=Condition->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	temp=ThenBody->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	if(ElseBody!=nullptr){
		temp=ElseBody->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}

	return result;
}

vector<shared_ptr<ExprAST> > WhileStatementAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	temp=Condition->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	temp=Body->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}


vector<shared_ptr<ExprAST> > ReturnStatementAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	if(Expression!=nullptr){
		temp=Expression->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<shared_ptr<ExprAST> > VariableDefStatementAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	if(InitialValue!=nullptr){
		temp=InitialValue->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<shared_ptr<ExprAST> > ExpressionStatementAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	temp=Expression->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}

vector<shared_ptr<ExprAST> > BlockAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	vector<shared_ptr<StatementAST> >::iterator iter;
	for(iter=Body->begin();iter!=Body->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}

	return result;
}

vector<shared_ptr<ExprAST> > UnBuiltExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

vector<shared_ptr<ExprAST> > IntValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

vector<shared_ptr<ExprAST> > BoolValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

vector<shared_ptr<ExprAST> > StringValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

vector<shared_ptr<ExprAST> > ListValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	list<shared_ptr<ExprAST> >::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<shared_ptr<ExprAST> > TupleValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	list<shared_ptr<ExprAST> >::iterator iter;
	for(iter=Value->begin();iter!=Value->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<shared_ptr<ExprAST> > DataValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	vector< pair<string,shared_ptr<ExprAST> > >::iterator iter;
	for(iter=InitValue->begin();iter!=InitValue->end();iter++){
		temp=(*iter).second->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<shared_ptr<ExprAST> > OperatorAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

vector<shared_ptr<ExprAST> > FunctionAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	result=Body->GetCallExprList();
	return result;
}

vector<shared_ptr<ExprAST> > VariableExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

vector<shared_ptr<ExprAST> > CallExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;
	vector<shared_ptr<ExprAST> >::iterator iter;
	for(iter=args->begin();iter!=args->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}

	if(callee!=nullptr){
		temp=callee->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	//自身を追加
	if(callee!=nullptr){
		result.push_back(shared_from_this());
	}

	return result;
}

vector<shared_ptr<ExprAST> > ListRefExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;
	vector<shared_ptr<ExprAST> >::iterator iter;

	temp=target->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	temp=IndexExpression->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}

vector<shared_ptr<ExprAST> > DataMemberRefExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;
	vector<shared_ptr<ExprAST> >::iterator iter;

	temp=target->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());

	return result;
}

vector<shared_ptr<ExprAST> > UnaryExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;
	temp=Operand->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	return result;
}

vector<shared_ptr<ExprAST> > BinaryExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;
	temp=LHS->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	temp=RHS->GetCallExprList();
	result.insert(result.end(),temp.begin(),temp.end());
	return result;
}

void DoubleValExprAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	bytecodes->push_back(ldc);
    bytecodes->push_back(PoolIndex);
    return;
}

vector<shared_ptr<ExprAST> > DoubleValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}
