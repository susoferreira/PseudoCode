// reads a whole text file into a cstring, heap allocated
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "utils.h"
#include "logger.h"
#include "config.h"




/* antes usaba fstat y no esta guarrada pero parece 
 * ser que los cavernícolas de windows no tienen fstat
 * y no quiero llenar todo de ifdef
 *
 * sacado de stackoverflow
 * https://stackoverflow.com/questions/238603/how-can-i-get-a-files-size-in-c
 */
int filesize(FILE* file){
    fseek(file, 0, SEEK_END); // seek to end of file
    int size = ftell(file); // get current file pointer
    fseek(file, 0, SEEK_SET); // seek back to beginning of file
    return size;
}

bool file_exists (char *filename) {
  struct stat   buffer;   
  return (stat (filename, &buffer) == 0);
}

char* read_file(char* file_path){
    FILE *input=NULL;
    input = fopen(file_path,"r");
    if (input == 0x00) {
        logger("Error abriendo archivo\n",LOG_ERROR);
        exit(-1);
    }
    int size = filesize(input);
    printf("Tamaño del archivo: %d",size);
    char* file = malloc(size+1);
    fread(file,sizeof(char),size,input);
    printf("texto leído del archivo:\n\n%s",file);
    return file;
}

// used to select default values,
// returns the same type it was given (sds/char)

char* sel_default(char* in,char* def){
    return in[0]==0|| in[0]=='\n' ? def: in;
}


bool in(sds list[] ,int list_length,sds element){
    for(int i =0;i<list_length;i++)
    {
        if (strcmp(element,list[i])==0) return true;
    }
    return false;
}

//receives index and returns line number + index in that line
// useful for error handling and highlighting
// position is input and output, line is only output
void index_to_line(const char* source,int* index,int*line ){
    *line=1;
    int new_index = 1;

    for (int i =0; i< *index; i++) {

        if (source[i]==0x00){
            char buf[200];
            snprintf(buf, 200, "se ha llegado al final del texto cuando se buscaba un error en la posición %d",*index);
            logger(buf,LOG_ERROR);
            return;
        }
        new_index++;
        if (source[i]=='\n') 
            (*line)++;
    }
    *index=new_index;
}
