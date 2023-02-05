// el header logger.h es global pero la implementación depende de las sources que se usen al compilar
// para distintas plataformas

/* las plataformas pueden crear su propio header que amplíe la funcionalidad de este, 
 * e inlcuir ese en sus implementaciones, como por ejemplo logger-gui.h
 */
#pragma once
#include <stdbool.h>

enum logger_types            {LOG_ERROR    ,LOG_WARNING    ,LOG_INFO};
extern int has_logged[3];//for checking how many logs of each kind have happened
void logger(char* str, enum logger_types type);
void log_range(char* str, enum logger_types type,int range_start,int range_end);

