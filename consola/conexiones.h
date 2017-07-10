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


#endif /* CONEXIONES_H_ */
