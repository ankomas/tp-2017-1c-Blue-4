/* capaMemoria.c
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
#include "heapNico.h"


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
		pthread_mutex_lock(&mutex_memoria);
		send(idUMC,"P",1,0);
		if(recv(idUMC,paquete,4,MSG_WAITALL) == 4){
			res=atoi(paquete);
			free(paquete);
			pthread_mutex_unlock(&mutex_memoria);
			return res;
		}
	}
	log_error(logger,"No se pudo obtener el tamanio de pagina");
	free(paquete);
	pthread_mutex_unlock(&mutex_memoria);
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
	int* valInicial=NULL;

	while (varCompartidas_ids[aux]){
		log_info(logger,"Agregado a VARIABLES COMPARTIDAS: %s\n",varCompartidas_ids[aux]);
		valInicial=malloc(sizeof(int));
		*valInicial=0;
		dictionary_put(variablesCompartidas,string_substring_from(varCompartidas_ids[aux],1),valInicial);
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

	pthread_mutex_lock(&mutex_memoria);
	// Envio de opcode
	if(sendall(i,"W",&tamOpCode) < 0){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}

	// Envio de tamanio de paquete
	if(sendall(i, streamTamPaquete, &tamInt) < 0){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}
	free(streamTamPaquete);

	// Envio de paquete

	if(sendall(i, paquete.data, &paquete.data_size) < 0){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}
	free(paquete.data);

	free(streamPID);
	free(streamPagina);
	free(streamOffset);
	free(streamTamanioContenido);

	char* respuesta = malloc(1);
	if(recv(i,respuesta,1,0) < 1){
		free(respuesta);
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	} else {
		if(respuesta[0] == 'N'){
			free(respuesta);
			pthread_mutex_unlock(&mutex_memoria);
			return -1;
		} else {
			free(respuesta);
			pthread_mutex_unlock(&mutex_memoria);
			return 0;
		}
	}
	pthread_mutex_unlock(&mutex_memoria);
}

int cargarDeMemoria(int socket,uint32_t pid,uint32_t pag, uint32_t off,uint32_t size, package_t* paqueteParametro){
	uint32_t tamARecibir,pointer=0;
	package_t paquete;
	char* res;
	//mandar cop y paquete
	log_info(logger,"Leyendo de memoria:\n");
	//log_info("PID: %i, PAG: %i, OFF: %i, SIZE: %i\n",pid,pag,off,size);
	paquete=serializar(8,
			sizeof(uint32_t),&pid,
			sizeof(uint32_t),&pag,
			sizeof(uint32_t),&off,
			sizeof(uint32_t),&size
			);
	pthread_mutex_lock(&mutex_memoria);
	if(send(socket,"R",1,0)==-1){
		pthread_mutex_unlock(&mutex_memoria);
		perror("Error socket memoria");
	}
	if(send(socket,&paquete.data_size,sizeof(uint32_t),0)==-1){
		pthread_mutex_unlock(&mutex_memoria);
		perror("Error socket memoria");
	}
	if(send(socket,paquete.data,paquete.data_size,0)==-1){
		pthread_mutex_unlock(&mutex_memoria);
		perror("Error socket memoria");
	}

	free(paquete.data);
	//recibir contenido
	res=malloc(1);

	recv(socket,res,1,0);
	//log_info("Leyendo de memoria, respuesta: %c\n",res[0]);
	if(res[0]=='N'){
		log_error(logger,"Error al recibir contenido de la memoria\n");
		free(res);
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}
	/*res=realloc(res,sizeof(uint32_t));
	recv(socket,res,sizeof(uint32_t),0);
	memcpy(&tamARecibir,res,sizeof(uint32_t));*/
	tamARecibir=size;
	log_info(logger,"Leyendo de memoria, tamanio a recibir: %i\n",tamARecibir);
	res=realloc(res,tamARecibir);
	if(recv(socket,res,tamARecibir,0)<=0){
		log_error(logger,"Error al recibir'n");
	}

	//paquete=deserializar(&pointer,res);

	paqueteParametro->data_size=size;
	paqueteParametro->data=res;
	pthread_mutex_unlock(&mutex_memoria);
	return 0;
}


