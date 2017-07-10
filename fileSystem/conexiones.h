/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

// Defino estructuras administrativas que leo del config

#include "commons/config.h"

struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(int , char* );
int conectarseAlKernel();

int establecerHandshake(int, char*, int );

int servidor( );

extern t_config *config;


#endif /* CONEXIONES_H_ */
