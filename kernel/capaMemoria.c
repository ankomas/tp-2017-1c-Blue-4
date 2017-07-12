/*
 * capaMemoria.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "commons/string.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "capaMemoria.h"


int cantidadElementosArrayConfig(char* unaRuta,char*unId){
	int aux = 0;
	char** array = obtenerConfiguracionArray(cfg,unId);
	while (array[aux]){
		aux++;
	}

	return aux;
}

int obtenerTamanioPagina(){
	char*paquete = malloc(4);
	int res;
	if(idUMC > 0){
		send(idUMC,"P",1,0);
		if(recv(idUMC,paquete,4,MSG_WAITALL) == 4){
			res=atoi(paquete);
			free(paquete);
			return res;
		}
	}
	log_error(logger,"No se pudo obtener el tamanio de pagina");
	free(paquete);
	return -1;
}

void inicializarSemaforos() {
	char** sem_ids = obtenerConfiguracionArray(cfg,"SEM_IDS");
	char** sem_init = obtenerConfiguracionArray(cfg,"SEM_INIT");
	int aux = 0;

	while (sem_ids[aux]){
		t_semaforo * nuevoSemaforo = malloc(sizeof(t_semaforo));
		t_queue * nuevaColaEspera = queue_create();
		nuevoSemaforo->valor = (int32_t)sem_init[aux];
		nuevoSemaforo->colaEspera = nuevaColaEspera;
		dictionary_put(semaforos,sem_ids[aux],nuevoSemaforo);
		aux++;
	}
}

void inicializarVariablesCompartidas() {
	char** varCompartidas_ids = obtenerConfiguracionArray(cfg,"SEM_IDS");
	int aux = 0;

	while (varCompartidas_ids[aux]){
		dictionary_put(variablesCompartidas,varCompartidas_ids[aux],0);
		aux++;
	}
}

int guardarEnMemoria(uint32_t i, uint32_t pid,uint32_t pagina,uint32_t offset,uint32_t tamanioContenido,char*contenido) {
	package_t paquete;
	uint32_t tamOpCode = 1;
	uint32_t tamInt = sizeof(uint32_t);
	//todo hacen falta los siguientes string?
	//sip, en pos de la expresividad
	char *streamPID = intToStream(pid);
	char *streamPagina = intToStream(pagina);
	char *streamOffset = intToStream(offset);
	char *streamTamanioContenido = intToStream(tamanioContenido);

	//[Identificador del Programa], [#página], [offset], [tamaño] y [buffer]
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
	char* streamTamPaquete = intToStream(paquete.data_size);

	// Envio de opcode
	if(sendall(i,"W",&tamOpCode) < 0)
		return -1;

	// Envio de tamanio de paquete
	if(sendall(i, streamTamPaquete, &tamInt) < 0)
		return -1;
	free(streamTamPaquete);

	// Envio de paquete

	if(sendall(i, paquete.data, &paquete.data_size) < 0)
		return -1;
	free(paquete.data);

	free(streamPID);
	free(streamPagina);
	free(streamOffset);
	free(streamTamanioContenido);

	char* respuesta = malloc(1);
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

int inicializarEnMemoria(uint32_t i, uint32_t pid,uint32_t paginasNecesarias) {
	package_t paquete;
	uint32_t tamOpCode = 1;
	uint32_t tamInt = sizeof(uint32_t);
	char *streamPID = intToStream(pid);
	char *streamCantPaginas = intToStream(paginasNecesarias);

	paquete = serializar(4,tamInt, streamPID,tamInt, streamCantPaginas);
	char* streamTamPaquete = intToStream(paquete.data_size);

	// Envio de opcode
	if(sendall(i,"I",&tamOpCode) < 0)
		return -1;

	// Envio de tamanio de paquete
	if(sendall(i, streamTamPaquete, &tamInt) < 0)
		return -1;
	free(streamTamPaquete);

	// Envio de paquete
	if(sendall(i, paquete.data, &paquete.data_size) < 0)
		return -1;
	free(paquete.data);

	free(streamPID);
	free(streamCantPaginas);

	char* respuesta = malloc(1);
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

// SYSCALLS Memoria
void leerVarGlobal(uint32_t i){
	// Envio el valor de una variable global

	uint32_t tamInt = sizeof(int32_t);
	uint32_t tamARecibir=0;
	char * rev=NULL;

	// Recibo largo del nombre de la variable
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al recibir un valor de variable global");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");
	rev=realloc(rev,tamARecibir+1);
	// Recibo el nombre de la variable
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al recibir un valor de variable global");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");
	memcpy(rev+tamARecibir,"\0",1);

	int32_t res = *(int32_t*)dictionary_get(variablesCompartidas,rev);
	printf("Rev: %s,tam a recibir: %i\n",rev,tamARecibir);
	printf("Valor de la variable global a enviar: %i\n",*(int32_t*)dictionary_get(variablesCompartidas,rev));
	if(sendall(i, (char*)&res, &tamInt) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");
	free(rev);
	//free(res);
}

void guardarVarGlobal(uint32_t i){
	// Guardo el valor de una variable global

	uint32_t tamInt=sizeof(int32_t);
	uint32_t tamARecibir=0;
	int32_t *nuevoValorVar=malloc(sizeof(int32_t));
	char * rev = malloc(1);

	// Recibo largo del nombre de la variable
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al recibir un valor de variable global");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre de la variable
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al recibir un valor de variable global");

	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");

	// Recibo el valor a asignar
	if(recv(i,nuevoValorVar,tamInt,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");

	printf("Nuevo valor var: %i, rev: %s\n",*nuevoValorVar,rev);
	dictionary_put(variablesCompartidas,rev,nuevoValorVar);

	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");

	free(rev);
}

void semWait(uint32_t i){
	// Wait semaforo

	uint32_t tamARecibir=0;
	char * rev = malloc(1);

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	send(i,"Y",1,0);

	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(semaforoObtenido->valor > 0){
		semaforoObtenido->valor--;
		if(send(i,"Y",1,0))
			anuncio("Ocurrio un problema al hacer un Wait");
	}else {
		t_cpu * cpuEncontrada = encontrarCPU(i);
		uint32_t pid = cpuEncontrada->programaEnEjecucion->pid;
		queue_push(semaforoObtenido->colaEspera,&pid);
	}

	free(rev);

}

void semSignal(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	send(i,"Y",1,0);

	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(queue_size(semaforoObtenido->colaEspera)>0){
		uint32_t *proximoPID = queue_pop(semaforoObtenido->colaEspera);
		t_cpu * cpuEncontrada = encontrarCPUporPID(*proximoPID);

		if(send(i,"Y",cpuEncontrada->id,0))
			anuncio("Ocurrio un problema al hacer un Wait");
	}else {
		semaforoObtenido->valor++;
	}

	free(rev);
}

/*void guardarEnHeap(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	send(i,"Y",1,0);

	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(queue_size(semaforoObtenido->colaEspera)>0){
		uint32_t *proximoPID = queue_pop(semaforoObtenido->colaEspera);
		t_cpu * cpuEncontrada = encontrarCPUporPID(*proximoPID);

		if(send(i,"Y",cpuEncontrada->id,0))
			anuncio("Ocurrio un problema al hacer un Wait");
	}else {
		semaforoObtenido->valor++;
	}

	free(rev);
}

void leerHeap(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	send(i,"Y",1,0);

	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(queue_size(semaforoObtenido->colaEspera)>0){
		uint32_t *proximoPID = queue_pop(semaforoObtenido->colaEspera);
		t_cpu * cpuEncontrada = encontrarCPUporPID(*proximoPID);

		if(send(i,"Y",cpuEncontrada->id,0))
			anuncio("Ocurrio un problema al hacer un Wait");
	}else {
		semaforoObtenido->valor++;
	}

	free(rev);
}*/
// Fin SYSCALLs Memoria

/*int liberarPagina(){

}*/

