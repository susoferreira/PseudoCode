/* 
 * cada plataforma(web,interfaz gráfica...) tiene implementaciones 
 * propias, en principio debe implementar salidas (ESCRIBIR), entradas(LEER)
 * (LEER y ESCRIBIR van en builtins.h dentro de cada carpeta, y se incluirán 
 * en el código generado)
 * y un logger para los errores 
 */

#include <stdio.h>
#include "logger.h"
#include "utils.h"
#include "colors.h"


//should be set by main to be able to define error ranges
const char* source_str;

const char* logger_names[] = {"Error: ","Aviso: "  ,"Info: "};
const char* colors[]       = {RED      ,YELLOW     ,BLUE};

//for checking how many logs of each kind have happened
int has_logged[]           = {0        ,0          ,0};



//log types son LOG_ERROR,LOG_WARNING Y LOG_INFO
void logger(char* str, enum logger_types type){
    // type,colors y logger_names estan alineados para no usar un if
    const char* color = colors[type]; 
    printf("%s%s%s%s",color,logger_names[type],str,RESET);
    has_logged[type]++;

}

//log types son LOG_ERROR,LOG_WARNING Y LOG_INFO
// admite la posición del error en el archivo
void log_range(char* str, enum logger_types type,int range_start,int range_end){
    // type,colors y logger_names estan alineados para no usar un if
    const char* color = colors[type];
    
    int index_start = range_start;
    int line_start = 0;

    index_to_line(source_str,&index_start,&line_start);

    int index_end = range_end;
    int line_end = 0;

    index_to_line(source_str,&index_end,&line_end);  

    printf(" DESDE línea %s%d%s, posición  %s%d%s HASTA\
                    línea %s%d%s, posición  %s%d%s -\
                    %s%s%s%s ",
    GREEN,line_start,RESET,
    GREEN,index_start,RESET,
    GREEN,line_end,RESET,
    GREEN,index_end,RESET,

    color,logger_names[type],str,RESET);

    has_logged[type]++;
}