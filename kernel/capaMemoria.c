/*
 * capaMemoria.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
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

int valorSemaforo(char * unSemaforo){
	char** sem_ids = obtenerConfiguracionArray(rutaAbsolutaDe("config.cfg"),"SEM_IDS");
	int aux = 0;


	while (strcmp(sem_ids[aux],unSemaforo) != 0){
		aux++;
	}

	return aux;
}

int solicitarMemoria(uint32_t i, uint32_t data) {
	package_t paquete;
	uint32_t bytes=-1, contador=1;

	paquete = serializar(2, sizeof(uint32_t), &data);

	bytes = send(i, "A", 1, 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);
	bytes = send(i, &paquete.data_size, sizeof(uint32_t), 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);
	bytes = send(i, paquete.data, paquete.data_size, 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);

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

