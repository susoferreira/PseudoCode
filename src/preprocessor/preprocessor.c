#include <unistd.h>
#include <stdio.h>
#include "sds/sds.h"
#include <libgen.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include "preprocessor.h"
#include "utils.h"

int is_do_while_start(sds *words) {
    return strcmp(words[0], "HACER") == 0;
}

int is_do_while_end(sds *words) {
    return strcmp(words[0], "MIENTRAS") == 0;
}

int is_struct_declaration(int words_size, sds *words) {
    return (words_size == 3) && (strcmp(words[1], "=") == 0)
            && (strcmp(words[2], "REGISTRO") == 0);
}

//two or more spaces in a row will create this
//words_size will be read and repurposed for the new array
sds * remove_empty_words(sds words[],int words_size){
    sds new_words[words_size];
    int counter=0;
    //str==0x00 ==> empty string
    for (int i =0; i< words_size; i++) {
        if(words[i][0] == 0x00){
            sdsfree(words[i]);
            continue;
        }
        new_words[counter++] = words[i];
    }
    //copying results onto a stack allocated array
    sds* ret = malloc(sizeof (sds)*counter+1);
    for (int i=0;i<counter;i++) {
        ret[i]=new_words[i];
    }
    return ret;
}

sds preprocess(char* src){

    sds src_ = sdsnew(src);
    int lines_size;
    sds * lines = sdssplitlen(src_,sdslen(src_) ,"\n",1,&lines_size);

   

	sds line;
    sds result=sdsempty();
    char* typedefs[100] = {"Entero","Real","Cadena","Carácter","Lógico"}; //builtin types
    //como haya más de 100 tipos en el programa me suicido
    int typedef_size = 4;

    for (int i=0; i<lines_size; i++)
    
    {
        
        line = lines[i];

        int words_size=0;
        sds * words = sdssplitlen(line,sdslen(line)," ",1,&words_size);
        words = remove_empty_words(words,words_size);

        if (words_size==0) continue;
        
        //modificar el inicio del do-while para que lo pueda detectar el parser
        if (is_do_while_start(words)) {

            sdsfree(line);
            line=sdsnew("HACER_");
        }

        if (is_do_while_end(words)) {
            sdsrange(line,sdslen(words[0]),sdslen(line)); //gets the rest of the line
            line = sdscat(line," MIENTRAS_");
        }

        if (is_struct_declaration(words_size, words))
            typedefs[++typedef_size] = words[0];

        
        if (in(typedefs,typedef_size,words[0])){
            sds line = sdsnew(line);
            sdsrange(line,sdslen(words[0]),sdslen(line)); //gets only the declaration(without the type)
            line = sdscat(words[0],line);
        }

        line=sdscat(line,"\n");
        result=sdscat(result,line);
        sdsfree(line);
        
    }


    return result;
}