int inicializarEnMemoria(uint32_t i, uint32_t pid,uint32_t paginasNecesarias) {
	package_t paquete;
	uint32_t tamOpCode = 1;
	uint32_t tamInt = sizeof(uint32_t);
	char *streamPID = intToStream(pid);
	char *streamCantPaginas = intToStream(paginasNecesarias);

	paquete = serializar(4,tamInt, streamPID,tamInt, streamCantPaginas);
	char* streamTamPaquete = intToStream(paquete.data_size);
	pthread_mutex_lock(&mutex_memoria);
	// Envio de opcode
	if(sendall(i,"I",&tamOpCode) < 0){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}

	// Envio de tamanio de paquete
	if(sendall(i, streamTamPaquete, &tamInt) < 0){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}
	free(streamTamPaquete);

	// Envio de paquete
	if(sendall(i, paquete.data, &paquete.data_size) < 0){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}
	free(paquete.data);

	free(streamPID);
	free(streamCantPaginas);

	char* respuesta = malloc(1);
	if(recv(i,respuesta,1,0) < 1){
		free(respuesta);
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	} else {
		if(respuesta[0] == 'N'){
			free(respuesta);
			pthread_mutex_unlock(&mutex_memoria);
			return -1;
		} else {
			free(respuesta);
			pthread_mutex_unlock(&mutex_memoria);
			return 0;
		}
	}
	pthread_mutex_unlock(&mutex_memoria);
}

int pedirPagias(uint32_t pid,uint32_t cantPaginas){
	package_t paquete;
	char res;
	pthread_mutex_lock(&mutex_memoria);
	log_trace(logger,"Llamada a PEDIR PAGINAS");
	send(idUMC,"A",1,0);
	paquete=serializar(4,
			sizeof(uint32_t),&pid,
			sizeof(uint32_t),&cantPaginas);
	send(idUMC,&paquete.data_size,sizeof(uint32_t),0);
	send(idUMC,paquete.data,paquete.data_size,0);

	free(paquete.data);

	recv(idUMC,&res,1,0);
	printf("PEDIR PAGINAS COP: %c\n",res);
	switch(res){
	case 'Y':
		log_trace(logger,"Llamada a PEDIR PAGINAS correcto");
		pthread_mutex_unlock(&mutex_memoria);
		return 1;
	case 'N':
		log_error(logger,"Llamada a PEDIR PAGINAS no se pudo asignar paginas");
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	default:
		log_error(logger,"Llamada a PEDIR PAGINAS respuesta invalida");
		pthread_mutex_unlock(&mutex_memoria);
		return -2;
	}

}

int liberarPagina(uint32_t pid,uint32_t pagina){
	package_t paquete;
	char res;
	pthread_mutex_lock(&mutex_memoria);
	send(idUMC,"E",1,0);
	paquete=serializar(4,
			sizeof(uint32_t),&pid,
			sizeof(uint32_t),&pagina);
	send(idUMC,&paquete.data_size,sizeof(uint32_t),0);
	send(idUMC,paquete.data,paquete.data_size,0);

	recv(idUMC,&res,1,0);
	free(paquete.data);
	switch(res){
	case 'Y':
		log_trace(logger,"Llamada a ELIMINAR PAGINA correcto");
		pthread_mutex_unlock(&mutex_memoria);
		return 1;
	case 'N':
		log_error(logger,"Llamada a ELIMINAR PAGINA no se pudo eliminar pagina");
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	default:
		log_error(logger,"Llamada a ELIMINAR PAGINA respuesta invalida");
		pthread_mutex_unlock(&mutex_memoria);
		return -2;
	}

}

