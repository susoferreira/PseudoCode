#pragma once
#include <stdbool.h>
#include <stdio.h>
#include "sds/sds.h"

char* read_file(char* file_path);
char* sel_default(char* in,char* def);
bool file_exists (char *filename);
void index_to_line(const char* source,int* index,int*line );
int filesize(FILE* file);
void forward_to_backslash(char* str);
bool in(sds list[] ,int list_length,sds element);