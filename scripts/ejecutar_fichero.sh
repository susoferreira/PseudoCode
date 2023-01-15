#!/usr/bin/bash

[ $# -lt 1 ] && echo "Error. uso: ejecutar_fichero.sh <nombre_fichero>" && exit
compiler=g++
generator="./build/generator"
sources="./src/pseudocode/builtins.cpp"
include="./include"
file="./intermediate/generator/$(basename $1).cpp"

echo -e "\e[31;1mGenerando código...\e[0m"
$generator $1
echo -e "\e[31;1m Compilando código generado...\e[0m"
$compiler  -g $file $sources -I$include -o ./output/$(basename $1)
echo -e "\e[31;1m Ejecutando...\e[0m"
./output/$(basename $1)
