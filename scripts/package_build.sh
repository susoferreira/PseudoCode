#!/bin/bash
USAGE="Uso: <package_build> windows|linux"
path="/tmp/package_build"
if [ $# -lt 1 ];then
    echo $USAGE
    exit
fi

case $1 in
    [Ww]indows)
        os=windows
        exe=".exe"
        ;;
    [Ll]inux)
        os=linux
        exe=""
        ;;
    *)
        echo $USAGE
        exit 1
esac


rm -rf $path
for i in 'preprocessor' 'generator' 'code_io' 'compiled';do
    mkdir -p $path/intermediate/$i
done

cp -r ./tests ./LICENSE ./README.MD $path

cp ./include/platform-specifics/cli/builtins.h $path/builtins.h


#copy executables
for filename in "generator_tui" "imgui_test";do
    cp ./build_$os/$filename$exe $path
done

#find dlls and copy them to release
if [ $os = "windows" ];then

    mv "./scripts/descargar_compilador.ps1" $path/descargar_compilador.ps1 

    for i in  $(find ./build_windows/subprojects/ -name "*.dll");do
        cp $i $path/$(basename $i)
    done

    for file in "libwinpthread-1.dll" "libgcc_s_seh-1.dll" "libstdc++-6.dll";do
        for found in $(find /usr/x86_64-w64-mingw32/bin/ -name "$file");do
            cp $found "$path/$(basename $found)"
        done
    done
fi


rm ./build_$os.zip
zip -r ./build_$os.zip $path/*