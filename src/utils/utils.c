// reads a whole text file into a cstring, heap allocated
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "logger.h"


char* read_file(char* file_path){
    FILE *input=NULL;
    input = fopen(file_path,"r");
    if (input == 0x00) {
        logger("Error abriendo archivo\n",LOG_ERROR);
        exit(-1);
    }
    struct stat stat;
    fstat(input->_fileno, &stat);
    char* file = malloc(stat.st_size+1);
    fread(file,sizeof(char),stat.st_size,input);
    return file;
}

bool in(sds list[] ,int list_length,sds element){
    for(int i =0;i<list_length;i++)
    {
        if (strcmp(element,list[i])==0) return true;
    }
    return false;
}



