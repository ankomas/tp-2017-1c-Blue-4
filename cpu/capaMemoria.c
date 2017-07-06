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

int pedirTamGlobal(int memoria){
	char*paquete = malloc(4);
	if(memoria > 0){
		send(memoria,"P",1,0);
		if(recv(memoria,paquete,4,MSG_WAITALL) == 4)
			return atoi(paquete);
	}
	printf("No se pudo obtener el tamanio de pagina");
	return -1;
}

int cargarDeMemoria(int socket,uint32_t pid,uint32_t pag, uint32_t off,uint32_t size, package_t* paqueteParametro){
	uint32_t tamARecibir,pointer=0;
	package_t paquete;
	char* res;
	//mandar cop y paquete
	printf("Leyendo de memoria:\n");
	printf("PAG: %i, OFF: %i, SIZE: %i\n",pag,off,size);
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

int substraer(int* from,int size){
	uint32_t res;
	if(*from-size<0){
		res=*from;
		*from=0;
		return res;
	}
	else{
		*from-=size;
		return size;
	}
}

char* pedirLineaAMemoria(t_pcb2* pcb,uint32_t start,uint32_t off){
	package_t paquete;
	int i=0,pag,offsetPag,sizeTotal,size,tamPag=tamPag_global;
	uint32_t offset=0;
	char* res=NULL;

	pag=start/tamPag_global; // 9 / 16 = 0
	offsetPag=start%tamPag_global; // 9 % 16 = 9
	sizeTotal=off; // 9

	while(sizeTotal){
		printf("Pidiendo linea, size restante: %i\n",sizeTotal);
		printf("Socket: %i, PID: %i, tamGlob: %i\n",memoria,pcb->pid,tamPag_global);

		size=substraer(&sizeTotal,tamPag-offsetPag); // 16 - 9 = 7

		cargarDeMemoria(memoria, pcb->pid,pag+i, offsetPag,size,&paquete);
		res=realloc(res,paquete.data_size+offset);

		memcpy(res+offset,paquete.data,paquete.data_size);

		free(paquete.data);

		offset+=paquete.data_size;
		offsetPag=0;
		i++;
	}
	res=realloc(res,off+1);
	memcpy(res+off,"\0",1);
	return res;
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
