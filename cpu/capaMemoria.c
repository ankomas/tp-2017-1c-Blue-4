/*
 * capaMemoria.c
 *
 *  Created on: 3/7/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <netdb.h>

#include <parser/metadata_program.h>
#include <commons/collections/list.h>

#include "pcb.h"
#include "conexiones.h"
#include "capaMemoria.h"

int cargarDeMemoria(int socket,uint32_t pid,uint32_t pag, uint32_t off,uint32_t size, package_t* paqueteParametro){
	uint32_t tamARecibir,pointer=0;
	package_t paquete;
	char* res;
	//mandar cop y paquete
	paquete=serializar(8,
			sizeof(uint32_t),pid,
			sizeof(uint32_t),pag,
			sizeof(uint32_t),off,
			sizeof(uint32_t),size
			);

	send(socket,"R",1,0);
	send(socket,&paquete.data_size,sizeof(uint32_t),0);
	send(socket,paquete.data,paquete.data_size,0);

	free(paquete.data);
	//recibir contenido
	res=malloc(1);

	recv(socket,res,1,0);

	if(res[0]=='N'){
		printf("Error al recibir contenido de la memoria\n");
		return -1;
	}

	recv(socket,&tamARecibir,sizeof(uint32_t),0);
	res=realloc(res,tamARecibir);
	recv(socket,res,tamARecibir,0);

	paquete=deserializar(&pointer,res);

	*paqueteParametro=paquete;

	return 0;
}

char* pedirProgramaAMemoria(t_pcb2 *pcb,int socket){
	package_t paquete;
	t_size size=0,offset=0;
	char* res=NULL;
	int i;

	for(i=0;i<pcb->cantPagCod;i++){
		cargarDeMemoria(socket, pcb->pid,i, 0,tamPag_global,&paquete);

		size+=paquete.data_size;
		res=realloc(res,size);

		memcpy(res+offset,paquete.data,paquete.data_size);
		offset=size;

		free(paquete.data);
	}

	return res;
}
