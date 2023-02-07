# Transpilador de pseudocódigo a C(++)

Este repositorio contiene un programa para transformar pseudocódigo a código equivalente en c++ (usando mayoritariamente características de C) que sea legible por humanos para poder comparar ambos. Actualmente **aún no está terminado**. la transformación ocurre en 3 subprocesos:

## Cosas que no funcionan actualmente
- Asignar un literal a una ```CADENA``` despues de inicializarla:

  

  ```
  VARIABLES
  CADENA x[10]
  INICIO
  x <- "asd"
  FIN
  ```

  (una manera de arreglar esto es modificar el código generado **manualmente** de la siguiente forma)

  ``` c
  Cadena uwu ="owo" => Cadena uwu =(char*)"owo"
  ```
  

- Abrir y cerrar ficheros (WIP)

## Legibilidad del código 

uno de los futuros objetivos para este proyecto es que el código sea razonablemente legible (indentación y estilo ) por humanos, actualmente esto no se cumple, por lo que es recomendable usar la función ```Formatear código``` de una IDE o un formateador de código (online o ejecutable como ```clang-format```)  si se pretende leer el código generado 


## Uso
Para usarlo basta con descargar la última release y ejecutar ```generator_tui```(interfaz de línea de comandos)  o ```generator_gui``` (interfaz gráfica), generator_tui da problemas en windows por problemas con el locale (lee memoria basura por algún motivo), se puede probar a ejecutarlo desde powershell o **(recomendado)** usar la interfaz gráfica.

Para usar la opción de compilar el código se debe tener instalado g++ en linux, en windows ejecutar el script ```descargar_compilador.ps1``` descargará un entorno de desarrollo **mínimo y completamente portable** sin hacer modificaciones al sistema,lo que permitirá al proyecto usarlo para compilar

## Errores
Un error al generar el código c++ significa un error de parseo (el pseudocódigo está mal formado, faltan palabras clave, falta la palabra ALGORITMO...)

Un error al compilar el código significa que el programa está mal escrito aunque tenga la estructura correcta, por lo general los errores que da el compilador sobre el c++ son bastante fáciles de relacionar con el pseudocódigo que los generó y arreglarlos.

Un error que no debería estar ahi (error de parser cuando el código está bien formado o error de compilación cuando el pseudocódigo es válido) es motivo suficiente para [abrir una issue](https://github.com/susoferreira/PseudoCode/issues/new)( por favor, reportad issues y no espereis a que otra persona lo haga :) )

## Compilar desde código fuente

El proyecto usa el sistema de construcción [Meson](https://mesonbuild.com)

- Para crear un directorio de compilación se usa ```cd src; meson setup <directorio de la build>```
- Para compilarlo despues del setup basta con ```meson compile .c <directorio de la build>```
- En ```scripts``` se guardan distintos scripts para facilitar la build
- Si se modifica ```grammar.owl``` se debe usar el comando ```owl -c grammar.owl >parser.h```
- Al ejecutar el programa sin generar una release con ./scripts/package_build puede que el ejecutable no encuentre distintos archivos, esto se debe a que la estructura de ficheros es distinta en las releases que en el proyecto, se puede arreglar enlazando los archivos que busca a la raíz del proyecto o cambiando la ruta de busqueda (```config.c```), también, en windows **se deberán copiar los dll que genera ```./scripts/package_build windows``` al directorio de la build**
- para enlazar los archivos se puede usar ```ln ./include/platform-specifics/cli/builtins.h builtins.h``` y **descargar [w64devkit](https://github.com/skeeto/w64devkit/releases)** en la raíz del proyecto, ya sea mediante github o mediante ```./scripts/descargar_compilador.ps1```
- Para  hacer _cross-compilation_ desde linux a windows se puede usar ```cd src; meson setup ../build_windows --cross-file ../cross_compilation.ini``` (requiere mingw-64), para otros targets, se puede ver la documentacion de **Meson**, no es demasiado difícil 


## Uso como librería

El programa que transforma pseudocódigo en c++ se encuentra en ```./generator.c``` ,```./generators.c```, se debe proveer una implementación propia de ```logger.c``` y ```config.c```. 
> Se recomienda usar una de las implementaciones en ```./src/platform-specifics``` y sus respectivas lineas de _executable_ en ```meson.build``` de ejemplo


### Librerías usadas:
- [sds](https://github.com/antirez/sds/) (strings dinámicos en c)
- [owl](https://github.com/ianh/owl)
- [Dear ImGui](https://github.com/ocornut/imgui) (interfaz gráfica)
- [ImGuiColorTextEdit](https://github.com/BalazsJako/ImGuiColorTextEdit) (editor de texto con _syntax highlighting_ para imgui)
- [GLFW3](https://www.glfw.org/) (Renderer para imgui)

## Disclaimer

**This software serves as educative tool for the author, as well as a tool for users to learn how to translate pseudo code to C++. The software is provided "as is", without warranty of any kind, and shall not be related to any external entities and/or their products.**

**The author does not guarantee the correct operation of the software in any scenario excluded from the internal testing process. The author will not be liable for any repercusions the usage of this software might have on the user.**

**The source code is available for inspection and vulnerability reports. The source code is available for commercial use, patent use, private use, modification and distribution on the conditions stated in the _GNU Affero General Public License v3.0_, available [here](LICENSE).**
