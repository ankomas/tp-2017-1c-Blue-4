/*
 * blue4-lib.h
 *
 *  Created on: 4/4/2017
 *      Author: alejandrokrumkamp
 */

#ifndef BLUE4_LIB_H_
#define BLUE4_LIB_H_

#include <stdint.h>
#include <stdarg.h>

typedef struct {
	int tipo;
	int tam;
}header_t;

/**
 * @NAME: serializadorTom
 * @DESC: envia un mensaje serializado
 * @Uso: 	Parametro 1 (int socket), el socket al que le vamos a enviar el mensaje
 * 			Parametro 2 (int tipo), el tipo de mensaje (mensaje, archivo, operacion)
 * 			Parametro 3 (char* mensaje), los datos que se van a enviar
 * 			Valor de Retorno: -1 para error, 0 para OK
 */
int serializadorTom(int, int, char*);

/**
 * @NAME: desSerializadorTom
 * @DESC: recibe un mensaje serializado
 * @Uso: 	Parametro 1 (int socket), el socket que nos va a enviar mensajes serializados
 * 			Valor de Retorno: char* datos recibidos
 */

char* desSerializadorTom(int);

typedef struct{
	uint32_t pid;
	uint32_t pc;
	void * tablaArchivos;
	void * posicionStack;
	uint32_t exitCode;
	uint32_t cantidadPaginasAsignadas;
	uint32_t quantumRestante;
}t_pcb;

typedef struct{
	uint32_t data_size;
	char* data;
}package_t;

	/**
	* @NAME: serializar
	* @DESC: Serializa una serie de argumentos.
	* Uso: El primer argumento es la cantidad de argumentos totales (menos este),
	* luego los siguientes argumentos deben ser ingresados de la forma "tamanio,dato",
	* quedando de la siguiente forma:
	* serializador(cantidad_de_argumentos,tam1,dato1,tam2,dato2,.....)
	* Siendo la firma la siguiente
	* serializador(int args, uint32_t tam1,void* dato1,....)
	* Retorno: Retorna una estructura del tipo package_t, que almacena el stream y
	* su tamanio.
	* Nota: Liberar paquete.data al terminar de utilizarlo.
	* Ejemplo:
	*
	* 		uint32_t tam1,tam2,dato1;
	* 		char* dato2;
	*		package_t res;
	*		....
	* 		res=serializador(4,tam1,&dato1,tam2,dato2);
	*/
	package_t serializar(int args, ...);

	/**
	* @NAME: deserializar
	* @DESC: Obtiene un argumento dentro de un stream (con el formato de serializacion
	* de "serializar").
	* Uso: El primer argumento es un entero que indica donde empieza, dentro del stream,
	* el dato que se desea obtener dentro del stream, este entero debe ser psado por
	* referencia ya que la funcion lo actualizara al terminar su ejecucion, posicionandolo
	* en el siguiente dato para su futuro uso.
	* El segundo argumento es un Stream.
	* Retorno: Retorna una estructura del tipo package_t, que almacena el dato y
	* su tamanio.
	* Nota: Liberar paquete.data al terminar de utilizarlo.
	* Ejemplo:
	*
	*		uint32_t *dato1,puntero;
	*		char* dato2
	*		package_t dato2_p;
	*		.....
	*		dato1=(uint32_t*)deserializar(&puntero,package.data).data;
	*
	*		dato2_p=deserializar(&puntero,package.data);
	*		dato2=malloc(dato2_p.data_size);
	*		dato2=dato2_p.data;
	*/
	package_t deserializar(uint32_t *pointer,char *stream);

	/**
	* @NAME: assert
	* @DESC: evalua que dos strings sean iguales
	*/
	void anuncio(char * aString);

	/**
	* @NAME: assert
	* @DESC: evalua que dos strings sean iguales
	*/
	void assert(char * aString, char * anotherString);

	/**
	* @NAME: asserti
	* @DESC: evalua que dos ints sean iguales
	*/
	void asserti(int aInt, int anotherInt);

	/**
	* @NAME: charToString
	* @DESC: Pasa un char a un String
	*/
	char* charToString(char element);

	/**
	* @NAME: concat
	* @DESC: concatena n elementos
	*/
	char* concat(int n_args, ...);

	/**
	* @NAME: killme
	* @DESC: mata el proceso actual
	*/
	void killme();

	/**
	* @NAME: obtenerConfiguracion
	* @DESC: devuelve el valor int de un atributo de un archivo de configuracion
	*/
	int obtenerConfiguracion(char *rutaArchivo,char *propiedadConfiguracion);

	/**
	* @NAME: obtenerConfiguracionString
	* @DESC: devuelve el valor string de un atributo de un archivo de configuracion
	*/
	char* obtenerConfiguracionString(char *rutaArchivo,char *propiedadConfiguracion);

	/**
	* @NAME: obtenerConfiguracionArray
	* @DESC: devuelve el valor array de un atributo de un archivo de configuracion
	*/
	char** obtenerConfiguracionArray(char *rutaArchivo,char *propiedadConfiguracion);

	/**
	* @NAME: recvall
	* @DESC: recibe un mensaje a traves de sockets
	*/
	int recvall(uint32_t s, char *buf, uint32_t len);

	/**
	* @NAME: rutaAbsoluta
	* @DESC: devuelve la ruta de la carpeta donde se esta ejecutando la carpeta
	* Si esta en /Debug devolvera un nivel mas arriba de la carpeta actual
	*/
	char * rutaAbsoluta();

	/**
	* @NAME: rutaAbsolutaDe
	* @DESC: concatena rutaAbsoluta() con el nombre de archivo pasado por parametro
	*/
	char * rutaAbsolutaDe(char * archivo);

	/**
	* @NAME: sendall
	* @DESC: envia un mensaje a traves de sockets
	*/
	int sendall(uint32_t s, char *buf, uint32_t *len);

	/**
	* @NAME: test
	* @DESC: imprime por pantalla un string
	*/
	void test(char * aString);

	/**
	* @NAME: testi
	* @DESC: imprime por pantalla un int
	*/
	void testi(int aInt);

#endif /* BLUE4_LIB_H_ */
