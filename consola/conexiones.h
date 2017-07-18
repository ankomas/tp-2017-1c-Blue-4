/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_



#define id_kernel 2

// Defino estructuras administrativas que leo del config
//int socket_kernel;

#include "commons/config.h"

extern t_config * config;

#include <unistd.h>
#include <inttypes.h>
typedef struct{
	char* ip;
	int puerto_kernel;
}datosConfig_t;

/*
typedef struct{
	uint32_t data_size;
	char* data;
}package_t;
*/
struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(int , char* ,int);
int conectarseAlKernel();
datosConfig_t obtenerEstructurasDelConfig();
void crearMenuPrincipal();

/**
 *
 * @param socket_kernel
 * @return pid en string
 */
char* recibirPid(int );

/**
 * Valida si se recibio data.
 * @param socket
 * @param resultadoRecv
 * @return 0 en caso de exito y -1 en caso de error
 */
int validarRecv(int ,int );

/**
 * Recibe el tamanio de lo que me van a enviar.
 * @param socket
 * @return tamanio de lo que envian.
 */
uint32_t recibirTamPaquete(int );

/**
 * Recibe un paquete dado un socket y un tamanio
 * @param socket
 * @param tamanio
 * @param resultadoDelRecv
 * @return devuelve el buffer y por referencia si se lo pudo recibir adecuadamente.
 */
char* recibirPaquete(int ,uint32_t ,int* );


/**
 *
 * @param socket
 * @return sentencia en string (liberar cuando no se use mas!!!).
 */
char* recibirSentenciaAnsisop(int );


#endif /* CONEXIONES_H_ */
