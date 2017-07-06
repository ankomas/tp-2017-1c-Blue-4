/*
 * capaKernel.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */
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

#include "conexiones.h"
#include "primitivas.h"
#include "capaMemoria.h"
#include "capaKernel.h"

char *const lineaEnPrograma(t_pcb2* pcb,t_puntero_instruccion inicioDeLaInstruccion, t_size tamanio) {
	char* aRetornar;
	//cargarDeMemoria(socket, pcb->pid,i, 0,tamPag_global,&paquete);
	return aRetornar;
}

void ejecutarPCB(t_pcb2 *pcb, int socket){
	//char* programa=pedirProgramaAMemoria(pcb,socket);
	char* const linea = lineaEnPrograma(pcb,
			pcb->indiceCodigo[pcb->pc].start,
			pcb->indiceCodigo[pcb->pc].offset);

	printf("\t Evaluando -> %s", linea);
	//analizadorLinea(linea, &funciones, &kernel_functions);
	free(linea);
	pcb->pc++;
}

void recibirPCB(int socket){
	uint32_t tam;
	char* buffer;
	package_t paquete;

	buffer=malloc(sizeof(uint32_t));
	if(recv(socket,buffer,4,0)==-1){
		perror("Error al recibir tamanio en recibirPCB");
		return;
	}
	memcpy(&tam,buffer,4);
	buffer=realloc(buffer,tam);
	recv(socket,buffer,tam,0);

	pcb_global=deserializarPCB(buffer);
	free(buffer);

	ejecutarPCB(&pcb_global,socket);

}

