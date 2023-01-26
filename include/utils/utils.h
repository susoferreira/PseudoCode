#pragma once
#include <stdbool.h>
#include "sds/sds.h"

char* read_file(char* file_path);

bool in(sds list[] ,int list_length,sds element);