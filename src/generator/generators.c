#include "generators.h"
#include "./sds/sds.h"
#include <stdlib.h>
#include <string.h>
#include "parser.h"

sds get_identifier_text(struct owl_ref identifier){
    struct parsed_identifier id = parsed_identifier_get(identifier);
    
    char buf[id.length];
    for(int i=0;i<id.length;i++){
        buf[i]=id.identifier[i];
    }
    return sdsnew(buf);
}


sds generate_break(struct parsed_stmt statement){
    return sdsnew("break;");
}

sds generate_continue(struct parsed_stmt statement){
    return sdsnew("continue;");
}

sds generate_return(struct parsed_stmt statement){
    sds expr = generate_expr(statement.expr);
    return sdscatprintf(sdsempty(),"return %s;",expr);
}






sds generate_variable_declaration(struct parsed_variable_declaration var){
    const char* var_type = get_identifier_text(var.var_type);
    const char* var_name = get_identifier_text(var.var_name);
    if (var.initial_value.empty){
        return sdscatprintf(sdsempty(),"%s %s;",var_type,var_name);
    }else{
    char* var_initial_value = generate_expr(var.initial_value);

        return sdscatprintf(sdsempty(),"%s %s = %s;",var_type,var_name,var_initial_value);
    }
}

sds generate_variables_block(struct parsed_variables_block var){
    //si, no hace nada excepto desempaquetar los datos
    return generate_variable_declaration(parsed_variable_declaration_get(var.variable_declaration));
}

sds generate_tipos_block(struct parsed_tipos_block tipos){
    sds result = sdsempty();

    if (!tipos.struct_declaration.empty){
        char* start="typedef struct {\n";
        struct  parsed_struct_declaration structure = parsed_struct_declaration_get(tipos.struct_declaration);
        result = sdscatprintf(start,"%s\n",get_identifier_text(structure.struct_name));
    
        while(!tipos.struct_declaration.empty){
            result = sdscatprintf(result,"%s\n",generate_variable_declaration(parsed_variable_declaration_get(structure.variable_declaration)));
            tipos.struct_declaration = owl_next(tipos.struct_declaration);
        }
        result = sdscatprintf(result,"\n}%s",get_identifier_text(structure.struct_name));
    }
    return result;
}

sds generate_variable_declaration_block(struct parsed_variable_declaration_block vars){
    sds result=sdsempty();

    int count = 0; 
    // usado para comprobar que no haya más de dos declaraciones
    unsigned int end_tipos=0;
    unsigned int start_variables=-1;
    //usado para comprobar que tipos va antes que variables

    while (!vars.tipos_block.empty){
        count++;
        struct parsed_tipos_block bloque = parsed_tipos_block_get(vars.tipos_block);
        end_tipos = bloque.range.end;
        result = sdscatprintf(result,"%s\n",generate_tipos_block(bloque));
        vars.tipos_block = owl_next(vars.tipos_block);
    }

    while (!vars.variables_block.empty){
        count++;
        struct parsed_variables_block bloque = parsed_variables_block_get(vars.variables_block);
        start_variables = bloque.range.start;
        result = sdscatprintf(result,"%s\n",generate_variables_block(bloque));
        vars.variables_block = owl_next(vars.variables_block);
    }

    if (count > 2 || end_tipos > start_variables ){
        printf("AVISO: En pseudocódigo no se puede tener más de una declaración de cada tipo, y 'TIPOS' debe ir antes que 'VARIABLES'");
    }
    return result;
}

sds generate_program_declarations(struct parsed_stmt statement){
    sds program_name = get_identifier_text(statement.program_name);
    //sds program_name = sdsnew(º(statement.program_name).identifier);
    sds var = generate_variable_declaration_block(parsed_variable_declaration_block_get(statement.variable_declaration_block));
    sds stmt_list = generate_statement_list(parsed_stmt_list_get(statement.stmt_list));
    return sdscatprintf(sdsempty(),"//algoritmo %s\n %s\n%s",program_name,var,stmt_list);
}


sds generate_array_values(struct parsed_expr array){
    //AÑADIR ARRAY ASSIGNMENTS, EL COSO ESTABA MAL
    sds result = sdsnew("{");
    while(!array.expr.empty){
        result = sdscatprintf(result,"%s,",generate_expr(array.expr));
        array.expr = owl_next(array.expr);
    }
    result=sdscat(result,"}");
    return result;
}

