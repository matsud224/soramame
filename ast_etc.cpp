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
#include "ast_etc.h"

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

	}else if(typeid(ListTypeAST)==typeid(*type)){
		auto emptylist=make_shared< list< shared_ptr<ExprAST> > >();
		auto emptylistval=make_shared< ListValExprAST >(cgi,emptylist);
		return emptylistval;
	}else if(typeid(TupleTypeAST)==typeid(*type)){
		auto initlist=make_shared<list<shared_ptr<ExprAST> > >();
		auto tt=dynamic_pointer_cast<TupleTypeAST>(type);
		for(auto iter=tt->ContainTypeList.begin();iter!=tt->ContainTypeList.end();iter++){
			initlist->push_back(GetInitValue((*iter),cgi));
		}
		return make_shared<TupleValExprAST>(cgi,initlist);
	}

	return make_shared<NullValExprAST>(type);
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

bool BlockAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	vector<shared_ptr<StatementAST> >::iterator iter;
	for(iter=Body->begin();iter!=Body->end();iter++){
		if((*iter)->IsCTFEable(cgi,curr_fun_index)==false){
			return false;
		}
	}
	return true;
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
