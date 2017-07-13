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
	int res;
	if(memoria > 0){
		send(memoria,"P",1,0);
		if(recv(memoria,paquete,4,MSG_WAITALL) == 4){
			res=atoi(paquete);
			free(paquete);
			return res;
		}
	}
	printf("No se pudo obtener el tamanio de pagina");
	return -1;
}

int guardarEnMemoria(uint32_t i, uint32_t pid,uint32_t pagina,uint32_t offset,uint32_t tamanioContenido,char*contenido) {
	package_t paquete;
	uint32_t tamOpCode = 1;
	uint32_t tamInt = sizeof(uint32_t);
	char* respuesta = malloc(1);
	//todo hacen falta los siguientes string?
	//sip, en pos de la expresividad
	/*char *streamPID = intToStream(pid);
	char *streamPagina = intToStream(pagina);
	char *streamOffset = intToStream(offset);
	char *streamTamanioContenido = intToStream(tamanioContenido);*/

	//[Identificador del Programa], [#página], [offset], [tamaño] y [buffer]
	printf("GUARDANDO EN MEMORIA:\n");
	printf("PID: %i, PAGINA: %i, OFFSET: %i, TAMANIO: %i\n",pid,pagina,offset,tamanioContenido);
/*
	int wtf;
	char *wtf2=contenido;
	printf("\n");
	for(wtf=0;wtf<256;wtf++)
		printf("%c",wtf2[wtf]);
	printf("\n");
*/
	paquete = serializar(10,
			tamInt,&pid,
			tamInt,&pagina,
			tamInt,&offset,
			tamInt,&tamanioContenido,
			tamanioContenido,contenido
			);
	//char* streamTamPaquete = intToStream(paquete.data_size);

	// Envio de opcode
	/*if(sendall(i,"W",&tamOpCode) < 0)
		return -1;*/
	send(i,"W",1,0);

	// Envio de tamanio de paquete
	/*if(sendall(i, streamTamPaquete, &tamInt) < 0)
		return -1;
	free(streamTamPaquete);*/
	send(i,&paquete.data_size,sizeof(uint32_t),0);

	// Envio de paquete

	/*if(sendall(i, paquete.data, &paquete.data_size) < 0)
		return -1;*/
	send(i,paquete.data,paquete.data_size,0);
	free(paquete.data);
/*
	free(streamPID);
	free(streamPagina);
	free(streamOffset);
	free(streamTamanioContenido);
*/

	if(recv(i,respuesta,1,0) < 1){
		free(respuesta);
		return -1;
	} else {
		if(respuesta[0] == 'N'){
			free(respuesta);
			return -1;
		} else {
			free(respuesta);
			return 0;
		}
	}
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
		free(res);
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

		if(cargarDeMemoria(memoria, pcb->pid,pag+i, offsetPag,size,&paquete)==-1){
			return -1;
		}
		res=realloc(res,paquete.data_size+offset);

		memcpy(res+offset,paquete.data,paquete.data_size);

		free(paquete.data);

		offset+=paquete.data_size;
		offsetPag=0;
		i++;
	}
	//res=realloc(res,off+1);
	memcpy(res+off-1,"\0",1);
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
