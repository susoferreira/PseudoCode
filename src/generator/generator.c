

#include "generators.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "./sds/sds.h"
#include "preprocessor.h"
#include <libgen.h>
#include "utils.h"
#include "logger.h"
#include "config.h"
#include "generator.h"
#define OWL_PARSER_IMPLEMENTATION
#include "parser.h"

/* this file contains the functions used to preprocess and parse pseudocode into c++*/
/*should be used as main 'library' for the platforms to interface with the code*/



/*logs parsing errors*/
static bool handle_parse_errors(struct owl_tree* tree){
    char msg[200];

    switch (tree->error) {
        case ERROR_NONE:
            return false;
        case ERROR_INVALID_FILE:
            logger("fichero no válido", LOG_ERROR);
            return true;
        case ERROR_INVALID_OPTIONS:
            logger("opciones del parser no válidas\n",LOG_ERROR);
            return true;
        case ERROR_INVALID_TOKEN:
            snprintf(msg,200, "invalid token '%.*s'\n", (int)(tree->error_range.end - tree->error_range.start), tree->string + tree->error_range.start);
            log_range(msg, LOG_ERROR,tree ->error_range.start,tree->error_range.end);
            return true;
        case ERROR_UNEXPECTED_TOKEN:
            snprintf(msg,200, "unexpected token '%.*s'\n", (int)(tree->error_range.end - tree->error_range.start), tree->string + tree->error_range.start);
            log_range(msg, LOG_ERROR, tree->error_range.start, tree->error_range.end);
            return true;
        case ERROR_MORE_INPUT_NEEDED:
            logger("more input needed\n",LOG_ERROR);
            return true;
        default:
            logger("error desconocido al parsear", LOG_ERROR);
            return true;
    }
}

/*applies parser,preprocessor and generator, organizes source (main into main function)
 * and returns everything by reference
 */
bool generate_from_file(char * file_path ,sds* processed, sds* src,sds* generated_program,struct owl_tree** tree){
    *src = read_file(file_path);
    return generate_from_string(*src,processed,generated_program,tree);
}

/*applies parser,preprocessor and generator, organizes source (main into main function)
 * and returns everything by reference
 * returns true when no errors have been found, returns true when parse error has been found
 */
bool generate_from_string(char* src,sds* processed,sds* generated_program,struct owl_tree** tree){
   *processed = preprocess(src);
   printf("texto preprocesado:\n%s\n",*processed);
    *tree = owl_tree_create_from_string(*processed);
    //logs errors if there are any
    if (handle_parse_errors(*tree)){
        return false;
    } 
    struct parsed_program  program = owl_tree_get_parsed_program(*tree);
    struct parsed_stmt_list stmt_list = parsed_stmt_list_get(program.stmt_list);

    //guardamos el main (primer statement del programa)
    //como alguien no ponga el main al principio del programa me cago en sus muertos
    sds main = generate_statement_code(parsed_stmt_get(stmt_list.stmt));
    stmt_list.stmt = owl_next(stmt_list.stmt);    

    //resto del programa (funciones)
    *generated_program = sdscatprintf(sdsempty(),"%s\n",generate_statement_list(stmt_list));

    //añadimos el main con el include al resto de las funciones
    *generated_program=sdscatprintf(sdsempty(),"%s\n%s\n%s\n\n",conf.builtins_include,*generated_program,main);
    return true;
}
