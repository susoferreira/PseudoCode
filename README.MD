# Transpilador de pseudocódigo a C(++)

Este repositorio contiene un programa para transformar pseudocódigo a código equivalente en c++ (usando mayoritariamente características de C) que sea legible por humanos para poder comparar ambos. Actualmente **aún no está terminado**. la transformación ocurre en 3 subprocesos:

## Parser

Es un parser generado usando el programa [owl](https://github.com/ianh/owl), la definición de la gramática usada se puede encontrar en ```grammar.owl```. Actualmente está mayoritariamente completo, aunque le faltan algunas características, como soportar declaraciones de arrays dentro del bloque ```TIPOS```

## Preprocesador

Modifica ligeramente el pseudocódigo antes de ser parseado para que sea posible usar owl, dado a limitaciones por diseño del programa. Se encuentra básicamente completado. Los ficheros preprocesados se guardan en ```intermediate/preprocessor/<nombre_fichero>```

## Generador

Transforma el [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree) generado por el parser en código C++. Es la parte que menos completa está del programa y con más bugs

## Setup

El proyecto usa el sistema de construcción [Meson](https://mesonbuild.com) con el directorio de compilación ```./build```

Para compilarlo se deben tener Meson instalado y owl descargado en la raíz del proyecto, generar el parser  con ```./generate_parser.sh``` y usar ```meson compile -C ./build```, Se generará el ejecutable ```build/generator``` 

Cada vez que se modifique ```grammar.owl``` se debe regenerar el parser y recompilar el proyecto

## Uso

La sintáxis del generador es la siguiente: ``` generator <nombre fichero>``` ~Actualmente no es muy usable, en parte porque el output está mezclado con mensajes de debug y error, y en parte porque no funciona (xd)~ la salida del generador se guarda en ```./intermediate/generator/<nombre_fichero>```

