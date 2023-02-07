#include "logger.h"
#include "sds.h"
#include "utils.h"
#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compile_code.h"
#include "logger-gui.h"
#include "colors.h"


//this class with static variables with getters mimics a singleton ig

//shows if compiled file has errors
static bool is_executable=false;


static char* compiler=NULL;
static char* exe_path=NULL;
 
static char* get_compiler(){

    #if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
    char* compiler="./w64devkit/bin/g++.exe";
        if(!file_exists(compiler)){
            logger("No se encuentra el compilador, ejecuta descargar_compilador.ps1\n",LOG_ERROR);        
        
    }

        return "set \"PATH=./w64devkit/bin/;%PATH%\" & g++.exe";
    #endif

    logger("detectado sistema linux, usando el compilador g++\n",LOG_INFO);
    // si no hay 3 bytes de espacio en el buffer no es culpa mía
    // ( bueno si porque yo escribí todo el código)
    //retorna 0 cuando no falla
    if (system("which g++") !=0){
        logger("No se encuentra g++, debes instalarlo\n", LOG_ERROR);
    }
    return "g++"; 

}




void compile(char* filename){

    is_executable=false;

    char* compiler = get_compiler();

    char buf[2000];
    snprintf(buf,2000,"Intentando compilar %s...\n",filename);
    logger(buf,LOG_INFO);

    sds outfile =sdscatprintf(sdsempty(),"%scompiled/%s",conf.output_path,filename);
    sds command = sdscatprintf(sdsempty(),"%s %sgenerator/%s -o %s 2>&1",
                        compiler,conf.output_path,filename,outfile);


    //reusing buf
    snprintf(buf,2000,"%sComando de compilación: %s%s%s\n",RESET,BLUE,command,RESET);
    logger(buf,LOG_INFO);
    printf("%s",buf);


    FILE* out = popen(command,"r");

    if(out==NULL){
        logger("Se han producido errores al ejecutar el comando",LOG_ERROR);
        return;
    }

    char buf2[1900];
    buf2[0]=0;//in case there is no output, null byte terminator
    fread(buf2,2000,sizeof(char),out);
    
    int status = pclose(out);
    printf("status");
    if (status ==-1){
        logger("Error ejecutando el comando (?)\n", LOG_ERROR);
        is_executable=false;
        
    }else if (status == 0){

        is_executable=true;

        logger("compilación realizada satisfactoriamente\n",LOG_INFO);


    }else{
        is_executable=false;
        logger("Se han producido errores al compilar\n",LOG_ERROR);
    }

    
    //always show output, even if there are no errors
    //solo se guardan los primeros 2000 caracteres de errores, si necesitas más busca ayuda
    logger(buf2,LOG_INFO);

    sdsfree(outfile);
    sdsfree(command);
}


// i technically should free this buf there's only one in the whole runtime
// returns folder where compiled files are
char* get_exe_path(){
    if (exe_path==NULL){
        exe_path=sdscat(sdsnew(conf.output_path),"compiled/");
            #if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
                // me cago en windows, soporta / en unas funciones y en otras no
                forward_to_backslash(exe_path);
            #endif
    }
    return exe_path;
 
}

char* used_compiler(){
    if (compiler==NULL){
        compiler=get_compiler();
    }
    return compiler;
}
//returns wether the compilation was succesful or not
bool can_execute(){
    return is_executable; 
}