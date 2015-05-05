#include <vector>
#include <string>
#include "ast_etc.h"
#include "statement.h"
#include "type.h"
#include "expression.h"
#include "vm.h"
#include "utility.h"
#include <algorithm>
#include <iostream>
#include <stack>
#include <queue>
#include <typeinfo>
#include "color_text.h"
#include <memory>
#include "expression.h"


shared_ptr<ExprAST> GetInitValue(shared_ptr<TypeAST>  type,shared_ptr<CodegenInfo> cgi);


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
    }else if(Operator=="?"){
        if(typeid(*Operand->TypeInfo)==typeid(ChannelTypeAST)){
            bytecodes->push_back(channel_receive);
        }
    }
    return;
}

void BinaryExprAST::Codegen(shared_ptr<vector<int> > bytecodes,shared_ptr<CodegenInfo> geninfo)
{
	RHS->Codegen(bytecodes,geninfo);


    //代入だけ特殊で、左辺を評価しない
    if(Operator=="="){
		if(typeid(*LHS)==typeid(ListRefExprAST)){
			shared_ptr<ListRefExprAST> lhs_cast=dynamic_pointer_cast<ListRefExprAST>(LHS);
			lhs_cast->AssignmentCodegen(bytecodes,geninfo);
		}else if(typeid(*LHS)==typeid(DataMemberRefExprAST)){
			shared_ptr<DataMemberRefExprAST> lhs_cast=dynamic_pointer_cast<DataMemberRefExprAST>(LHS);
			lhs_cast->AssignmentCodegen(bytecodes,geninfo);
		}else if(typeid(*LHS)==typeid(VariableExprAST)){
			bytecodes->push_back(storelocal);
			bytecodes->push_back(dynamic_pointer_cast<VariableExprAST>(LHS)->FlameBack);
			bytecodes->push_back(dynamic_pointer_cast<VariableExprAST>(LHS)->LocalIndex);
		}else{
			error("代入式の左辺が不正です");
		}

        return;
    }


    LHS->Codegen(bytecodes,geninfo);

    if(Operator=="+"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(iadd);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dadd);
        }
    }else if(Operator=="-"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(isub);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dsub);
        }
    }else if(Operator=="*"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(imul);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dmul);
        }
    }else if(Operator=="/"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(idiv);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(ddiv);
        }
    }else if(Operator=="<<"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(ilshift);
        }else if(typeid(*LHS->TypeInfo)==typeid(ChannelTypeAST)){
            bytecodes->push_back(channel_send);
        }
    }else if(Operator==">>"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(irshift);
        }
    }else if(Operator=="%"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(imod);
        }
    }else if(Operator=="&&"){
        if(LHS->TypeInfo->GetName()=="bool"){
            bytecodes->push_back(band);
        }
    }else if(Operator=="||"){
        if(LHS->TypeInfo->GetName()=="bool"){
            bytecodes->push_back(bor);
        }
    }else if(Operator=="=="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpeq);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpeq);
        }else if(LHS->TypeInfo->GetName()=="bool"){
			bytecodes->push_back(bcmpeq);
        }
    }else if(Operator=="!="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpne);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpne);
        }else if(LHS->TypeInfo->GetName()=="bool"){
			bytecodes->push_back(bcmpne);
        }
    }else if(Operator=="<"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmplt);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmplt);
        }
    }else if(Operator==">"){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpgt);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmpgt);
        }
    }else if(Operator=="<="){
        if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmple);
        }else if(LHS->TypeInfo->GetName()=="double"){
			bytecodes->push_back(dcmple);
        }
    }else if(Operator==">="){
		if(LHS->TypeInfo->GetName()=="int"){
            bytecodes->push_back(icmpge);
        }else if(LHS->TypeInfo->GetName()=="double"){
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
		bytecodes->push_back(VM::PublicConstantPool.SetValue(CalculatedValue));
		return;
	}
    vector<shared_ptr<ExprAST> >::reverse_iterator iter;
    for(iter=args->rbegin();iter!=args->rend();iter++){
        (*iter)->Codegen(bytecodes,geninfo);
    }
	if(args->size()==0 && typeid(*(callee->TypeInfo))==typeid(ContinuationTypeAST)){
		//引数がvoidの時でもダミーの値を乗っける
		bytecodes->push_back(pushnull);
	}
    callee->Codegen(bytecodes,geninfo);
    if(typeid(*(callee->TypeInfo))==typeid(FunctionTypeAST)){
		bytecodes->push_back(invoke);
		bytecodes->push_back(IsTail?1:0);
		bytecodes->push_back(IsAsync?1:0);
    }else if(typeid(*(callee->TypeInfo))==typeid(ContinuationTypeAST)){
		bytecodes->push_back(resume_continuation);
    }
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

void ListRefExprAST::AssignmentCodegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	target->Codegen(bytecodes,geninfo);
	IndexExpression->Codegen(bytecodes,geninfo);
	bytecodes->push_back(storebyindex);
}

