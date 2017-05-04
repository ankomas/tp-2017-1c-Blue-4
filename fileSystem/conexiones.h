/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

// Defino estructuras administrativas que leo del config

struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(int , char* );
int conectarseAlKernel();

int establecerHandshake(int, char*, int );

int servidor( );


#endif /* CONEXIONES_H_ */
