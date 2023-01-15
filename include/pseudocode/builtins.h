#include <iostream>
#include <fstream>


template<typename T>
struct is_outfile {
    static const bool value = false;
};
template<>
struct is_outfile<std::ofstream*> {
    static const bool value = true;
};

template<typename T>
struct is_infile {
    static const bool value = false;
};
template<>
struct is_infile<std::ifstream*> {
    static const bool value = true;
};

void ESCRIBIR(){}

//escribe todos los argumentos recibidos a cout, uno en cada línea.
template <typename T, typename... Types>

typename std::enable_if<!is_outfile<T>::value>::type
ESCRIBIR(T var1, Types... var2){
    std::cout << var1 <<std::endl;
    ESCRIBIR(var2...);
}

void __escribir__(std::ofstream& file) {}

template <typename T,typename... Types>
void __escribir__(std::ofstream& file,T arg1, Types... args){
    file << arg1 << std::endl;
    __escribir__(file,args...);
}


//escribe todos los siguientes argumentos al ofstream recibido en el primer argumento
template <typename... Types,typename T>
void ESCRIBIR(std::ofstream&fichero, Types... var2)
{

    __escribir__(fichero,var2...);
}




void LEER(){}

//escribe todos los argumentos recibidos a cout, uno en cada línea.
template <typename T, typename... Types>

typename std::enable_if<!is_infile<T>::value>::type
LEER(T& var1, Types... var2){
    std::cin >> var1;
    LEER(var2...);
}

void __leer__(std::ifstream& file) {}

template <typename T,typename... Types>
void __leer__(std::ifstream& file,T arg1, Types... args){
    file >> arg1;
    __leer__(file,args...);
}
//escribe todos los siguientes argumentos al ofstream recibido en el primer argumento
template <typename... Types,typename T>
typename std::enable_if<is_infile<T>::value>::type
LEER(std::ifstream& fichero, Types... var2)
{

    __leer__(fichero,var2...);
}


#define Entero int
#define ENTERO int
#define Cadena char 
#define CADENA char *
#define Caracter char
#define CARACTER char
#define Real float
#define REAL float
#define Entero int
#define Logico bool
#define LOGICO bool