void DataMemberRefExprAST::AssignmentCodegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	target->Codegen(bytecodes,geninfo);
	bytecodes->push_back(storefield);
	bytecodes->push_back((make_shared<StringValExprAST>(geninfo,MemberName))->PoolIndex);
}


shared_ptr<ExprAST> UnBuiltExprAST::BuildAST(shared_ptr<CodegenInfo> geninfo)
{
    //式の解析を操車場アルゴリズムで行う
    //一旦RPNにして、それからASTにする
    queue<shared_ptr<ExprAST> > output;
    stack<shared_ptr<OperatorAST> > operatorstack;
	int input_pos=0;

/*
	cout<<"ExprList size:"<<ExprList->size()<<endl;
	for(auto iter=ExprList->begin();iter!=ExprList->end();iter++){
		cout<<typeid(**iter).name()<<endl;
	}
*/

    while(input_pos < ExprList->size()){
        if(typeid(OperatorAST) == typeid(*(ExprList->at(input_pos)))){
            if(geninfo->OperatorList.count(dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Operator)==0){
                //未登録の演算子
                error("未定義の演算子です:"+dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Operator);
            }

            string op1=dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Operator;bool unary_force=false;

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
						dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Info=(*iter).second;
						break;
					}else if(!unary_force && (*iter).second.UnaryOrBinary==Binary){
						op1=(*iter).first;
						dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Info=(*iter).second;
						break;
					}
				}
			}else if(geninfo->OperatorList.count(op1)==1){
				multimap<string,OperatorInfo>::iterator iter=geninfo->OperatorList.find(op1);
				op1=(*iter).first;
				dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Info=(*iter).second;
			}else{
				error("演算子が未定義です");
			}

            while(!operatorstack.empty()){
                OperatorInfo op2=operatorstack.top()->Info;
                if((dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Info.Associativity==Left && dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Info.Precedence<=op2.Precedence) || (dynamic_pointer_cast<OperatorAST>(ExprList->at(input_pos))->Info.Precedence<op2.Precedence)){
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

            if(op->Info.UnaryOrBinary==Binary){
                operand2=calcstack.top(); calcstack.pop();
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(make_shared<BinaryExprAST>(op->Operator,operand1,operand2)); //マージ
            }else if(op->Info.UnaryOrBinary==Unary){
                operand1=calcstack.top(); calcstack.pop();
                calcstack.push(make_shared<UnaryExprAST>(op->Operator,operand1)); //マージ
            }else{
				error("unknown error.");
            }
        }
    }

    if(calcstack.size()!=1){
        error("expressionに問題があります。");
    }

    return calcstack.top();
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
					}else if(typeid(ContinuationTypeAST)==typeid(*TypeInfo)){
						if(typeid(*((*env)[currentenv].Items[i].VariableInfo.second))!=typeid(ContinuationTypeAST)){
							error("継続ではありません");
						}
						shared_ptr<TypeAST> type1=dynamic_pointer_cast<ContinuationTypeAST >(TypeInfo)->Type;
						shared_ptr<TypeAST> type2=dynamic_pointer_cast<ContinuationTypeAST >((*env)[currentenv].Items[i].VariableInfo.second)->Type;

						bool fail=false;

						if(type1->GetName()!=type2->GetName()){
							error("継続の型が一致しません");
						}
					}
				}


				TypeInfo=(*env)[currentenv].Items[i].VariableInfo.second;

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
	}else if(Operator=="-"){
		if(oprandt->GetName()!="int" && oprandt->GetName()!="double"){
			error("型に問題があります。単項演算子:"+Operator+" オペランド:"+oprandt->GetName());
		}
		TypeInfo=oprandt;
	}else if(Operator=="?"){
		if(!(typeid(*oprandt)==typeid(ChannelTypeAST))){
			error("型に問題があります。単項演算子:"+Operator+" オペランド:"+oprandt->GetName());
		}
		TypeInfo=dynamic_pointer_cast<ChannelTypeAST>(oprandt)->Type;
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
			if(!(Operator=="<<" && typeid(ChannelTypeAST)==typeid(*lhst) && dynamic_pointer_cast<ChannelTypeAST>(lhst)->Type->GetName()==rhst->GetName())){
				error("型に問題があります。二項演算子 "+Operator+" 左辺:"+lhst->GetName()+" 右辺:"+rhst->GetName());
			}
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

shared_ptr<TypeAST>  FunctionAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	vector< pair<string,shared_ptr<TypeAST> > >::iterator argiter;
	for(argiter=Args->begin();argiter!=Args->end();argiter++){
		LocalVariables->push_back(*argiter);
	}

	Body->CheckType(env,geninfo,true,LocalVariables);

	vector<shared_ptr<StatementAST> >::iterator iter2;

	for(iter2=Body->Body->begin();iter2!=Body->Body->end();iter2++){
		if(typeid(*(*iter2))==typeid(ReturnStatementAST)){
			auto ft_ptr=dynamic_pointer_cast<FunctionTypeAST>(TypeInfo);
			auto r_ptr=dynamic_pointer_cast<ReturnStatementAST>(*iter2);
			//return文の場合特別扱い。返す値の型とこの関数の返り値の型が不一致ならばエラー
			if(r_ptr->Expression==nullptr){
				if(ft_ptr->TypeList.back()->GetName()=="!!undefined!!"){
					//推論が必要
					ft_ptr->TypeList.pop_back();
					ft_ptr->TypeList.push_back(make_shared<BasicTypeAST>("void"));
				}else if(ft_ptr->TypeList.back()->GetName() != "void"){
					error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
				}
			}else if(ft_ptr->TypeList.back()->GetName()=="!!undefined!!"){
				//推論が必要
				ft_ptr->TypeList.pop_back();
				ft_ptr->TypeList.push_back(r_ptr->Expression->TypeInfo);
			}else if(ft_ptr->TypeList.back()->GetName() != r_ptr->Expression->TypeInfo->GetName()){
				error("'returnする値の型'と、'関数の戻り値の型'が一致しません。");
			}

			//末尾呼び出しの判定(return statement)
			if(r_ptr->Expression!=nullptr && typeid(*(r_ptr->Expression))==typeid(CallExprAST)){
				//cout<<BG_YELLOW<<"末尾呼び出しを発見しました"<<RESET<<endl;
				dynamic_pointer_cast<CallExprAST>(r_ptr->Expression)->IsTail=true;
			}
		}

		if(iter2==Body->Body->end()-1 && typeid(*(*iter2))==typeid(ExpressionStatementAST)){
			//末尾呼び出しの判定(return statement)
			if(dynamic_pointer_cast<ExpressionStatementAST>(*iter2)->Expression!=nullptr && typeid(*(dynamic_pointer_cast<ExpressionStatementAST>(*iter2)->Expression))==typeid(CallExprAST)){
				//cout<<BG_YELLOW<<"末尾呼び出しを発見しました"<<RESET<<endl;
				dynamic_pointer_cast<CallExprAST>(dynamic_pointer_cast<ExpressionStatementAST>(*iter2)->Expression)->IsTail=true;
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
		if((*iter2)->TypeInfo==nullptr){
			error("グローバル変数の型宣言は必須です");
		}
		argtype.push_back((*iter2)->TypeInfo);
	}

	argtype.push_back(make_shared<BasicTypeAST>("void"));


	if(callee->IsBuilt()==false){
		callee=dynamic_pointer_cast<UnBuiltExprAST >(callee)->BuildAST(geninfo);
	}

	callee->CheckType(env,geninfo,CurrentLocalVars);

	if(callee->TypeInfo!=nullptr && typeid(FunctionTypeAST) != typeid(*(callee->TypeInfo)) && typeid(ContinuationTypeAST) != typeid(*(callee->TypeInfo))){
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


	if(typeid(*(callee->TypeInfo))==typeid(FunctionTypeAST)){
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
	}else if(typeid(*(callee->TypeInfo))==typeid(ContinuationTypeAST)){
		shared_ptr<TypeAST> contarg=dynamic_pointer_cast<ContinuationTypeAST>(callee->TypeInfo)->Type;
		if((contarg->GetName()=="void" && args->size()==0) || (args->size()==1 && contarg->GetName()==args->at(0)->TypeInfo->GetName())){
			TypeInfo=make_shared<BasicTypeAST>("void"); //継続へジャンプするから返り値の型とかどうでもいい
			return TypeInfo;
		}else{
			error("引数リストの型と継続の型が一致しません");
		}
	}
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

		for(auto riter=Value->rbegin();riter!=Value->rend();riter++){
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
		for(auto riter=Value->rbegin();riter!=Value->rend();riter++){
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

    for(auto iter=Value->begin();iter!=Value->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

shared_ptr<TypeAST>  ListValExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)
{
    for(auto iter=Value->begin();iter!=Value->end();iter++){
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

    for(auto iter=Value->begin();iter!=Value->end();iter++){
		list_tmp=(*iter)->FindChildFunction();
		result_list.insert(result_list.end(),list_tmp.begin(),list_tmp.end());
    }

    return result_list;
}

shared_ptr<TypeAST>  TupleValExprAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars)
{
	vector<shared_ptr<TypeAST> > typelist;
    for(auto iter=Value->begin();iter!=Value->end();iter++){
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
	for(auto iter=Value->begin();iter!=Value->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
}

bool TupleValExprAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	for(auto iter=Value->begin();iter!=Value->end();iter++){
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

	for(auto iter=Value->begin();iter!=Value->end();iter++){
		temp=(*iter)->GetCallExprList();
		result.insert(result.end(),temp.begin(),temp.end());
	}
	return result;
}

vector<shared_ptr<ExprAST> > TupleValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	vector<shared_ptr<ExprAST> > temp;

	for(auto iter=Value->begin();iter!=Value->end();iter++){
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

void NullValExprAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	bytecodes->push_back(pushnull);
}

vector<shared_ptr<ExprAST> > NullValExprAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}

shared_ptr<TypeAST> ContinuationAST::CheckType(shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo, shared_ptr<vector< pair<string, shared_ptr<TypeAST> > > > CurrentLocalVars)
{
	InternalClosure->CheckType(env,geninfo,CurrentLocalVars);

	return TypeInfo;
}

vector<int> ContinuationAST::FindChildFunction()
{
	InternalClosure->FindChildFunction();

    vector<int> result;result.push_back(InternalClosure->PoolIndex);
    return result;
}

void ContinuationAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	bytecodes->push_back(makecontinuation);
    InternalClosure->Codegen(bytecodes,geninfo);
	//---この時点でスタックトップに継続を欲しがっているクロージャが置かれている
	bytecodes->push_back(invoke);
	bytecodes->push_back(0);
	bytecodes->push_back(0);
}

vector<shared_ptr<ExprAST> > ContinuationAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	result=InternalClosure->GetCallExprList();
	return result;
}

shared_ptr<TypeAST> NewObjectAST::CheckType(shared_ptr<vector<Environment> > env, shared_ptr<CodegenInfo> geninfo, shared_ptr<vector< pair<string, shared_ptr<TypeAST> > > > CurrentLocalVars)
{
	if(typeid(*TypeInfo)!=typeid(ChannelTypeAST)){
		error("newを使用できない型です");
	}
	return TypeInfo;
}

vector<int> NewObjectAST::FindChildFunction()
{
    vector<int> result;
    return result;
}

void NewObjectAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	if(typeid(*TypeInfo)==typeid(ChannelTypeAST)){
		bytecodes->push_back(makechannel);
	}
}

vector< shared_ptr<ExprAST> > NewObjectAST::GetCallExprList()
{
	vector<shared_ptr<ExprAST> > result;
	return result;
}
