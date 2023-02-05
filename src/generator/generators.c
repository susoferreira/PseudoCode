#include "generators.h"
#include "./sds/sds.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "logger.h"
sds get_identifier_text(struct owl_ref identifier){
    struct parsed_identifier id = parsed_identifier_get(identifier);
    
    char buf[id.length+1];
    buf[id.length]=0x00; //colocando el null byte porque c no lo hace solo (?)
    for(int i=0;i<id.length;i++){
        buf[i]=id.identifier[i];
    }
    return sdsnew(buf);
}
//si, es un copia-pega
sds get_string_text(struct owl_ref identifier){
    struct parsed_string id =parsed_string_get(identifier);
    
    char buf[id.length+1];
    buf[id.length]=0x00; //colocando el null byte porque c no lo hace solo (?)
    for(int i=0;i<id.length;i++){
        buf[i]=id.string[i];
    }
    return sdscat(sdscat(sdsnew("\""),sdsnew(buf)),"\"");
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


    //puede haber más de una variable declarada a la vez
    sds var_names = sdsempty();
    while(!var.var_name.empty){
        const char* var_name = get_identifier_text(var.var_name);
        var_names = sdscatprintf(var_names,"%s, ",var_name);
        var.var_name=owl_next(var.var_name);
    }

    var_names=sdstrim(var_names,", ");// remove trailing comma and whitespace

    struct parsed_variable_type type = parsed_variable_type_get(var.variable_type);

    sds var_declaration;
    sds var_type;
    if(!type.var_type.empty){
        var_type = get_identifier_text(type.var_type);
        var_declaration = sdscat(sdscat(var_type," "),var_names);
    }else{
        sds arr_size = generate_array_size(parsed_array_size_get(parsed_tipo_cadena_get(type.tipo_cadena).array_size));
        var_declaration = sdscatprintf(sdsempty(),"Cadena %s [%s]",
            var_names,
            arr_size);
    }


    if (var.initial_value.empty){

        return sdscatprintf(sdsempty(),"%s;",var_declaration);

    }else{

        return sdscatprintf(sdsempty(),"%s = %s;",var_declaration,generate_expr(var.initial_value));

    }
}

sds generate_variables_block(struct parsed_variables_block var){
    sds result=sdsempty();
    while(!var.variable_declaration.empty){
        
        result = sdscatprintf(result,"%s\n",
            generate_variable_declaration(
                parsed_variable_declaration_get(var.variable_declaration)));
        var.variable_declaration = owl_next(var.variable_declaration);
    }
    return result;
}
sds generate_struct_declaration(struct parsed_struct_declaration st){
    sds variables=sdsempty();
    while(!st.variable_declaration.empty){

        variables = sdscatprintf(variables,"%s\n",generate_variable_declaration(parsed_variable_declaration_get(st.variable_declaration)));
        st.variable_declaration = owl_next(st.variable_declaration);
    }
    return sdscatprintf(sdsempty(),"typedef struct{\n%s}%s;",
        variables,
        get_identifier_text(st.struct_name)
        );
    
}

sds generate_array_size(struct parsed_array_size size){
    if (!size.expr.empty){
        return generate_expr(size.expr);
    }else{
        logger("Rango detectado en el tamaño de un array, se ignora el valor de inicio,\n\
solo están soportados arrays que empiezan en 0\n",LOG_INFO);
        return generate_expr(parsed_size_range_get(size.size_range).end);
    }
}
sds generate_array_declaration(struct parsed_array_declaration arr){
    return sdscatprintf(sdsempty(),"typedef %s %s [%s];",
                        get_identifier_text(arr.array_type),
                        get_identifier_text(arr.array_name),
                        generate_array_size(parsed_array_size_get(arr.array_size))
                        );
}
sds generate_tipos_block(struct parsed_tipos_block tipos){

    sds structs=sdsempty();
    sds arrays=sdsempty();

    if (!tipos.struct_declaration.empty){
        struct  parsed_struct_declaration structure = parsed_struct_declaration_get(tipos.struct_declaration);    
        while(!tipos.struct_declaration.empty){
            structs = sdscatprintf(structs,"%s\n",generate_struct_declaration(structure));
            tipos.struct_declaration = owl_next(tipos.struct_declaration);
        }
    }
    
    if (!tipos.array_declaration.empty){
    struct  parsed_array_declaration array = parsed_array_declaration_get(tipos.array_declaration);    
        while(!tipos.array_declaration.empty){
            arrays = sdscatprintf(arrays,"%s\n",generate_array_declaration(array));
            tipos.array_declaration = owl_next(tipos.array_declaration);
        }
    }
    
    return sdscat(structs,arrays);

}

sds generate_constantes_block(struct parsed_constantes_block var){
    sds result=sdsempty();
    while(!var.variable_declaration.empty){
        
        result = sdscatprintf(result,"const %s\n",
            generate_variable_declaration(
                parsed_variable_declaration_get(var.variable_declaration)));
        var.variable_declaration = owl_next(var.variable_declaration);
    }
    return result;
}

