﻿#pragma once
#include "common.h"

class CodegenInfo;

TokenValue success(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue program_empty_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue program_addfundef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue program_addvardef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue program_adddatadef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue program_addgroupdef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue intvalexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue doublevalexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue boovalexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue stringvalexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue operator_n_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue function_norettype_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue function_withrettype_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue function_operator_norettype_reduce(shared_ptr<CodegenInfo>, vector<TokenValue>);
TokenValue function_operator_withrettype_reduce(shared_ptr<CodegenInfo>, vector<TokenValue>);
TokenValue parameter_list_empty_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue parameter_list_parameter_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue parameter_list_addparameter_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue parameter_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue variabledef_list_variabledef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue variabledef_list_addvariabledef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue variabledef_nonassignment_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue variabledef_withassignment_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue variabledef_infer_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue statement_list_empty_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue statement_list_addstatement(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue statement_expression_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue statement_list_variabledef_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue expression_primary_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue expression_add_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue expression_paren_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue expression_addparen_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue funcallexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue funcallexpr_parenexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue variableexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue parenexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue returnstatement_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue returnstatement_noexp_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_normal_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_fun_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_chantype_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue type_conttype_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_listtype_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_tupletype_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_list_empty_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_list_type_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue type_vectortype_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue type_list_addtype_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue closureexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue callccexpr_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue callccexpr_void_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
TokenValue closureexpr_rettypeinfer_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue arg_list_empty_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue arg_list_expression_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue arg_list_addexpression_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue tuple_list_expression_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
TokenValue ifstatement_noelse_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue ifstatement_withelse_reduce(shared_ptr<CodegenInfo>,vector<TokenValue>);
TokenValue block_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue while_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue async_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue listvalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue tuplevalexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue datadef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue groupdef_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue dmlist_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue dmlist_add_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue gmlist_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue gmlist_addvoid_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue gmlist_addnonvoid_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue gmlist_addfield_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue dataexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue ialist_empty_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue ialist_add_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue listrefexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue listrefexpr_paren_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
TokenValue datamemberrefexpr_reduce(shared_ptr<CodegenInfo> cgi,vector<TokenValue> values);
TokenValue datamemberrefexpr_paren_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
TokenValue newchanexpr_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
TokenValue newchanexpr_capacity0_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
TokenValue newvectorexpr_reduce(shared_ptr<CodegenInfo> cgi, vector<TokenValue> values);
