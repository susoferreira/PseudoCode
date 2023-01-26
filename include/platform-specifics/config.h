#pragma once
//se define aquí pero cada plataforma tiene su propio config.c
typedef struct config{
    char* builtins_include;
}config;

extern const config conf;
