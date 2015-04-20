#include "parser_actions.h"
#include "common.h"
#include "ast.h"
#include "color_text.h"

TokenValue success(CodegenInfo *cgi,vector<TokenValue> values){
    #ifdef PARSER_DEBUG
		cout<<BG_CYAN"acceptされました"RESET<<endl
    #endif
    return values[0];
}


TokenValue program_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	return Parser::dummy;
}

TokenValue program_addfundef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	cgi->TopLevelFunction.push_back(values[1].function_ast);
	return Parser::dummy;
}

TokenValue program_addvardef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	cgi->TopLevelVariableDef.insert(cgi->TopLevelVariableDef.end(),values[2].variabledef_list->begin(),values[2].variabledef_list->end());
	return Parser::dummy;
}

TokenValue program_adddatadef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	cgi->TopLevelDataDef.push_back(values[1].datadef_ast);
	return Parser::dummy;
}

TokenValue program_addgroupdef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	cgi->TopLevelGroupDef.push_back(values[1].groupdef_ast);
	return Parser::dummy;
}

TokenValue intvalexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new IntValExprAST(values[0].intval);
	return t;
}

TokenValue boovalexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new BoolValExprAST(values[0].boolval);
	return t;
}

TokenValue stringvalexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new StringValExprAST(cgi,values[0].str);
	return t;
}

TokenValue operator_n_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new OperatorAST(values[0].str);
	return t;
}

TokenValue function_norettype_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.function_ast=new FunctionAST(cgi,values[1].str,values[3].parameter_list,new BasicTypeAST("void"),values[6].block_ast);
	return t;
}

TokenValue function_withrettype_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.function_ast=new FunctionAST(cgi,values[1].str,values[3].parameter_list,values[6].type_ast,values[8].block_ast);
	return t;
}

TokenValue parameter_list_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_list=new vector<pair<string,TypeAST*> >();
	return t;
}

TokenValue parameter_list_parameter_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_list=new vector<pair<string,TypeAST*> >();
	t.parameter_list->push_back(*values[0].parameter_ast);
	return t;
}

TokenValue parameter_list_addparameter_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_list=new vector<pair<string,TypeAST*> >(*values[0].parameter_list);
	t.parameter_list->push_back(*values[2].parameter_ast);
	return t;
}

TokenValue parameter_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.parameter_ast=new pair<string,TypeAST*>(values[0].str,values[2].type_ast);
	return t;
}

TokenValue variabledef_list_variabledef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_list=new vector<VariableDefStatementAST*>();
	t.variabledef_list->push_back(values[0].variabledef_ast);
	return t;
}

TokenValue variabledef_list_addvariabledef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_list=new vector<VariableDefStatementAST*>(*values[0].variabledef_list);
	t.variabledef_list->push_back(values[2].variabledef_ast);
	return t;
}

TokenValue variabledef_nonassignment_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_ast=new VariableDefStatementAST(new pair<string,TypeAST*>(values[0].str,values[2].type_ast),NULL);
	return t;
}

TokenValue variabledef_withassignment_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.variabledef_ast=new VariableDefStatementAST(new pair<string,TypeAST*>(values[0].str,values[2].type_ast),new UnBuiltExprAST(values[4].expression_list));
	return t;
}

TokenValue variabledef_infer_reduce(CodegenInfo*,vector<TokenValue> values){
	TokenValue t;
	t.variabledef_ast=new VariableDefStatementAST(new pair<string,TypeAST*>(values[0].str,NULL),new UnBuiltExprAST(values[2].expression_list));
	return t;
}

TokenValue statement_list_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_list=new vector<StatementAST*>();
	return t;
}

TokenValue statement_list_addstatement(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_list=new vector< StatementAST* >(*values[0].statement_list);
	t.statement_list->push_back(values[1].statement_ast);
	return t;
}

TokenValue statement_expression_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=new ExpressionStatementAST(new UnBuiltExprAST(values[0].expression_list));
	return t;
}

TokenValue statement_list_variabledef_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_list=new vector< StatementAST* >(*values[0].statement_list);
	t.statement_list->insert(t.statement_list->end(),values[2].variabledef_list->begin(),values[2].variabledef_list->end());
	return t;
}

TokenValue expression_primary_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=new vector< ExprAST* >();
	t.expression_list->push_back(values[0].expression_ast);
	return t;
}

TokenValue expression_add_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=new vector< ExprAST* >(*values[0].expression_list);
	t.expression_list->push_back(values[1].expression_ast);
	return t;
}

TokenValue expression_addparen_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=new vector< ExprAST* >(*values[0].expression_list);
	t.expression_list->insert(t.expression_list->end(),values[1].expression_list->begin(),values[1].expression_list->end());
	return t;
}

TokenValue funcallexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new CallExprAST(values[0].expression_ast,values[2].arg_exp_list);
	return t;
}

TokenValue variableexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new VariableExprAST(values[0].str);
	return t;
}

TokenValue parenexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_list=values[1].expression_list;
	return t;
}

TokenValue returnstatement_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=new ReturnStatementAST(new UnBuiltExprAST(values[1].expression_list));
	return t;
}

TokenValue returnstatement_noexp_reduce(CodegenInfo*, vector<TokenValue>)
{
	TokenValue t;
	t.statement_ast=new ReturnStatementAST(NULL);
	return t;
}

