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
#include "heap.h"


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
		nuevoSemaforo->valor = atoi(sem_init[aux]);
		nuevoSemaforo->colaEspera = nuevaColaEspera;
		dictionary_put(semaforos,sem_ids[aux],nuevoSemaforo);
		aux++;
	}
}

void inicializarVariablesCompartidas() {
	char** varCompartidas_ids = obtenerConfiguracionArray(cfg,"SHARED_VARS");
	int aux = 0;

	while (varCompartidas_ids[aux]){
		printf("Agregado a VARIABLES COMPARTIDAS: %s\n",varCompartidas_ids[aux]);
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

int pedirPagias(uint32_t pid,uint32_t cantPaginas){
	package_t paquete;
	char res;

	log_trace(logger,"Llamada a PEDIR PAGINAS");
	send(idUMC,"A",1,0);
	paquete=serializar(4,
			sizeof(uint32_t),&pid,
			sizeof(uint32_t),&cantPaginas);
	send(idUMC,&paquete.data_size,sizeof(uint32_t),0);
	send(idUMC,paquete.data,paquete.data_size,0);

	free(paquete.data);

	recv(idUMC,&res,1,0);

	switch(res){
	case 'Y':
		log_trace(logger,"Llamada a PEDIR PAGINAS correcto");
		return 1;
	case 'N':
		log_error(logger,"Llamada a PEDIR PAGINAS no se pudo asignar paginas");
		return -1;
	default:
		log_error(logger,"Llamada a PEDIR PAGINAS respuesta invalida");
		return -2;
	}

}

// SYSCALLS Memoria
void leerVarGlobal(uint32_t i){
	// Envio el valor de una variable global

	uint32_t tamInt = sizeof(int32_t);
	uint32_t tamARecibir=0;
	char * rev=NULL,*aux=NULL;

	// Recibo largo del nombre de la variable
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al recibir un valor de variable global");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");
	rev=realloc(rev,tamARecibir+1);
	// Recibo el nombre de la variable
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al recibir un valor de variable global");
	memcpy(rev+tamARecibir,"\0",1);

	//Verificar si existe la variable
	aux=concat(2,"Llamada a LEER VAR GLOBAL: ",rev);
	log_trace(logger,aux);
	free(aux);
	aux=NULL;

	if(dictionary_has_key(variablesCompartidas,rev)!=true){
		log_error(logger,"La variable global no existe");
		if(send(i,"N",1,0) < 0)
			anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}

	//Existe, sigo

	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");

	int32_t res = *(int32_t*)dictionary_get(variablesCompartidas,rev);
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
	char * rev = malloc(1),*aux=NULL;

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

	// Verificar si existe la variable
	aux=concat(2,"Llamada a GUARDAR VAR GLOBAL: ",rev);
	log_trace(logger,aux);
	free(aux);
	aux=NULL;

	printf("%i,%i\n",dictionary_has_key(variablesCompartidas,rev),dictionary_has_key(variablesCompartidas,"Ca"));
	if(dictionary_has_key(variablesCompartidas,rev)!=true){
		log_error(logger,"La variable global no existe");
		if(send(i,"N",1,0) < 0)
			anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}

	// Existe, ingresar nuevo valor

	printf("Nuevo valor var: %i, rev: %s\n",*nuevoValorVar,rev);
	if(dictionary_has_key(variablesCompartidas,rev)){
		dictionary_remove(variablesCompartidas,rev);
	}

	dictionary_put(variablesCompartidas,rev,nuevoValorVar);

	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al enviar un valor de variable global");

	free(rev);
}



void semWait(uint32_t i,uint32_t *pid){
	// Wait semaforo

	uint32_t tamARecibir=0;
	char * rev = NULL;

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);
	// Recibo el nombre del semaforo
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	send(i,"Y",1,0);
	printf("Llamada a SEM WAIT: %s\n",rev);
	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(dictionary_has_key(semaforos,rev)){
		test("Valor semaforo en Wait antes de decrementar");
		testi(semaforoObtenido->valor);
		semaforoObtenido->valor--;
		printf("Nuevo valor de %s: %i\n",rev,semaforoObtenido->valor);
		if(semaforoObtenido->valor >= 0){
			if(send(i,"Y",1,0) <= 0)
				anuncio("Ocurrio un problema al hacer un Wait");
		}else {
			//t_cpu * cpuEncontrada = encontrarCPU(i);
			//uint32_t pid = cpuEncontrada->programaEnEjecucion->pid;

			queue_push(semaforoObtenido->colaEspera,pid);
			/*uint32_t *proximoPID = queue_peek(semaforoObtenido->colaEspera);
			printf("PID COLA SEM INGRESADO: %i\n",*proximoPID);*/
			t_programa* programaAux = encontrarPrograma(*pid);
			//moverPrograma(programaAux,procesosREADY,procesosBLOCK);

			if(send(i,"B",1,0) <= 0)
				anuncio("Ocurrio un problema al hacer un Wait");
		}
	} else {
		if(send(i,"N",1,0) <= 0)
			anuncio("Ocurrio un problema al hacer un Wait");
	}


	free(rev);

}