int finalizarProcesoMemoria(uint32_t pid,bool force){
	pthread_mutex_lock(&mutex_memoria);
	t_programa * aux = encontrarProgramaPorPID(pid);
	if(aux == NULL){
		pthread_mutex_unlock(&mutex_memoria);
		return -1;
	}

	bool _condicion(t_programa * self){
		return self->pcb->pid==pid;
	}

	if(list_find(procesosEXIT->elements,(void*)_condicion) != NULL){
		pthread_mutex_unlock(&mutex_memoria);
		return -1; // El proceso ya estaba finalizado
	}
	if(list_find(procesosEXEC->elements,(void*)_condicion) != NULL){
		// force es para forzar un movimiento de exec a exit, solo se debe ejecutar desde la funcion cpu
		if(!force){
			aux->debeFinalizar = 1;
			aux->pcb->exitCode = -6;
			pthread_mutex_unlock(&mutex_memoria);
			return -2; // No finaliza un proceso hasta que deje de estar en Running
		}
	}
	if(list_find(procesosREADY->elements,(void*)_condicion) != NULL || list_find(procesosNEW->elements,(void*)_condicion) != NULL || list_find(procesosEXEC->elements,(void*)_condicion) != NULL){
		char* charsito = malloc(1);
		if(send(idUMC,"F",1,0)<=0){
			pthread_mutex_unlock(&mutex_memoria);
			return -1;
		}
		if(send(idUMC,&pid,sizeof(uint32_t),0) <=0){
			pthread_mutex_unlock(&mutex_memoria);
			return -1;
		}
		if(recv(idUMC,charsito,1,0) <=0){
			pthread_mutex_unlock(&mutex_memoria);
			return -1;
		}
		if(charsito[0] == 'Y'){
			// puedo continuar, no debo retornar nada aun
		} else {
			pthread_mutex_unlock(&mutex_memoria);
			return -1;
		}
	}
	if(list_find(procesosBLOCK->elements,(void*)_condicion) != NULL){
		moverPrograma(aux,procesosBLOCK,procesosEXIT);
	}

	if(list_find(procesosREADY->elements,(void*)_condicion) != NULL){
		moverPrograma(aux,procesosREADY,procesosEXIT);
	}

	if(list_find(procesosNEW->elements,(void*)_condicion) != NULL){
		moverPrograma(aux,procesosNEW,procesosEXIT);
	}

	if(force){
		if(list_find(procesosEXEC->elements,(void*)_condicion) != NULL){
			moverPrograma(aux,procesosEXEC,procesosEXIT);
		}
	}
	pthread_mutex_unlock(&mutex_memoria);
	return 0;
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
	if(send(i,"Y",1,0) <= 0){
		anuncio("Ocurrio un problema al enviar un valor de variable global");
		return;
	}
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
		if(send(i,"N",1,0) <= 0)
			anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}

	//Existe, sigo

	if(send(i,"Y",1,0) <= 0){
		log_error(logger,"Ocurrio un problema al enviar un valor de variable global");
		return;
	}

	int32_t res = *(int32_t*)dictionary_get(variablesCompartidas,rev);
	log_info(logger,"Valor de la variable global a enviar: %i\n",*(int32_t*)dictionary_get(variablesCompartidas,rev));
	if(sendall(i, (char*)&res, &tamInt) < 0){
		log_error(logger,"Ocurrio un problema al enviar un valor de variable global");
		return;
	}
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
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0){
		anuncio("Ocurrio un problema al recibir un valor de variable global");
		//free(rev);
		//return;
	}
	if(send(i,"Y",1,0) <= 0){
		anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre de la variable
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0){
		anuncio("Ocurrio un problema al recibir un valor de variable global");
		//free(rev);
		//return;
	}

	if(send(i,"Y",1,0) <= 0){
		anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}

	// Recibo el valor a asignar
	if(recv(i,nuevoValorVar,tamInt,MSG_WAITALL) <= 0){
		anuncio("Ocurrio un problema al enviar un valor de variable global");
		//free(rev);
		//return;
	}

	// Verificar si existe la variable
	aux=concat(2,"Llamada a GUARDAR VAR GLOBAL: ",rev);
	log_trace(logger,aux);
	free(aux);
	aux=NULL;

	log_info(logger,"%i,%i\n",dictionary_has_key(variablesCompartidas,rev),dictionary_has_key(variablesCompartidas,"Ca"));
	if(dictionary_has_key(variablesCompartidas,rev)!=true){
		log_error(logger,"La variable global no existe");
		if(send(i,"N",1,0) <= 0)
			anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}

	// Existe, ingresar nuevo valor

	log_info(logger,"Nuevo valor var: %i, rev: %s\n",*nuevoValorVar,rev);
	if(dictionary_has_key(variablesCompartidas,rev)){
		int *aux;
		aux=dictionary_remove(variablesCompartidas,rev);
		free(aux);
	}

	dictionary_put(variablesCompartidas,rev,nuevoValorVar);

	if(send(i,"Y",1,0) <= 0){
		anuncio("Ocurrio un problema al enviar un valor de variable global");
		free(rev);
		return;
	}

	free(rev);
}