TokenValue type_normal_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_ast=new BasicTypeAST(values[0].str);
	return t;
}

TokenValue type_fun_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	vector<TypeAST*> *v=new vector<TypeAST*>(*values[2].type_list);
	v->push_back(values[5].type_ast);
	t.type_ast=new FunctionTypeAST(*v);
	return t;
}

TokenValue type_listtype_reduce(CodegenInfo *cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.type_ast=new ListTypeAST(values[1].type_ast);
	return t;
}

TokenValue type_list_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_list=new vector<TypeAST*>();
	return t;
}

TokenValue type_list_type_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_list=new vector<TypeAST*>();
	t.type_list->push_back(values[0].type_ast);
	return t;
}

TokenValue type_list_addtype_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.type_list=new vector<TypeAST*>(*values[0].type_list);
	t.type_list->push_back(values[2].type_ast);
	return t;
}

TokenValue closureexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.expression_ast=new FunctionAST(cgi,"<anonymous>",values[2].parameter_list,values[5].type_ast,values[7].block_ast);
	return t;
}

TokenValue closureexpr_rettypeinfer_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	//返り値を推論する時は返り値の型をNULLにすると関数の型リストに埋め込まれて識別できないので型名としてありえない文字列にした
	t.expression_ast=new FunctionAST(cgi,"<anonymous>",values[2].parameter_list,new BasicTypeAST("!!undefined!!"),values[5].block_ast);
	return t;
}

TokenValue arg_list_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.arg_exp_list=new vector<ExprAST*>();
	return t;
}

TokenValue arg_list_expression_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.arg_exp_list=new vector<ExprAST*>();
	t.arg_exp_list->push_back(new UnBuiltExprAST(values[0].expression_list));
	return t;
}

TokenValue arg_list_addexpression_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.arg_exp_list=new vector<ExprAST*>(*values[0].expression_list);
	t.arg_exp_list->push_back(new UnBuiltExprAST(values[2].expression_list));
	return t;
}

TokenValue ifstatement_noelse_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=new IfStatementAST(new UnBuiltExprAST(values[2].expression_list),values[5].block_ast,new BlockAST(new vector<StatementAST*>()));
	return t;
}

TokenValue ifstatement_withelse_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=new IfStatementAST(new UnBuiltExprAST(values[2].expression_list),values[5].block_ast,values[9].block_ast);
	return t;
}

TokenValue block_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.block_ast=new BlockAST(values[0].statement_list);
	return t;
}

TokenValue while_reduce(CodegenInfo *cgi,vector<TokenValue> values)
{
	TokenValue t;
	t.statement_ast=new WhileStatementAST(new UnBuiltExprAST(values[2].expression_list),values[5].block_ast);
	return t;
}


TokenValue listvalexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values){
	TokenValue t;
	list<ExprAST*> *lst=new list<ExprAST*>();
	lst->assign(values[1].arg_exp_list->begin(),values[1].arg_exp_list->end());
	t.expression_ast=new ListValExprAST(cgi,lst);
	return t;
}

TokenValue type_tupletype_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.type_ast=new TupleTypeAST(*values[1].type_list);
	return t;
}

TokenValue tuplevalexpr_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	list<ExprAST*> *lst=new list<ExprAST*>();
	lst->assign(values[1].arg_exp_list->begin(),values[1].arg_exp_list->end());
	t.expression_ast=new TupleValExprAST(cgi,lst);
	return t;
}

TokenValue datadef_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	values[3].datadef_ast->Name=values[1].str;
	t.datadef_ast=values[3].datadef_ast;
	return t;
}

TokenValue groupdef_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	values[6].groupdef_ast->Name=values[1].str;
	values[6].groupdef_ast->TargetName=values[3].str;
	t.groupdef_ast=values[6].groupdef_ast;
	return t;
}

TokenValue dmlist_empty_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.datadef_ast=new DataDefAST();
	return t;
}

TokenValue dmlist_add_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	values[0].datadef_ast->MemberList.push_back(pair<string,TypeAST*>(values[1].str,values[3].type_ast));
	t.datadef_ast=values[0].datadef_ast;
	return t;
}

TokenValue gmlist_empty_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	t.groupdef_ast=new GroupDefAST();
	return t;
}

TokenValue gmlist_addvoid_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	vector<TypeAST*> args=*(values[4].type_list);
	args.push_back(new BasicTypeAST("void"));
	values[0].groupdef_ast->MemberList.push_back(pair<string,TypeAST*>(values[2].str,new FunctionTypeAST(args)));
	t.groupdef_ast=values[0].groupdef_ast;
	return t;
}

TokenValue gmlist_addnonvoid_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{
	TokenValue t;
	vector<TypeAST*> args=*(values[4].type_list);
	args.push_back(values[7].type_ast);
	values[0].groupdef_ast->MemberList.push_back(pair<string,TypeAST*>(values[2].str,new FunctionTypeAST(args)));
	t.groupdef_ast=values[0].groupdef_ast;
	return t;
}

TokenValue dataexpr_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{

}

TokenValue ialist_empty_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{

}

TokenValue ialist_add_reduce(CodegenInfo* cgi, vector<TokenValue> values)
{

}