void semSignal(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = NULL;

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Signal");
	if(send(i,"Y",1,0) < 0)
		anuncio("Ocurrio un problema al hacer un Signal");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Signal");
	send(i,"Y",1,0);

	printf("Llamada a SEM SIGNAL: %s\n",rev);
	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(dictionary_has_key(semaforos,rev)){
		test("Valor semaforo en Signal antes de aumentar");
			testi(semaforoObtenido->valor);
		semaforoObtenido->valor++;
		printf("Nuevo valor de %s: %i\n",rev,semaforoObtenido->valor);
		if(queue_size(semaforoObtenido->colaEspera)>0){
			uint32_t *proximoPID = (uint32_t*)queue_pop(semaforoObtenido->colaEspera);
			printf("PID COLA SEM: %i\n",*proximoPID);
			t_programa * programaAux =  encontrarPrograma(*proximoPID);
			printf("DESBLOQUEANDO PROCESO\n");
			printf("PAUX: %i\n",programaAux->pcb->pid);
			moverPrograma(programaAux,procesosBLOCK,procesosREADY);
		}

		if(send(i,"Y",1,0) <= 0)
			anuncio("Ocurrio un problema al hacer un Signal");
	} else {
		if(send(i,"N",1,0) <= 0)
			anuncio("Ocurrio un problema al hacer un Signal");
	}


	free(rev);
}

void guardarEnHeap(uint32_t i,t_list * paginasHeap,uint32_t *pid){
	uint32_t tamARecibir=0;
	char * rev = NULL;

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		log_error(logger,"Ocurrio un problema al tratar de guardar memoria en el Heap");
	if(send(i,"Y",1,0) < 0)
		log_error(logger,"Ocurrio un problema al tratar de guardar memoria en el Heap");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		log_error(logger,"Ocurrio un problema al tratar de guardar memoria en el Heap");
	send(i,"Y",1,0);

	//Trato de meterlo en alguna pagina de heap que ya tenia asignada
	int contador = 0;
	while(contador < list_size(paginasHeap)){
		paginaHeap * unaPaginaCargada = list_get(paginasHeap,contador);
		bloque * bloqueEncontrado = guardarDataHeap(unaPaginaCargada,rev,tamARecibir);
		if(bloqueEncontrado != NULL){
			// Se pudo guardar en una pagina previamente cargada
			char* metadataSerializado = calloc(1,sizeof(heapMetadata));
			memcpy(metadataSerializado,&bloqueEncontrado->metadata->isFree,sizeof(bloqueEncontrado->posicionInicioBloque));
			memcpy(metadataSerializado,&bloqueEncontrado->metadata->size,sizeof(bloqueEncontrado->metadata->size));
			guardarEnMemoria(i,*pid,unaPaginaCargada->numero,bloqueEncontrado->posicionInicioBloque,sizeof(heapMetadata),metadataSerializado);
			free(metadataSerializado);
			return;
		}
	}


	paginaHeap * unaPagina = malloc(sizeof(unaPagina));
	uint32_t numeroPagina = inicializarEnMemoria(idUMC, *pid,1);
	if(numeroPagina < 0){
		log_error(logger,"La memoria no dio espacio al kernel para guardar espacio de Heap");
		send(i,"N",1,0);
		return;
	}
	unaPagina->numero = numeroPagina;
	iniciarBloqueHeap(unaPagina);
	if(guardarDataHeap(unaPagina,rev,tamARecibir) == 0){
		// Se pudo guardar en una pagina previamente cargada
		return;
	} else {
		log_error(logger,"No se pudo reservar espacio en el heap");
		return;
	}
}

void leerHeap(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = NULL;

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
// Fin SYSCALLs Memoria

/*int liberarPagina(){

}*/

