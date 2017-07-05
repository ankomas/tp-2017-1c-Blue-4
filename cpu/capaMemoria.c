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
			sizeof(uint32_t),&pid,
			sizeof(uint32_t),&pag,
			sizeof(uint32_t),&off,
			sizeof(uint32_t),&size
			);

	if(send(socket,"R",1,0)==-1){
		perror("Error socket memoria");
	}
	if(send(socket,&paquete.data_size,sizeof(uint32_t),0)==-1){
		perror("Error socket memoria");
	}
	if(send(socket,paquete.data,paquete.data_size,0)==-1){
		perror("Error socket memoria");
	}

	free(paquete.data);
	//recibir contenido
	res=malloc(1);

	recv(socket,res,1,0);
	printf("Leyendo de memoria, respuesta: %c\n",res[0]);
	if(res[0]=='N'){
		printf("Error al recibir contenido de la memoria\n");
		return -1;
	}
	/*res=realloc(res,sizeof(uint32_t));
	recv(socket,res,sizeof(uint32_t),0);
	memcpy(&tamARecibir,res,sizeof(uint32_t));*/
	tamARecibir=size;
	printf("Leyendo de memoria, tamanio a recibir: %i\n",tamARecibir);
	res=realloc(res,tamARecibir);
	if(recv(socket,res,tamARecibir,0)<=0){
		printf("Error al recibir'n");
	}

	//paquete=deserializar(&pointer,res);

	paqueteParametro->data_size=size;
	paqueteParametro->data=res;

	return 0;
}

char* pedirProgramaAMemoria(t_pcb2 *pcb,int socket){
	package_t paquete;
	t_size size=0,offset=0;
	char* res=NULL;
	int i;

	for(i=0;i<pcb->cantPagCod;i++){
		printf("Pidiendo codigo, iteracion N° %i\n",i+1);
		printf("Socket: %i, PID: %i, tamGlob: %i\n",socket,pcb->pid,tamPag_global);
		cargarDeMemoria(socket, pcb->pid,i, 0,tamPag_global,&paquete);

		size+=paquete.data_size;
		res=realloc(res,size);

		memcpy(res+offset,paquete.data,paquete.data_size);
		offset=size;

		free(paquete.data);
	}

	return res;
}
