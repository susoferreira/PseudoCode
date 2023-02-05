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

- ~~Bucles HACER-MIENTRAS (do-while)~~

- Agradezco todos los reportes de bugs 

## Parser

Es un parser generado usando el programa [owl](https://github.com/ianh/owl), la definición de la gramática usada se puede encontrar en ```grammar.owl```. Actualmente está mayoritariamente completo, aunque le faltan algunas características,como los bucles HACER-MIENTRAS

## Preprocesador

Modifica ligeramente el pseudocódigo antes de ser parseado para que sea posible usar owl, dado a limitaciones por diseño del programa. Se encuentra básicamente completado. Los ficheros preprocesados se guardan en ```intermediate/preprocessor/<nombre_fichero>```

## Generador

Transforma el [AST](https://en.wikipedia.org/wiki/Abstract_syntax_tree) generado por el parser en código C++. Es la parte que menos completa está del programa y con más bugs

##### Legibilidad del código 

uno de los futuros objetivos para este proyecto es que el código sea razonablemente legible (indentación y estilo ) por humanos, actualmente esto no se cumple, por lo que es recomendable usar la función ```Formatear código``` de una IDE o un formateador de código (online o ejecutable como ```clang-format```)  si se pretende leer el código generado 



## Setup

El proyecto usa el sistema de construcción [Meson](https://mesonbuild.com) con el directorio de compilación ```./build```

Para compilarlo se deben tener Meson instalado y owl descargado en la raíz del proyecto, generar el parser  con ```./generate_parser.sh``` y usar ```meson compile -C ./build```, Se generará el ejecutable ```build/generator``` 

Cada vez que se modifique ```grammar.owl``` se debe regenerar el parser y recompilar el proyecto

## Uso

Se incluye un script ```./scripts/ejecutar_fichero.sh``` que aplica el generador a un fichero, compila y ejecuta el código generado, puede que se deba abrir el script con un editor de texto y modificar la ruta del compilador

## Uso como librería



## Disclaimer

**This software serves as educative tool for the author, as well as a tool for users to learn how to translate pseudo code to C++. The software is provided "as is", without warranty of any kind, and shall not be related to any external entities and/or their products.**

**The author does not guarantee the correct operation of the software in any scenario excluded from the internal testing process. The author will not be liable for any repercusions the usage of this software might have on the user.**

**The source code is available for inspection and vulnerability reports. The source code is available for commercial use, patent use, private use, modification and distribution on the conditions stated in the _GNU Affero General Public License v3.0_, available [here](LICENSE).**
