#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include "config.h"
#include "parser.h"
#include "sds/sds.h"
#include "preprocessor.h"
#include "logger.h"
#include "generator.h"
#include "utils.h"
#include <locale.h>
#include "compile_code.h"
#include "colors.h"
#if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
#include <windows.h>
#endif





//entry point for cli platform


void locale(){
    #if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
        
        printf("locale es:%s\n",setlocale(LC_ALL,"C"));
    #endif
    return;
}
void exit_wait(int status){
    printf("%s","presiona ENTER");
    fgetc(stdin);
    exit(status);
}

void enable_ansi_colors(){
    //hay que activar los colores a mano en windows porque son tontos
    #if defined WIN32 || defined _WIN32 || defined WIN64 || defined _WIN64
        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD dwMode = 0;
        GetConsoleMode(hOut, &dwMode);
        dwMode = dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, dwMode);

        // References:
        //SetConsoleMode() and ENABLE_VIRTUAL_TERMINAL_PROCESSING?
        //https://stackoverflow.com/questions/38772468/setconsolemode-and-enable-virtual-terminal-processing

        // Windows console with ANSI colors handling
        // https://superuser.com/questions/413073/windows-console-with-ansi-colors-handling
    #endif
}






//fflush no vale para inputstreams
void flush_stdin(){
    char c;
    while((c = fgetc(stdin)) != '\n' && c != EOF);
    
}

char get_ch_in(){
    char c= fgetc(stdin);
    if (c!='\n' ) {
        flush_stdin();
    }
    return c;

}

void write_out(char *src,char* output_path){
    enable_ansi_colors();
    FILE *out = fopen(output_path,"w");

    if(out == NULL) {
        char buf[300];
        snprintf(buf,300,"No se pudo abrir el fichero %s para escritura, asegurate de que exista el directorio",
            output_path);
        logger(buf,LOG_ERROR);
        exit_wait(-1);
    }
    fprintf(out,"%s",src);
    fclose(out);
}

sds get_str_in(char* prompt,bool optional){
    char buf[200];


    printf("%s",prompt);
    do{
        fgets(buf,200,stdin);
    
    //buf[0]=='\n' => string vacio
    }while( !optional && buf[0]=='\n' &&
        printf("%s","Esta entrada no es opcional, debes introducir un valor\n"));

    buf[strcspn(buf, "\n")] = 0;//eliminar newline
    return  sdsnew(buf);
}



void op4(sds file){
    compile(file);

    logger("Compilación realizada sin errores\n",LOG_INFO);
    printf("%s","presiona una tecla para ejecutar el código\n\n");
    get_ch_in();
    sds exe = sdscat(get_exe_path(),file);
    system(exe);
    printf("\n\n");
}



int main(){
    locale(); //hace set_locale en windows
    sds path = get_str_in("Introduce la ruta del archivo a generar:",false);
    sds output_path=sdsnew(conf.output_path);

    printf("Ruta de salida configurada a: %s, asegurate de que en dentro de esa ruta existan los directorios:\
%spreprocessor%s, %sgenerator%s y %scode_io%s\n\n",
        output_path,
        RED,RESET,
        BLUE,RESET,
        GREEN,RESET);
    
    sds filename;

    sds processed = NULL;
    sds src = NULL;
    sds generated_program = NULL;
    struct owl_tree* tree = NULL;
    //retorna falso cuando hay un error, si hay error no se debería guardar el programa generado pero si el preprocesado
    if (!generate_from_file(path,&processed,&src,&generated_program,&tree)){


        sds out = sdscat(sdscat(sdsnew(output_path),"/preprocessor/"),basename(path));
        write_out(processed,out);
        sdsfree(out);
        

    }else{

        sds out = sdscat(sdscat(sdsnew(output_path),"/preprocessor/"),basename(path));
        write_out(processed,out);
        sdsfree(out);

        filename = sdscat(sdsnew(basename(path)),".cpp");
        out = sdscat(sdscat(sdsnew(output_path),"/generator/"),filename);
        write_out(generated_program,out);
        sdsfree(out);
    }

    sds out = sdscat(sdscat(sdsnew(output_path),"/preprocessor/"),basename(path));
    write_out(processed,out);
    sdsfree(out);



    const char* sel ="Menú:\n\
0:Cerrar\n\
1: Ver código generado\n\
2: Ver árbol de código (debug)\n\
3: Ver código preprocesado(debug)\n\
4: Compilar y ejecutar el código\n";

    printf("Procesamiento del código terminado,se produjeron:\
    %s%d Errores%s\
    %s%d Avisos%s\
    y %s%d Info%s\n",
    RED,has_logged[LOG_ERROR],RESET,
    YELLOW,has_logged[LOG_WARNING],RESET,
    BLUE,has_logged[LOG_INFO],RESET);

    /*mucha indentación, lo se, pero solo son printf >:( )*/
    char op;

    do{
        printf("%s",sel);
        op = get_ch_in();
        switch (op) {
            case '0':
                printf("Adiós amigo\n");
                break;

            case '1':
                printf("%sINICIO DEL CÓDIGO GENERADO%s\n\n\n%s\n\n\n%sFIN DEL CÓDIGO GENERADO%s\n%s Presiona 1 Para copiar el código%s\n\n"
                    ,GREEN,RESET
                    ,generated_program
                    ,GREEN,RESET
                    ,BLUE,RESET);

                if(get_ch_in() == '1'){
                    printf("Es broma, no funciona copiar PORQUE CADA SISTEMA LO HACE A SU MANERA\n");
                    printf("Algún día lo implementaré,supongo :(\n");
                }
                break;

            case '2':
                printf("%sINICIO DEL ÁRBOL%s",GREEN,RESET);
                owl_tree_print(tree);
                printf("%sFIN DEL ÁRBOL%s\n",GREEN,RESET);
                break;

            case '3':
                printf("%sINICIO DEL CÓDIGO PREPROCESADO %s\n\n\n%s\n\n\n%sFIN DEL CÓDIGO PREPROCESADO%s\n%sPresiona 1 Para copiar el código%s\n"
                    ,GREEN,RESET
                    ,processed
                    ,GREEN,RESET
                    ,BLUE,RESET);
                if(get_ch_in() == '1'){
                    printf("Es broma, no funciona copiar PORQUE CADA SISTEMA LO HACE A SU MANERA\n");
                    printf("Algún día lo implementaré,supongo :(\n");
                }
                break;
            case '4':
                op4(filename);
                break;
            default:
                printf("Opción %c no válida, a ver si aprendemos a leer compañero:\n",op);
                break;
            
        }
    }while(op!='0');
    sdsfree(output_path);
    sdsfree(path);
    printf("presiona ENTER para continuar\n");
    fgetc(stdin);

    
}