//retorna constantes y tipos como punteros porque en distintos scopes que variables y no pueden retornarse juntos
//tipos y constantes tienen que estar inicializados con sdsempty()
//retorna variables de la manera habitual 
sds generate_variable_declaration_block(struct parsed_variable_declaration_block vars, sds* tipos_out, sds* constantes_out){
    sds variables = sdsempty();
    sds tipos=sdsempty();
    sds constantes=sdsempty();

    while (!vars.constantes_block.empty){
        struct parsed_constantes_block bloque = parsed_constantes_block_get(vars.constantes_block);
        constantes = sdscatprintf(constantes,"%s\n",generate_constantes_block(bloque));
        vars.constantes_block = owl_next(vars.constantes_block);
    }

    while (!vars.tipos_block.empty){
        struct parsed_tipos_block bloque = parsed_tipos_block_get(vars.tipos_block);
        tipos = sdscatprintf(tipos,"%s\n",generate_tipos_block(bloque));
        vars.tipos_block = owl_next(vars.tipos_block);
    }

    while (!vars.variables_block.empty){
        struct parsed_variables_block bloque = parsed_variables_block_get(vars.variables_block);
        variables = sdscatprintf(variables,"%s\n",generate_variables_block(bloque));
        vars.variables_block = owl_next(vars.variables_block);
    }

    *constantes_out = constantes;
    *tipos_out = tipos;
    return variables;
}

