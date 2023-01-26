#pragma once
#include "./sds/sds.h"
#include "parser.h"

sds generate_break(struct parsed_stmt statement);
sds generate_continue(struct parsed_stmt statement);
sds generate_return(struct parsed_stmt statement);
sds generate_variable_declaration(struct parsed_variable_declaration var);
sds generate_variables_block(struct parsed_variables_block var);
sds generate_array_size(struct parsed_array_size size);
sds generate_tipos_block(struct parsed_tipos_block tipos);
sds generate_variable_declaration_block(struct parsed_variable_declaration_block vars, sds* tipos, sds* constantes);
sds generate_program_declarations(struct parsed_stmt statement);
sds generate_array_values(struct parsed_expr array);
sds generate_expr(struct owl_ref expr);
sds generate_parameter(bool is_reference, sds var_type, sds var_name);
sds generate_parameter_list(struct parsed_parameter_list par);
sds generate_function_header(struct parsed_function_header header);
sds generate_function(struct parsed_stmt func);
sds generate_procedure(struct parsed_stmt proc);
sds generate_assignment(struct parsed_stmt assign);
sds generate_field_assignment(struct parsed_stmt assign);
sds generate_lookup_assignment(struct parsed_stmt assign);
sds generate_if_then(struct parsed_stmt if_then);
sds generate_while(struct parsed_stmt loop);
sds generate_for(struct parsed_stmt loop);
sds generate_case(struct parsed_stmt stmt);
char *generate_statement_code(struct parsed_stmt statement);
sds generate_statement_list(struct parsed_stmt_list stmt_list);
