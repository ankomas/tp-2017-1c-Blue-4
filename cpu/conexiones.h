/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(char* , char* ,int );



#endif /* CONEXIONES_H_ */
