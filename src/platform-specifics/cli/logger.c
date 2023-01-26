/* 
 * cada plataforma(web,interfaz gráfica...) tiene implementaciones 
 * propias, en principio debe implementar salidas (ESCRIBIR), entradas(LEER)
 * (LEER y ESCRIBIR van en builtins.h dentro de cada carpeta, y se incluirán 
 * en el código generado)
 * y un logger para los errores 
 */

#include <stdio.h>
#include "logger.h"

// esto tiene que ir en un .c porque son definiciones aunque realmente estaría mejor en el .h
const char* logger_names[] = {"Error: ","Aviso: "  ,"Info: "};
const char* colors[] =       {RED      ,YELLOW     ,BLUE};



//log types son LOG_ERROR,LOG_WARNING Y LOG_INFO
void logger(char* str, enum logger_types type){
    // type,colors y logger_names estan alineados para no usar un if
    const char* color = colors[type]; 
    printf("%s%s%s%s",color,logger_names[type],str,RESET);

}
//log types son LOG_ERROR,LOG_WARNING Y LOG_INFO
// admite la posición del error en el archivo
void log_range(char* str, enum logger_types type,int range_start,int range_end){
    // type,colors y logger_names estan alineados para no usar un if
    const char* color = colors[type]; 
    printf("%s%s%s%s",color,logger_names[type],str,RESET);
//TODO: HACER ALGO CON LA POSICIÓN DEL ERROR (subrayar y tal no se)
}