sds generate_program_declarations(struct parsed_stmt statement){
    sds program_name = get_identifier_text(statement.program_name);
    //sds program_name = sdsnew(º(statement.program_name).identifier);
    sds constantes=sdsnew("asd");
    sds tipos=sdsnew("fgh");
    sds variables = generate_variable_declaration_block(parsed_variable_declaration_block_get(statement.variable_declaration_block),&tipos,&constantes);
    sds stmt_list = generate_statement_list(parsed_stmt_list_get(statement.stmt_list));
    return sdscatprintf(sdsempty(),"//algoritmo %s\n %s\n%s\nint main(int argc,char* argv[]){\n%s\n%s\n\n\
    LEER();// esta línea se añade automáticamente a todos los programas para que no se cierren al ejecutarlos directamente\n }",
    program_name,
    constantes,
    tipos,
    variables,
    stmt_list
    );
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

sds generate_function_call(struct parsed_expr call){
    if (call.expr.empty){
        return sdscatprintf(sdsempty(),"%s()",generate_expr(call.operand));

    }else{
        sds parameters = sdsempty();
        while(!call.expr.empty){
            parameters = sdscatprintf(parameters,"%s,",generate_expr(call.expr));
            call.expr = owl_next(call.expr);
        }
        parameters = sdstrim(parameters,","); // remove trailing comma
        return sdscatprintf(sdsempty(),"%s(%s)",generate_expr(call.operand),parameters);
    }

}


sds generate_expr(struct owl_ref expr){
    struct parsed_expr exp = parsed_expr_get(expr);
    double number; // a c no le gusta crear variables dentro de switch case y se queja (aunque funciona igual)
    switch(exp.type){
        case PARSED_STRING:
            return sdsnew(get_string_text(exp.string));
            break;
        case PARSED_NUMBER:
            number = parsed_number_get(exp.number).number;
            if (number == (int) number) return sdscatprintf(sdsempty(),"%d", (int) number);
            else return sdscatprintf(sdsempty(),"%f", number);
            break;
        case PARSED_TRUE:
            return sdsnew("true");
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
            return generate_function_call(exp);
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
        {
            char msg[200];
            snprintf(msg,200,"error fatal intentando generar una expresión de tipo %d\n\
            el programa intentará continuar pero el resultado debería ser ignorado",exp.type);
            logger(msg,LOG_ERROR);
            return sdsnew("//Aquí ha ocurrido un error de expresion");

        }
    }
}

sds generate_parameter(bool is_reference, sds var_type, sds var_name){

        if (is_reference){
            return sdscatprintf(sdsempty(),"%s& %s",var_type,var_name);
        }else{
            return sdscatprintf(sdsempty(),"%s %s",var_type,var_name);
        }
}

sds generate_parameter_list(struct parsed_parameter_list par){

    sds result = sdsnew("(");
    while (!par.var_name.empty){
        bool in  = false;
        bool out = false;
        sds var_type = get_identifier_text(par.var_type);
        sds var_name = get_identifier_text(par.var_name);
        if(!par.in.empty){
            in = true;
        }
        if(!par.out.empty){
            out = true;
        }
        
        if(out && !in){
            //creo que con la estructura del parser actual no es posible escribir un programa así pero xd
            logger("watefok lmao una variable de solo salida que te pasa bro",LOG_INFO);
        }

        result = sdscatprintf(result,"%s,", generate_parameter(out,var_type,var_name));
        par.var_name=owl_next(par.var_name);
        par.var_type=owl_next(par.var_type);
        par.in=owl_next(par.in);
        par.out=owl_next(par.out);
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


    sds tipos;
    sds constantes;
    sds variables = generate_variable_declaration_block(parsed_variable_declaration_block_get(func.variable_declaration_block),&tipos,&constantes);


    sds stmt_list = generate_statement_list(parsed_stmt_list_get(func.stmt_list));
    //tipos y constantes tienen un scope distinto a variables así que van fuera de la función
    return sdscatprintf(sdsempty(),"%s\n%s\n%s %s{%s %s}",
    tipos,
    constantes,
    return_type,
    function_header,
    variables,
    stmt_list
    );

}

//the same as generate function but always void  (yes im repeating code)
sds generate_procedure(struct parsed_stmt proc){
    sds function_header = generate_function_header(parsed_function_header_get(proc.function_header));

    sds tipos;
    sds constantes;
    sds variables = generate_variable_declaration_block(parsed_variable_declaration_block_get(proc.variable_declaration_block),&tipos,&constantes);
    sds stmt_list = generate_statement_list(parsed_stmt_list_get(proc.stmt_list));
    return sdscatprintf(sdsempty(),"%s\n%s\nvoid %s{%s %s}",
    tipos,
    constantes,
    function_header,
    variables,
    stmt_list
    );

}

sds generate_assignment(struct parsed_stmt assign){
    char* var_name = get_identifier_text(assign.identifier);
    char* value = generate_expr(assign.expr);
    return sdscatprintf(sdsempty(),"%s = %s;\n",var_name,value);
}

sds generate_field_assignment(struct parsed_stmt assign){
    char* field = get_identifier_text(assign.identifier);
    char* value = generate_expr(assign.expr);
    char* array = generate_expr(assign.array);
    return sdscatprintf(sdsempty(),"%s.%s = %s;\n",array,field,value);
}

sds generate_lookup_assignment(struct parsed_stmt assign){
    char* value = generate_expr(assign.expr);
    char* lookup = generate_expr(assign.lookup);
    char* array = generate_expr(assign.array);
    return sdscatprintf(sdsempty(),"%s[%s] = %s;\n",array,lookup,value);
}

sds generate_if_then(struct parsed_stmt if_then){
    sds cond = generate_expr(if_then.expr);
    sds if_stmt = generate_statement_list(parsed_stmt_list_get(if_then.stmt_list));

    sds result = sdscatprintf(sdsempty(),
    "if (%s){\n\
        %s\
    }\n",cond,if_stmt);

    if(!if_then.else_stmts.empty){

        char* else_stmt = generate_statement_list(parsed_stmt_list_get(if_then.else_stmts));
        result=sdscatprintf(result,"else {%s\n}\n",else_stmt);
    }
    return result;
}

sds generate_while(struct parsed_stmt loop){
    char* cond = generate_expr(loop.expr);
    char* stmt = generate_statement_list(parsed_stmt_list_get(loop.stmt_list));
    return sdscatprintf(sdsempty(),"while (%s) {%s}",cond,stmt);
}

sds generate_do_while(struct parsed_stmt loop){
    sds cond = generate_expr(loop.expr);
    sds stmt = generate_statement_list(parsed_stmt_list_get(loop.stmt_list));
    return sdscatprintf(sdsempty(),"do{%s}while(%s);",stmt,cond);
}

sds generate_for(struct parsed_stmt loop){
    char* stop = generate_expr(loop.expr);
    char* var = get_identifier_text(loop.var);
    char* value = generate_expr(loop.value);
    char* step;

    if (!loop.step.empty){
        step = generate_expr(loop.step);
    }
    else{
        step = "1";
    }

    char* stmt = generate_statement_list(parsed_stmt_list_get(loop.stmt_list));

    return sdscatprintf(sdsempty(),"for(%s = %s;%s <= %s;%s=%s+%s)\n{\n%s\n}\n",
                        var,value,var,stop,var,var,step,stmt);
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
    return result;
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
        case PARSED_FOR_:
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
        case PARSED_DO_WHILE:
            return generate_do_while(statement);
        case PARSED_EXPR:
            return sdscat(generate_expr(statement.expr),";\n");
        default:
        {
            char msg[200];
            snprintf(msg, 200,"error fatal intentando parsear un statement de tipo %d\n\
            el programa intentará continuar pero el resultado debería ser ignorado", statement.type);
            logger(msg,LOG_ERROR);
            return sdsnew("//Aquí ha ocurrido un error de statement");
        }

    }
}

sds generate_statement_list(struct parsed_stmt_list stmt_list){
    sds result = sdsempty();
    while (!stmt_list.stmt.empty) {
        struct parsed_stmt statement = parsed_stmt_get(stmt_list.stmt);
        result=sdscatprintf(result,"%s",generate_statement_code(statement));        
        stmt_list.stmt= owl_next(stmt_list.stmt);
    }
        return result;
}