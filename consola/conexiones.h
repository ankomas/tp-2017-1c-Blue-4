/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

// Defino estructuras administrativas que leo del config


typedef struct{
	char* ip;
	int puerto_kernel;
}datosConfig_t;


struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(int , char* ,int);
int conectarseAlKernel(int);
datosConfig_t obtenerEstructurasDelConfig();


#endif /* CONEXIONES_H_ */
