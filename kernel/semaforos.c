/*
 * semaforos.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#include "semaforos.h"

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
