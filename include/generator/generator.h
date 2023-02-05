#include "sds/sds.h"
#include "parser.h"



bool generate_from_string(char* src,sds* processed,sds* generated_program,struct owl_tree** tree);
bool generate_from_file(char * file_path ,sds* processed, sds* src,sds* generated_program,struct owl_tree** tree);
