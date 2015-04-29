#include "parser_actions.h"
#include "common.h"
#include "ast.h"
#include "color_text.h"
#include <memory>

TokenValue success(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values){
    #ifdef PARSER_DEBUG
		cout<<BG_CYAN"acceptされました"<<RESET<<endl;
    #endif
    return values[0];
}


TokenValue program_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	return Parser::dummy;
}

TokenValue program_addfundef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	cgi->TopLevelFunction.push_back(values[1].function_ast);
	return Parser::dummy;
}

TokenValue program_addvardef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	cgi->TopLevelVariableDef.insert(cgi->TopLevelVariableDef.end(),values[2].variabledef_list->begin(),values[2].variabledef_list->end());
	return Parser::dummy;
}

TokenValue program_adddatadef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	cgi->TopLevelDataDef.push_back(values[1].datadef_ast);
	return Parser::dummy;
}

TokenValue program_addgroupdef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	cgi->TopLevelGroupDef.push_back(values[1].groupdef_ast);
	return Parser::dummy;
}

TokenValue intvalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<IntValExprAST>(values[0].intval);
	return t;
}

TokenValue doublevalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<DoubleValExprAST>(cgi,values[0].doubleval);
	return t;
}

TokenValue boovalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<BoolValExprAST>(values[0].boolval);
	return t;
}

TokenValue stringvalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<StringValExprAST>(cgi,values[0].str);
	return t;
}

TokenValue operator_n_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<OperatorAST>(values[0].str);
	return t;
}

TokenValue function_norettype_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.function_ast=make_shared<FunctionAST>(cgi,values[1].str,values[3].parameter_list,make_shared<BasicTypeAST>("void"),values[6].block_ast);
	return t;
}

TokenValue function_withrettype_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.function_ast=make_shared<FunctionAST>(cgi,values[1].str,values[3].parameter_list,values[6].type_ast,values[8].block_ast);
	return t;
}

TokenValue parameter_list_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_list=make_shared<vector<pair<string,shared_ptr<TypeAST> > > >();
	return t;
}

TokenValue parameter_list_parameter_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_list=make_shared<vector<pair<string,shared_ptr<TypeAST> > > >();
	t.parameter_list->push_back(*values[0].parameter_ast);
	return t;
}

TokenValue parameter_list_addparameter_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_list=make_shared<vector<pair<string,shared_ptr<TypeAST> > > >(*values[0].parameter_list);
	t.parameter_list->push_back(*values[2].parameter_ast);
	return t;
}

TokenValue parameter_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_ast=make_shared<pair<string,shared_ptr<TypeAST> > >(values[0].str,values[2].type_ast);
	return t;
}

TokenValue variabledef_list_variabledef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_list=make_shared<vector<shared_ptr<VariableDefStatementAST> > >();
	t.variabledef_list->push_back(values[0].variabledef_ast);
	return t;
}

TokenValue variabledef_list_addvariabledef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_list=make_shared<vector<shared_ptr<VariableDefStatementAST> > >(*values[0].variabledef_list);
	t.variabledef_list->push_back(values[2].variabledef_ast);
	return t;
}

TokenValue variabledef_nonassignment_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_ast=make_shared<VariableDefStatementAST>(make_shared<pair<string,shared_ptr<TypeAST> > >(values[0].str,values[2].type_ast),nullptr);
	return t;
}

TokenValue variabledef_withassignment_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_ast=make_shared<VariableDefStatementAST>(make_shared<pair<string,shared_ptr<TypeAST> > >(values[0].str,values[2].type_ast),make_shared<UnBuiltExprAST>(values[4].expression_list));
	return t;
}

TokenValue variabledef_infer_reduce(shared_ptr<CodegenInfo>,vector<TokenValue> values){
	TokenValue t;
	t.variabledef_ast=make_shared<VariableDefStatementAST>(make_shared<pair<string,shared_ptr<TypeAST> > >(values[0].str,nullptr),make_shared<UnBuiltExprAST>(values[2].expression_list));
	return t;
}

TokenValue statement_list_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_list=make_shared<vector<shared_ptr<StatementAST> > >();
	return t;
}

TokenValue statement_list_addstatement(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_list=make_shared<vector<shared_ptr<StatementAST> > >(*values[0].statement_list);
	t.statement_list->push_back(values[1].statement_ast);
	return t;
}