bool semWait(uint32_t i,uint32_t pid, t_programa * proximoPrograma){
	// Wait semaforo

	uint32_t tamARecibir=0;
	char * rev = NULL;

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Wait");
	if(send(i,"Y",1,0) < 0){
		anuncio("Ocurrio un problema al hacer un Wait");
		return 0;
	}
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);
	// Recibo el nombre del semaforo
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0){
		anuncio("Ocurrio un problema al hacer un Wait");
	}
	if(send(i,"Y",1,0)< 0){
		anuncio("Ocurrio un problema al hacer un wait");
		return 0;
	}
	//printf("Llamada a SEM WAIT: %s\n",rev);
	//pthread_mutex_lock(&mutex_semaforos);
	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(dictionary_has_key(semaforos,rev)){
		testi(i);
		test("Valor semaforo en Wait antes de decrementar");
		testi(semaforoObtenido->valor);
		semaforoObtenido->valor--;
		printf("Nuevo valor de %s: %i\n",rev,semaforoObtenido->valor);
		if(semaforoObtenido->valor >= 0){
			if(send(i,"Y",1,0) <= 0){
				anuncio("Ocurrio un problema al hacer un Wait");
				return 0;
			}
		}else {
			//t_cpu * cpuEncontrada = encontrarCPU(i);
			//uint32_t pid = cpuEncontrada->programaEnEjecucion->pid;
			queue_push(semaforoObtenido->colaEspera,pid);
			char * string = concat(3,"Moviendo el proceso ",string_itoa(pid)," de EXEC a bloqueado");
			log_trace(logger,string);
			free(string);
			moverPrograma(proximoPrograma,procesosEXEC,procesosBLOCK);

			if(send(i,"B",1,0) <= 0){
				anuncio("Ocurrio un problema al hacer un Wait");
				return 0;
			}
			//
			char* res = malloc(1);
			recv(i,res,1,MSG_WAITALL);
			if(res[0] == 'B'){
				if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0){
					//liberarCPU(cpu,proximoPrograma);
				}
				res=realloc(res,tamARecibir);
				if(recv(i,res,tamARecibir,MSG_WAITALL) <= 0){
					//liberarCPU(cpu,proximoPrograma);
				}else{
					liberarPCB(*(proximoPrograma->pcb));
					*(proximoPrograma->pcb)=deserializarPCB(res);
					free(res);
					res=NULL;
				}

				free(rev);
				//pthread_mutex_unlock(&mutex_semaforos);
				return 1;
			}
			//
		}
	} else {
		if(send(i,"N",1,0) <= 0){
			anuncio("Ocurrio un problema al hacer un Wait");
			return 0;
		}
	}
	free(rev);
	return 0;

}

void semSignal(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = NULL;

	// Recibo largo del nombre del semaforo
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Signal");
	if(send(i,"Y",1,0) <= 0){
		anuncio("Ocurrio un problema al hacer un Signal");
		return;
	}
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo el nombre del semaforo
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		anuncio("Ocurrio un problema al hacer un Signal");
	if(send(i,"Y",1,0)<=0){
		anuncio("Ocurrio un problema al hacer un Signal");
		return;
	}

	printf("Llamada a SEM SIGNAL: %s\n",rev);
	//pthread_mutex_lock(&mutex_semaforos);
	t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
	if(dictionary_has_key(semaforos,rev)){
		testi(i);
		test("Valor semaforo en Signal antes de aumentar");
		testi(semaforoObtenido->valor);
		semaforoObtenido->valor++;
		printf("Nuevo valor de %s: %i\n",rev,semaforoObtenido->valor);
		if(queue_size(semaforoObtenido->colaEspera)>0){
			uint32_t *proximoPID = (uint32_t*)queue_pop(semaforoObtenido->colaEspera);
			printf("PID COLA SEM: %i\n",proximoPID);
			t_programa * programaAux =  encontrarProgramaPorPID(proximoPID);
			if(programaAux == NULL){
				test("NULL ALERT");
			}
			char * string = concat(3,"Moviendo el proceso ",string_itoa(programaAux->pcb->pid)," de BLOCK a READY");
			log_trace(logger,string);
			free(string);
			moverPrograma(programaAux,procesosBLOCK,procesosREADY);
		}

		if(send(i,"Y",1,0) <= 0){
			anuncio("Ocurrio un problema al hacer un Signal");
			return;	
		}
	} else {
		if(send(i,"N",1,0) <= 0){
			anuncio("Ocurrio un problema al hacer un Signal");
			return;
		}
	}
	free(rev);
	//pthread_mutex_unlock(&mutex_semaforos);
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

int guardarHeapNico(uint32_t socket,t_programa* programa){
	t_puntero res;
	t_valor_variable tam;
	printf("Llamada a GUARDAR HEAP NICO\n");
	recv(socket,&tam,sizeof(t_valor_variable),0);

	res=allocEnHeap(programa,tam);

	if(res==-1){
		printf("ERROR: tamanio mas grande que el de pagina\n");
		send(socket,"N",1,0);
		return 1;
	}
	if(res==-2){
		printf("ERROR: no se pudo asignar paginas al proceso\n");
		send(socket,"N",1,0);
		return 1;
	}

	programa->cantidadAlocarEjecutados++;
	programa->cantidadAlocarEjecutadosBytes += tam;

	send(socket,"Y",1,0);
	send(socket,&res,sizeof(uint32_t),0);
	return 1;
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

	pthread_mutex_lock(&mutex_semaforos);
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
	pthread_mutex_unlock(&mutex_semaforos);
}

void liberarHeapNico(int cpu,t_programa *proximoPrograma){
	uint32_t ptr;
	int res;
	char respuesta;
	recv(cpu,&ptr,sizeof(uint32_t),0);
	res=liberar(proximoPrograma,ptr);
	if(res==1)
		respuesta='Y';
	send(cpu,&respuesta,1,0);
}
// Fin SYSCALLs Memoria

/*int liberarPagina(){

}*/



