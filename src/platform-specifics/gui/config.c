#include "config.h"
//en este archivo simplemente se define la variable struct config
config conf ={
    

    //for debug enviroment
    //.builtins_include = "#include \"../../include/platform-specifics/cli/builtins.h\"",

    //for release enviroment
    .builtins_include = "#include \"../../builtins.h\"",
    
    .output_path      = "./intermediate/",
};
