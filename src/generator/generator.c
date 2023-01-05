

#include "generators.h"
#include <stdio.h>
#include <string.h>
#include "./sds/sds.h"
#include "preprocessor.h"

#define OWL_PARSER_IMPLEMENTATION
#include "parser.h"
int main(int argc, char* argv[])
{
    struct owl_tree *tree;


    if (argc < 2) {
        printf("Uso: generator <archivo>");
        exit(-1);
    }
    sds processed = sdsnew(preprocess(argv[1]));
    tree = owl_tree_create_from_string(processed);
    
    if(tree->error ){
        int errorlen=tree->error_range.end-tree->error_range.start;
        char buf[errorlen+100];
        //margen de 5 caracteres a la izquierda y a la derecha
        for(int i =0;i<errorlen+30;i++){ 
            buf[i] = tree->string[tree->error_range.start+i-15];
        }
        printf("error en: %s\n",buf);
    }
    owl_tree_print(tree);

    struct parsed_program  program = owl_tree_get_parsed_program(tree);
    struct parsed_stmt_list stmt_list = parsed_stmt_list_get(program.stmt_list);
    
    sds  generated_program = sdscatprintf(sdsempty(),"int main {%s\nreturn 0;}",generate_statement_list(stmt_list));
    printf("%s",generated_program);
    owl_tree_destroy(tree);
    
    return 0;
}