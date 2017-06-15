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

int solicitarMemoria(uint32_t i, uint32_t data,uint32_t data2) {
	package_t paquete;
	uint32_t bytes=-1;
	char* opcode= malloc(1);
	opcode[1] = 'A';
	paquete = serializar(6,1,&opcode, sizeof(uint32_t), string_itoa(data),sizeof(uint32_t), string_itoa(data2));
	bytes = sendall(i, &paquete.data, &paquete.data_size);
	testi(bytes);
	//printf("envie %i bytes en el %i send\n", bytes, contador++);
	/*bytes = send(i, &paquete.data_size, sizeof(uint32_t), 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);
	bytes = send(i, paquete.data, paquete.data_size, 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);*/

	char* respuesta = malloc(1);
	free(paquete.data);
	if(recv(i,respuesta,1,0) < 1){
		return -1;
	} else {
		if(respuesta[0] == 'N'){
			return -1;
		} else {
			return 0;
		}
	}
}

