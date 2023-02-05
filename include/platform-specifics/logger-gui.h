#pragma once

#include "logger.h"
//specific things for gui logging

extern const char* logger_names[];

struct range{
    int start_index;
    int end_index;

    int range_start_line;
    int range_start_pos;
    
    int range_end_line;
    int range_end_pos;
};

struct log{
    char* text;
    enum logger_types type;
    bool has_range;
    struct range range;

};
struct log* get_logs();
int get_log_size();
void free_logs();
void set_src(char* src);