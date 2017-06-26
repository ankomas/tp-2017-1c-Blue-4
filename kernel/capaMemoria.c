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
	char** array = obtenerConfiguracionArray(unaRuta,unId);
	while (array[aux]){
		aux++;
	}

	return aux;
}

int obtenerTamanioPagina(){
	char*paquete = malloc(4);
	if(idUMC > 0){
		send(idUMC,"P",1,0);
		if(recv(idUMC,paquete,4,MSG_WAITALL) == 4)
			return atoi(paquete);
	}
	log_error(logger,"No se pudo obtener el tamanio de pagina");
	return -1;
}

void inicializarSemaforos() {
	char** sem_ids = obtenerConfiguracionArray(rutaAbsolutaDe("config.cfg"),"SEM_IDS");
	char** sem_init = obtenerConfiguracionArray(rutaAbsolutaDe("config.cfg"),"SEM_INIT");
	int aux = 0;

	while (sem_ids[aux]){
		dictionary_put(semaforos,sem_ids[aux],sem_init[aux]);
		aux++;
	}
}

void inicializarVariablesCompartidas() {
	char** varCompartidas_ids = obtenerConfiguracionArray(rutaAbsolutaDe("config.cfg"),"SEM_IDS");
	int aux = 0;

	while (varCompartidas_ids[aux]){
		dictionary_put(semaforos,varCompartidas_ids[aux],0);
		aux++;
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
		return -1;
	} else {
		if(respuesta[0] == 'N'){
			return -1;
		} else {
			return 0;
		}
	}
	free(respuesta);
}

/*int liberarPagina(){

}*/