sds generate_expr(struct owl_ref expr){
    struct parsed_expr exp = parsed_expr_get(expr);

    switch(exp.type){
        case PARSED_STRING:
            return sdsnew(parsed_string_get(exp.string).string);
            break;
        case PARSED_NUMBER:
            return sdscatprintf(sdsempty(),"%f",parsed_number_get(exp.number).number);
            break;
        case PARSED_TRUE:
            return sdsnew("true"); //hay que definir true y false con #define
            break;
        case PARSED_FALSE:
            return sdsnew("false");
            break;
        case PARSED_VARIABLE:
            return sdsnew(get_identifier_text(exp.identifier));
            break;
        case PARSED_PARENS:
            // que pasa con parentesis vacios ?
            return generate_expr(exp.expr);
            break;
        case PARSED_ARRAY_VALUES:
            return generate_array_values(exp);
            break;
        case PARSED_CALL:
            return generate_expr(exp.expr);
            break;
        case PARSED_LOOKUP:
            return generate_expr(exp.expr);
            break;
        case PARSED_FIELD:
            return sdscat(sdscat(generate_expr(exp.left),"."),generate_expr(exp.right));
            break;
        case PARSED_NEGATE:
            return sdscat(sdsnew("-"),generate_expr(exp.operand));
            break;
        case PARSED_NOT:
            return sdscat(sdsnew("!"),generate_expr(exp.operand));
            break;
        case PARSED_TIMES:
            return sdscat(sdscat(generate_expr(exp.left),"*"),generate_expr(exp.right));
            break;
        case PARSED_DIVIDED_BY:
            return sdscat(sdscat(generate_expr(exp.left),"/"),generate_expr(exp.right));
            break;
        case PARSED_MODULUS:
            return sdscat(sdscat(generate_expr(exp.left),"%"),generate_expr(exp.right));
            break;
        case PARSED_PLUS:
            return sdscat(sdscat(generate_expr(exp.left),"+"),generate_expr(exp.right));
            break;
        case PARSED_MINUS:
            return sdscat(sdscat(generate_expr(exp.left),"-"),generate_expr(exp.right));
            break;
        case PARSED_EQUAL_TO:
            return sdscat(sdscat(generate_expr(exp.left),"=="),generate_expr(exp.right));
            break;
        case PARSED_NOT_EQUAL_TO:
            return sdscat(sdscat(generate_expr(exp.left),"!="),generate_expr(exp.right));
            break;
        case PARSED_LESS_THAN:
            return sdscat(sdscat(generate_expr(exp.left),"<"),generate_expr(exp.right));
            break;
        case PARSED_GREATER_THAN:
            return sdscat(sdscat(generate_expr(exp.left),">"),generate_expr(exp.right));
            break;
        case PARSED_LESS_THAN_OR_EQUAL_TO:
            return sdscat(sdscat(generate_expr(exp.left),"<="),generate_expr(exp.right));
            break;
        case PARSED_GREATER_THAN_OR_EQUAL_TO:
            return sdscat(sdscat(generate_expr(exp.left),">="),generate_expr(exp.right));
            break;
        case PARSED_AND:
            return sdscat(sdscat(generate_expr(exp.left),"&&"),generate_expr(exp.right));
            break;
        case PARSED_OR:
            return sdscat(sdscat(generate_expr(exp.left),"||"),generate_expr(exp.right));
            break;
        default:
            printf("error intentando generar una expresión de tipo %d",exp.type);
            exit(-1);
    }
}

sds generate_parameter(sds e_s, sds var_type, sds var_name){
    if (strcmp(e_s,"E/S")==0){

        return sdscatprintf(sdsempty(),"%s& %s",var_type,var_name);

    }else if(strcmp(e_s,"E")==0){

        return sdscatprintf(sdsempty(),"%s %s",var_type,var_name);

    }else{
        printf("ERROR en parametros de una función:posibles valores son 'E/S' o 'E', %s no es un valor aceptado",
        e_s);
        exit(-1);
    }
}

sds generate_parameter_list(struct parsed_parameter_list par){

    sds result = sdsnew("(");
    while (!par.var_name.empty){
        sds e_s = get_identifier_text(par.E_S);
        sds var_type = get_identifier_text(par.var_type);
        sds var_name = get_identifier_text(par.var_name);
        result = sdscatprintf(result,"%s,", generate_parameter(e_s,var_type,var_name));
        par.var_name=owl_next(par.var_name);
        par.var_type=owl_next(par.var_type);
        par.E_S=owl_next(par.E_S);
    }
        result=sdstrim(result,",");// remove trailing comma
        return sdscat(result,")");
}

sds generate_function_header(struct parsed_function_header header){
    sds name = sdsnew(get_identifier_text(header.name));
    sds parameters = generate_parameter_list(parsed_parameter_list_get(header.parameters));
    return sdscatprintf(sdsempty(),"%s %s",name,parameters);
}

sds generate_function(struct parsed_stmt func){
    sds function_header = generate_function_header(parsed_function_header_get(func.function_header));
    sds return_type = sdsnew(get_identifier_text(func.return_type));
    sds variable_declaration_block = generate_variable_declaration_block(parsed_variable_declaration_block_get(func.variable_declaration_block));
    sds stmt_list = generate_statement_list(parsed_stmt_list_get(func.stmt_list));
    return sdscatprintf(sdsempty(),"%s %s{%s %s}",return_type,function_header,variable_declaration_block,stmt_list);

}

//the same as generate function but always void  (yes im repeating code)
sds generate_procedure(struct parsed_stmt proc){
    sds function_header = generate_function_header(parsed_function_header_get(proc.function_header));
    sds variable_declaration_block = generate_variable_declaration_block(parsed_variable_declaration_block_get(proc.variable_declaration_block));
    sds stmt_list = generate_statement_list(parsed_stmt_list_get(proc.stmt_list));
    return sdscatprintf(sdsempty(),"void %s{%s %s}",function_header,variable_declaration_block,stmt_list);

}