TokenValue statement_expression_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=make_shared<ExpressionStatementAST>(make_shared<UnBuiltExprAST>(values[0].expression_list));
	return t;
}

TokenValue statement_list_variabledef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_list=make_shared<vector<shared_ptr<StatementAST> > >(*values[0].statement_list);
	t.statement_list->insert(t.statement_list->end(),values[2].variabledef_list->begin(),values[2].variabledef_list->end());
	return t;
}

TokenValue expression_primary_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=make_shared<vector< shared_ptr<ExprAST> > >();
	t.expression_list->push_back(values[0].expression_ast);
	return t;
}

TokenValue expression_add_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=make_shared<vector< shared_ptr<ExprAST> > >(*values[0].expression_list);
	t.expression_list->push_back(values[1].expression_ast);
	return t;
}


TokenValue expression_addparen_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=make_shared<vector< shared_ptr<ExprAST> > >(*values[0].expression_list);
	t.expression_list->insert(t.expression_list->end(),values[1].expression_list->begin(),values[1].expression_list->end());
	return t;
}

TokenValue funcallexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<CallExprAST>(values[0].expression_ast,values[2].arg_exp_list);
	return t;
}

TokenValue variableexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<VariableExprAST>(values[0].str);
	return t;
}

TokenValue parenexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=values[1].expression_list;
	return t;
}

TokenValue returnstatement_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=make_shared<ReturnStatementAST>(make_shared<UnBuiltExprAST>(values[1].expression_list));
	return t;
}

TokenValue returnstatement_noexp_reduce(shared_ptr<CodegenInfo>, vector<TokenValue>)
{
	TokenValue t;
	t.statement_ast=make_shared<ReturnStatementAST>(nullptr);
	return t;
}

TokenValue type_normal_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_ast=make_shared<BasicTypeAST>(values[0].str);
	return t;
}

TokenValue type_fun_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	shared_ptr<vector<shared_ptr<TypeAST> > > v=make_shared<vector<shared_ptr<TypeAST> > >(*values[2].type_list);
	v->push_back(values[5].type_ast);
	t.type_ast=make_shared<FunctionTypeAST>(*v);
	return t;
}

TokenValue type_listtype_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.type_ast=make_shared<ListTypeAST>(values[1].type_ast);
	return t;
}

TokenValue type_list_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_list=make_shared<vector<shared_ptr<TypeAST> > >();
	return t;
}

TokenValue type_list_type_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_list=make_shared<vector<shared_ptr<TypeAST> > >();
	t.type_list->push_back(values[0].type_ast);
	return t;
}

TokenValue type_list_addtype_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_list=make_shared<vector<shared_ptr<TypeAST> > >(*values[0].type_list);
	t.type_list->push_back(values[2].type_ast);
	return t;
}

TokenValue closureexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<FunctionAST>(cgi,"<anonymous>",values[2].parameter_list,values[5].type_ast,values[7].block_ast);
	return t;
}

TokenValue closureexpr_rettypeinfer_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	//返り値を推論する時は返り値の型をnullptrにすると関数の型リストに埋め込まれて識別できないので型名としてありえない文字列にした
	t.expression_ast=make_shared<FunctionAST>(cgi,"<anonymous>",values[2].parameter_list,make_shared<BasicTypeAST>("!!undefined!!"),values[5].block_ast);
	return t;
}

TokenValue arg_list_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.arg_exp_list=make_shared<vector<shared_ptr<ExprAST> > >();
	return t;
}

TokenValue arg_list_expression_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.arg_exp_list=make_shared<vector<shared_ptr<ExprAST> > >();
	t.arg_exp_list->push_back(make_shared<UnBuiltExprAST>(values[0].expression_list));
	return t;
}

TokenValue arg_list_addexpression_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.arg_exp_list=make_shared<vector<shared_ptr<ExprAST> > >(*values[0].arg_exp_list);
	t.arg_exp_list->push_back(make_shared<UnBuiltExprAST>(values[2].expression_list));
	return t;
}

TokenValue ifstatement_noelse_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=make_shared<IfStatementAST>(make_shared<UnBuiltExprAST>(values[2].expression_list),values[5].block_ast,make_shared<BlockAST>(make_shared<vector<shared_ptr<StatementAST> > >()));
	return t;
}

TokenValue ifstatement_withelse_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=make_shared<IfStatementAST>(make_shared<UnBuiltExprAST>(values[2].expression_list),values[5].block_ast,values[9].block_ast);
	return t;
}

