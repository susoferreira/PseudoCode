# Transpilador de pseudocódigo a C(++)

Este repositorio contiene un programa para transformar pseudocódigo a código equivalente en c++ (usando mayoritariamente características de C) que sea legible por humanos para poder comparar ambos. Actualmente **aún no está terminado**. la transformación ocurre en 3 subprocesos:

#### Cosas que no funcionan actualmente
- Asignar un literal a una ```CADENA``` despues de inicializarla:

  ```
  VARIABLES
  CADENA x[10]
  INICIO
  x <- "asd"
  FIN
  ```

- Bucles HACER-MIENTRAS (do-while)

- Agradezco todos los reportes de bugs 

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

Se incluye un script ```./scripts/ejecutar_fichero.sh``` que aplica el generador a un fichero, compila y ejecuta el código generado, puede que se deba abrir el script con un editor de texto y modificar la ruta del compilador

## Uso como librería

La sintáxis del generador es la siguiente: ``` generator <nombre fichero>```, aplicará el preprocesador al fichero, guardará el texto preprocesado en ```./intermediate/preprocessor/<nombre fichero>```, y generará el código c++ en ````./intermediate/generator/<nombre fichero> ```.  

