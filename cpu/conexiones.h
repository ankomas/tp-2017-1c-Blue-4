/*
 * consola.h
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#ifndef CONEXIONES_H_
#define CONEXIONES_H_

#include "pcb.h"

int kernel;
int memoria;

uint32_t tamPag_global;
uint32_t maxStack_global;
char exit_global;

struct data_socket crearSocket(int, char* );
int enviarMensaje(int ,char*,int );
int conectar(char* , char* ,int );
void cerrarConexion(int );
int pedirAMemoria(t_pcb2* pcb,t_pos pos,int32_t *valor);
int asignarAMemoria(t_pos pos,int32_t valor);
void standby(int );
t_pcb2 testPCB();


#endif /* CONEXIONES_H_ */