TokenValue block_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.block_ast=make_shared<BlockAST>(values[0].statement_list);
	return t;
}

TokenValue while_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=make_shared<WhileStatementAST>(make_shared<UnBuiltExprAST>(values[2].expression_list),values[5].block_ast);
	return t;
}


TokenValue listvalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values){
	TokenValue t;
	auto lst=make_shared<list<shared_ptr<ExprAST> > >();
	lst->assign(values[1].arg_exp_list->begin(),values[1].arg_exp_list->end());
	t.expression_ast=make_shared<ListValExprAST>(cgi,lst);
	return t;
}

TokenValue type_tupletype_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.type_ast=make_shared<TupleTypeAST>(*values[1].type_list);
	return t;
}

TokenValue tuplevalexpr_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	auto lst=make_shared<list<shared_ptr<ExprAST> > >();
	lst->assign(values[1].arg_exp_list->begin(),values[1].arg_exp_list->end());
	t.expression_ast=make_shared<TupleValExprAST>(cgi,lst);
	return t;
}

TokenValue datadef_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	values[3].datadef_ast->Name=values[1].str;
	t.datadef_ast=values[3].datadef_ast;
	return t;
}

TokenValue groupdef_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	values[6].groupdef_ast->Name=values[1].str;
	values[6].groupdef_ast->TargetName=values[3].str;
	t.groupdef_ast=values[6].groupdef_ast;
	return t;
}

TokenValue dmlist_empty_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.datadef_ast=make_shared<DataDefAST>();
	return t;
}

TokenValue dmlist_add_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	values[0].datadef_ast->MemberList.push_back(pair<string,shared_ptr<TypeAST> >(values[1].str,values[3].type_ast));
	t.datadef_ast=values[0].datadef_ast;
	return t;
}

TokenValue gmlist_empty_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.groupdef_ast=make_shared<GroupDefAST>();
	return t;
}

TokenValue gmlist_addvoid_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	vector<shared_ptr<TypeAST> > args=*(values[4].type_list);
	args.push_back(make_shared<BasicTypeAST>("void"));
	values[0].groupdef_ast->MemberList.push_back(pair<string,shared_ptr<TypeAST> >(values[2].str,make_shared<FunctionTypeAST>(args)));
	t.groupdef_ast=values[0].groupdef_ast;
	return t;
}

TokenValue gmlist_addnonvoid_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	vector<shared_ptr<TypeAST> > args=*(values[4].type_list);
	args.push_back(values[7].type_ast);
	values[0].groupdef_ast->MemberList.push_back(pair<string,shared_ptr<TypeAST> >(values[2].str,make_shared<FunctionTypeAST>(args)));
	t.groupdef_ast=values[0].groupdef_ast;
	return t;
}

TokenValue gmlist_addfield_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	vector<shared_ptr<TypeAST> > args=*(values[4].type_list);
	args.push_back(make_shared<BasicTypeAST>("void"));
	values[0].groupdef_ast->MemberList.push_back(pair<string,shared_ptr<TypeAST> >(values[1].str,values[3].type_ast));
	t.groupdef_ast=values[0].groupdef_ast;
	return t;
}

TokenValue dataexpr_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<DataValExprAST>(values[0].str,values[2].datainitval_list);
	return t;
}

TokenValue ialist_empty_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.datainitval_list=make_shared<vector< pair<string,shared_ptr<ExprAST> > > >(); //parameterと型が一緒なので使わせてもらう
	return t;
}

TokenValue ialist_add_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	shared_ptr<vector< pair<string,shared_ptr<ExprAST> > > > m=make_shared<vector< pair<string,shared_ptr<ExprAST> > > >(*(values[0].datainitval_list)); //parameterと型が一緒なので使わせてもらう
	m->push_back(pair<string,shared_ptr<ExprAST> >(values[1].str,make_shared<UnBuiltExprAST>(values[3].expression_list)));
	t.datainitval_list=m;
	return t;
}

TokenValue listrefexpr_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<ListRefExprAST>(values[0].expression_ast,make_shared<UnBuiltExprAST>(values[2].expression_list)); //parameterと型が一緒なので使わせてもらう
	return t;
}

TokenValue datamemberrefexpr_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=make_shared<DataMemberRefExprAST>(values[0].expression_ast,values[2].str); //parameterと型が一緒なので使わせてもらう
	return t;
}
