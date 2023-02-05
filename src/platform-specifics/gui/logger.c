/* 
 * cada plataforma(web,interfaz gráfica...) tiene implementaciones 
 * propias, en principio debe implementar salidas (ESCRIBIR), entradas(LEER)
 * (LEER y ESCRIBIR van en builtins.h dentro de cada carpeta, y se incluirán 
 * en el código generado)
 * y un logger para los errores 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger-gui.h"
#include "logger.h"
#include "sds/sds.h"
#include "utils.h"



//used for getting line from index, platforms must use set_src
static const char* source_str;

const char* logger_names[] = {"Error: ","Aviso: "  ,"Info: "};
int has_logged[]           = {0        ,0          ,0};
static struct log* logs;
static int log_size=0;
static int log_max_size=10;

//adds log to array
static void add_log(struct log newlog){
    if(!log_size){
        logs = (struct log*) malloc(sizeof(struct log)*log_max_size);
    }

    if(log_size == log_max_size){
        log_max_size*=2;
        logs = (struct log*) realloc(logs,log_max_size*sizeof(struct log));
    } 
    logs[log_size++] = newlog;
}


struct log* get_logs(){
    return logs;
}
int get_log_size(){
    return log_size;
}
void free_logs(){
    if (log_size==0)
        return;
    log_size=0; // sets log_size to 0 so next log will malloc
    free(logs);
}

void set_src(char* src){
    source_str=src;
}

//log types son LOG_ERROR,LOG_WARNING Y LOG_INFO
void logger(char* str, enum logger_types type){
    // type,colors y logger_names estan alineados para no usar un if
    has_logged[type]++;

    char* newstr = malloc(strlen(str)+1);
    strncpy(newstr,str,strlen(str)+1);
    add_log((struct log) {newstr,type});

}

// log types son LOG_ERROR,LOG_WARNING Y LOG_INFO
// admite la posición del error en el archivo
void log_range(char* str, enum logger_types type,int range_start,int range_end){
    // type,colors y logger_names estan alineados para no usar un if
    has_logged[type]++;
    
    int index_start = range_start;
    int line_start = 0;

    index_to_line(source_str,&index_start,&line_start);

    int index_end = range_end;
    int line_end = 0;

    index_to_line(source_str,&index_end,&line_end); 


    char* newstr = malloc(strlen(str)+1);
    strncpy(newstr,str,strlen(str)+1);

    
    add_log((struct log)
        {
            .text=newstr,
            .type=type,
            .has_range=true,
            .range=(struct range)
            {
                .start_index=range_start,
                .end_index=range_end,
                .range_start_line=line_start,
                .range_start_pos=index_start,
                .range_end_line=line_end,
                .range_end_pos=index_end,
            }
        }
    );
}