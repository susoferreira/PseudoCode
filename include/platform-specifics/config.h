#pragma once
//se define aquí pero cada plataforma tiene su propio config.c
/*not all config options are used in all platforms*/
typedef struct config{
    char* builtins_include;

    char* output_path;

    /*cli and gui only*/

}config;

extern  config conf;
