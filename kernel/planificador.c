/*
 * planificador.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "planificador.h"
#include "main.h"

t_cpu* indiceProximaCPULibre(){
	int indice = 0;
	t_cpu * CPUaux;
	if(list_size(CPUs) >0){
		CPUaux = list_get(CPUs,indice);

		while(CPUaux->disponible != true){
			CPUaux = list_get(CPUs,indice);
			indice++;
		}

		if(CPUaux->disponible != true){
			return NULL;
		}

		return CPUaux;
	}
	return NULL;
}

void FIFO(){


}

void RR(){

}

void* cpu(t_cpu * cpu){
	printf("cpu: %i\n",cpu->id);
	usleep(10000000);
	pthread_exit(NULL);
	return 0;
}

void *planificador(){
	char *algoritmoPlanificador = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"ALGORITMO");

	while(1){
		printf("algoritmoPlanificador %s\n",algoritmoPlanificador);
		//TODO falta mutex en todos los accesos a las colas
		//t_pcb * proximoProceso = queue_pop(procesosREADY);
		t_cpu* cpuLibre = indiceProximaCPULibre();
		//if(indiceProximaCPULibre() != NULL){
			//sendall(cpuLibre->id,"pcb",(uint32_t*)3);
			//cpuLibre->disponible = false;
		//}
		//TODO verificar el recv
		//recv(cpuLibre->id,tamPaquete,3,MSG_WAITALL);


		usleep(retardo);
	}

	return 0;
}