sds generate_assignment(struct parsed_stmt assign){
    char* var_name = get_identifier_text(assign.identifier);
    char* value = generate_expr(assign.expr);
    return sdscatprintf(sdsempty(),"%s = %s\n",var_name,value);
}

sds generate_field_assignment(struct parsed_stmt assign){
    char* field = get_identifier_text(assign.identifier);
    char* value = generate_expr(assign.expr);
    char* array = generate_expr(assign.array);
    return sdscatprintf(sdsempty(),"%s.%s = %s\n",array,field,value);
}

sds generate_lookup_assignment(struct parsed_stmt assign){
    char* value = generate_expr(assign.expr);
    char* lookup = generate_expr(assign.lookup);
    char* array = generate_expr(assign.array);
    return sdscatprintf(sdsempty(),"%s[%s] = %s\n",array,lookup,value);
}

sds generate_if_then(struct parsed_stmt if_then){
    sds cond = generate_expr(if_then.expr);
    sds if_stmt = generate_statement_list(parsed_stmt_list_get(if_then.stmt_list));

    sds result = sdscatprintf("if (%s){%s}\n",cond,if_stmt);

    if(!if_then.else_stmts.empty){

        char* else_stmt = generate_statement_list(parsed_stmt_list_get(if_then.else_stmts));
        result=sdscatprintf(result,"else {%s}\n",else_stmt);
    }
}

sds generate_while(struct parsed_stmt loop){
    char* cond = generate_expr(loop.expr);
    char* stmt = generate_statement_list(parsed_stmt_list_get(loop.stmt_list));
    return sdscatprintf(sdsempty(),"while (%s) {%s}",cond,stmt);
}

sds generate_for(struct parsed_stmt loop){
    char* cond = generate_expr(loop.expr);
    char* var = get_identifier_text(loop.var);
    char* value = generate_expr(loop.expr);
    char* step = generate_expr(loop.step);
    char* stmt = generate_statement_list(parsed_stmt_list_get(loop.stmt_list));

    return sdscatprintf(sdsempty(),"for(%s = %s;%s;%s=%s+%s)\n{%s}",
                        var,value,cond,var,var,step,stmt);
}

sds generate_case(struct parsed_stmt stmt ){
    char* var = generate_expr(stmt.expr);

    sds statements=sdsempty();
    while (!stmt.val.empty){
        char* val = generate_expr(stmt.val);
        char* case_stmt = generate_statement_list(parsed_stmt_list_get(stmt.stmt_list));

        statements=sdscatprintf(statements,"case %s:\n%s\nbreak;\n",val,case_stmt);

        stmt.val = owl_next(stmt.val);
        stmt.stmt_list = owl_next(stmt.stmt_list);
    }

    if (!stmt.case_default.empty){

        char* default_stmt = generate_statement_list(parsed_stmt_list_get(stmt.case_default));
        statements = sdscatprintf(statements,"default:\n%s\nbreak;",default_stmt);
    }


    sds result = sdscatprintf(sdsempty(),"switch (%s){%s}",var,statements);
    
}

char * generate_statement_code(struct parsed_stmt statement){
    
    switch (statement.type){
        case PARSED_NEWLINE:
            return sdsempty();
            break; //ignore newlines
        case PARSED_PROGRAM_DECLARATIONS:
            return generate_program_declarations(statement);
            break;
        case PARSED_FUNCTION:
            return generate_function(statement);
            break;
        case PARSED_PROCEDURE:
            return generate_procedure(statement);
            break;
        case PARSED_ASSIGNMENT:
            return generate_assignment(statement);
            break;
        case PARSED_FIELD_ASSIGNMENT:
            return generate_field_assignment(statement);
            break;
        case PARSED_LOOKUP_ASSIGNMENT:
            return generate_lookup_assignment(statement);
            break;
        case PARSED_RETURN:
            return generate_return(statement);
            break;
        case PARSED_IF_THEN:
            return generate_if_then(statement);
            break;
        case PARSED_WHILE:
            return generate_while(statement);
            break;
        case PARSED_FOR:
            return generate_for(statement);
            break;
        case PARSED_CASE:
            return generate_case(statement);
            break;
        case PARSED_BREAK:
            return generate_break(statement);
            break;
        case PARSED_CONTINUE:
            return generate_continue(statement);
        case PARSED_EXPR:
            return generate_expr(statement.expr);
        default:
            printf("error intentando parsear un statement de tipo %d",statement.type);
            exit(-1);

    }
}

sds generate_statement_list(struct parsed_stmt_list stmt_list){
    sds result = sdsempty();
    while (!stmt_list.stmt.empty) {
        struct parsed_stmt statement = parsed_stmt_get(stmt_list.stmt);
        result=sdscatprintf(result,"%s",generate_statement_code(statement));        
        stmt_list.stmt= owl_next(stmt_list.stmt);
        printf("resultado por el momento:\n%s\n\n\n\n",result);
    }
        return result;
}