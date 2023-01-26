// el header logger.h es global pero la implementación depende de las sources que se usen al compilar
// para distintas plataformas
#pragma once
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define RESET "\033[0m"


enum logger_types            {LOG_ERROR    ,LOG_WARNING    ,LOG_INFO};

void logger(char* str, enum logger_types type);
void log_range(char* str, enum logger_types type,int range_start,int range_end);