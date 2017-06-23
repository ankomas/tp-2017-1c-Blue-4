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

uint32_t tamPag_global;
uint32_t maxStack_global;

struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(char* , char* ,int );
void cerrarConexion(int );
int pedirAMemoria(t_pos pos);
int asignarAMemoria(pos,variable);
void standby(int );
t_pcb2 testPCB();


#endif /* CONEXIONES_H_ */
