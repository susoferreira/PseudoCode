

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

#define OWL_PARSER_IMPLEMENTATION
#include "parser.h"

//preprocesses a file,saves output to another file and returns string 
char* preprocess_file(char *src,char* file_path){
    sds result = preprocess(src);

    FILE *out = fopen(sdscat(sdsnew("./intermediate/preprocessor/"),basename(file_path)),"w");
    if(out == NULL) {
        logger("file couldn't be opened",LOG_ERROR);
        exit(-1);
    }
    fclose(out);
    return result;
}

bool handle_parse_errors(struct owl_tree* tree){
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


int main(int argc, char* argv[])
{
    struct owl_tree *tree;
 
    
    if (argc < 2) {
        printf("Uso: generator <archivo>\n");
        exit(-1);
    }

    char* src = read_file(argv[1]);
    sds processed = preprocess_file(src,argv[1]);

    tree = owl_tree_create_from_string(processed);


    if (handle_parse_errors(tree)){   
        exit(-1);
    }


    struct parsed_program  program = owl_tree_get_parsed_program(tree);
    struct parsed_stmt_list stmt_list = parsed_stmt_list_get(program.stmt_list);

    //guardamos el main (primer statement del programa)
    //como alguien no ponga el main al principio del programa me cago en sus muertos
    sds main = generate_statement_code(parsed_stmt_get(stmt_list.stmt));
    stmt_list.stmt = owl_next(stmt_list.stmt);    

    //resto del programa (funciones)
    sds  generated_program = sdscatprintf(sdsempty(),"%s\n",generate_statement_list(stmt_list));
    printf("%s\"\n%s\n%s\n",conf.builtins_include,generated_program,main);
    owl_tree_destroy(tree);

    char outfile[500]="";
    snprintf(outfile,500,"./intermediate/generator/%s.cpp",basename(argv[1]));
    FILE* output = fopen(outfile,"w");

    //builtins_include se define según la plataforma
    fprintf(output,"%s\"\n%s\n%s\n",conf.builtins_include,generated_program,main);
    

    free(src);
    return 0;
}