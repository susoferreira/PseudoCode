#include <unistd.h>
#include <stdio.h>
#include "sds/sds.h"
#include <libgen.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include "preprocessor.h"

bool in(sds list[] ,int list_length,sds element){
    for(int i =0;i<list_length;i++)
    {
        if (strcmp(element,list[i])==0) return true;
    }
    return false;
}
 
sds modify_function(sds type,sds declaration){
    return sdscatprintf(sdsempty(),"%s %s\n",declaration,type);
}

char* preprocess(char* file_path){
    


    sds result=sdsempty();
    FILE *input = fopen(file_path,"r");
    FILE *out = fopen(sdscat(sdsnew("./intermediate/preprocessor/"),basename(file_path)),"w");


    sds typedefs[100] = {"Entero","Real","Cadena","Carácter","Lógico"}; //builtin types
    //como haya más de 100 tipos en el programa me suicido
    int typedef_size = 4;


    if (!input || !out)
    {
        char buf[100];
        getcwd(buf,100);
        printf("current working directory: %s\n",buf);
        printf("Error de preprocesador, fichero no abierto\n");
        exit(-1); 
    }

    char line_char[100];
    while(fgets(line_char, 1000000, (FILE*)input)){
        int words_size;
        sds * words = sdssplitlen(sdsnew(line_char),sdslen(sdsnew(line_char))," ",1,&words_size); //array de sds

        if (words_size==0) continue;

        if ((words_size ==3) && (strcmp(words[1], "=")==0) && (strcmp(words[2],"REGISTRO")==0)) typedefs[++typedef_size] = words[0];

        
        if (in(typedefs,typedef_size,words[0])){
            sds line = sdstrim(sdsnew(line_char),"\n");
            sdsrange(line,sdslen(words[0]),sdslen(line)); //gets only the declaration(without the type)
            
            sds modified_function = modify_function(words[0],line);
            fputs(modified_function,out);

            result = sdscat(result,modified_function);
            continue;
        }

        fputs(line_char,out);

        result = sdscat(result,line_char);
    

    }
    fclose(input);
    fclose(out);

    return result;
}

