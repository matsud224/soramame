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
#include "statement.h"

shared_ptr<ExprAST> GetInitValue(shared_ptr<TypeAST>  type,shared_ptr<CodegenInfo> cgi);





void ReturnStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	if(Expression!=nullptr){
		Expression->Codegen(bytecodes,geninfo);
		bytecodes->push_back(ret_withvalue);
	}else{
        bytecodes->push_back(ret);
    }
}

void AsyncStatementAST::Codegen(shared_ptr<vector<int> > bytecodes, shared_ptr<CodegenInfo> geninfo)
{
	dynamic_pointer_cast<CallExprAST>(Expression)->IsAsync=true;
	Expression->Codegen(bytecodes,geninfo);
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
		if (FlameBack == 0 && LocalIndex <= 5){
			switch (LocalIndex){
			case 0:
				bytecodes->push_back(storelocal00);
				break;
			case 1:
				bytecodes->push_back(storelocal01);
				break;
			case 2:
				bytecodes->push_back(storelocal02);
				break;
			case 3:
				bytecodes->push_back(storelocal03);
				break;
			case 4:
				bytecodes->push_back(storelocal04);
				break;
			case 5:
				bytecodes->push_back(storelocal05);
				break;
			}
		}
		else{
			bytecodes->push_back(storelocal);
			bytecodes->push_back(FlameBack);
			bytecodes->push_back(LocalIndex);
		}
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
		if(Variable->second==nullptr){
			error("型注釈が必要です");
		}
		InitialValue->TypeInfo=Variable->second;
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



void ReturnStatementAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(Expression==nullptr){return;}
	if(Expression->IsBuilt()==false){
		Expression=dynamic_pointer_cast<UnBuiltExprAST >(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo,CurrentLocalVars);
}

void AsyncStatementAST::CheckType(shared_ptr<vector<Environment> > env,shared_ptr<CodegenInfo> geninfo,shared_ptr<vector< pair<string,shared_ptr<TypeAST> >  > > CurrentLocalVars){
	if(Expression==nullptr){return;}
	if(Expression->IsBuilt()==false){
		Expression=dynamic_pointer_cast<UnBuiltExprAST >(Expression)->BuildAST(geninfo);
	}
	Expression->CheckType(env,geninfo,CurrentLocalVars);
	if(typeid(*Expression)!=typeid(CallExprAST)){
		error("asyncステートメントには関数呼び出しが必要です");
	}
	if(typeid(*(dynamic_pointer_cast<CallExprAST>(Expression)->callee->TypeInfo))==typeid(ContinuationAST)){
		error("継続は別スレッドで再開できません");
	}
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

bool IfStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return Condition->IsCTFEable(cgi,curr_fun_index)&&ThenBody->IsCTFEable(cgi,curr_fun_index)&&(ElseBody==nullptr?true:ElseBody->IsCTFEable(cgi,curr_fun_index));
}

bool WhileStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return Condition->IsCTFEable(cgi,curr_fun_index)&&Body->IsCTFEable(cgi,curr_fun_index);
}

bool ReturnStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return (Expression==nullptr?true:Expression->IsCTFEable(cgi,curr_fun_index));
}

bool AsyncStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return false;
}

bool VariableDefStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return InitialValue==nullptr?true:InitialValue->IsCTFEable(cgi,curr_fun_index);
}

bool ExpressionStatementAST::IsCTFEable(shared_ptr<CodegenInfo> cgi,int curr_fun_index){
	return Expression->IsCTFEable(cgi,curr_fun_index);
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

vector<shared_ptr<ExprAST> > AsyncStatementAST::GetCallExprList()
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
