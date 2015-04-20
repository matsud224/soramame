#pragma once
#include "common.h"

class CodegenInfo;

TokenValue success(CodegenInfo*,vector<TokenValue>);
TokenValue program_empty_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue program_addfundef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue program_addvardef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue program_adddatadef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue program_addgroupdef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue intvalexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue boovalexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue stringvalexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue operator_n_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue function_norettype_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue function_withrettype_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue parameter_list_empty_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue parameter_list_parameter_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue parameter_list_addparameter_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue parameter_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue variabledef_list_variabledef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue variabledef_list_addvariabledef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue variabledef_nonassignment_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue variabledef_withassignment_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue variabledef_infer_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue statement_list_empty_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue statement_list_addstatement(CodegenInfo*,vector<TokenValue>);
TokenValue statement_expression_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue statement_list_variabledef_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue expression_primary_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue expression_add_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue expression_addparen_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue funcallexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue variableexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue parenexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue returnstatement_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue returnstatement_noexp_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_normal_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_fun_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_listtype_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_tupletype_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_list_empty_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_list_type_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue type_list_addtype_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue closureexpr_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue closureexpr_rettypeinfer_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue arg_list_empty_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue arg_list_expression_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue arg_list_addexpression_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue ifstatement_noelse_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue ifstatement_withelse_reduce(CodegenInfo*,vector<TokenValue>);
TokenValue block_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue while_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue listvalexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue tuplevalexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue datadef_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue groupdef_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue dmlist_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue dmlist_add_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue gmlist_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue gmlist_addvoid_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue gmlist_addnonvoid_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue dataexpr_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue ialist_empty_reduce(CodegenInfo *cgi,vector<TokenValue> values);
TokenValue ialist_add_reduce(CodegenInfo *cgi,vector<TokenValue> values);
