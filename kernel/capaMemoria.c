/*
 * capaMemoria.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

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
