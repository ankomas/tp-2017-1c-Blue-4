/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <netdb.h>
#include <blue4-lib.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include "pcb.h"

int kernel;
int memoria;

struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(char* , char* ,int );
void cerrarConexion(int );
void standby(int );
t_pcb2 testPCB();


#endif /* CONEXIONES_H_ */
