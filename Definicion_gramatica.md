- backus-naur form modificado (se usan operadores de regex como | * ? +)
- el texto entre comillas se refiere a literales,
- y para indicar lo que representan algunos tokens se usa @
- por ejemplo <identificador@tipo-variable>
- <expr -(cosa)> significa todos los identificadores menos cosa
- sintaxis inspirada (muy inspirada) por owl
- https://github.com/ianh/owl


programa := <algoritmo> (<procedimiento>|<funcion>)*
algoritmo := <identificador@nombre-algoritmo> <declaracion-variables> <declaraciones>* "FIN"


cabecera-funcion := <identificador@nombre-funcion> <lista-parametros> <final-linea>

procedimiento := "PROCEDIMIENTO" <cabecera-funcion>  <declaracion-variables> <declaraciones>* "FIN_PROCEDIMIENTO" 


las funciones son modificadas por el preprocesador de la forma
<TIPO> "FUNCION" <NOMBRE> <LISTA-PARAMETROS> <DECLARACIONES>*
 a
"FUNCION" <NOMBRE> <LISTA-PARAMETROS> <TIPO> 
para que sea posible parsear el código en tiempo líneal y de manera más sencilla

funcion := "FUNCION" <cabecera-funcion> <identificador@tipo-retorno> <declaracion-variables> <declaraciones*> "FIN_FUNCION" 

asignacion := <identificador> "<-" <expesion> 
asignacion-campo := <expresion -(negacion,array,parentesis)> "." <identificador> "<-" <identificador>
asignacion-array:= <expresion -(negacion,array,parentesis)> "[" <expresion>] "<-  
