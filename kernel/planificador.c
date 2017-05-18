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

char *algoritmoPlanificador;

void encolarReady(t_programa* nuevoProceso){
	//Para encolarlo a Ready hay que tener suficiente memoria
	//TODO solicitar memoria deberia pasarle el mensaje entero, no el pid
	if(solicitarMemoria(idUMC, nuevoProceso->id) < 0){
		log_error(logger,"ERROR, el kernel no pudo solicitar memoria correctamente");
	} else {
		queue_push(procesosREADY,nuevoProceso);
	}
}

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

void* cpu(t_cpu * cpu){
	printf("cpu: %i\n",cpu->id);
	t_pcb * proximoProceso;
	while(1){
		//TODO falta mutex en todos los accesos a las colas

		proximoProceso = planificador(NULL);
		//send al proximoProceso->id
		//TODO verificar el recv
		//recv(cpuLibre->id,tamPaquete,3,MSG_WAITALL);
		if(1 == 2){
			//TODO checkear errores en el futuro
			pthread_exit(NULL);
		} else if(1==3){
			//checkear si termino bien para limpiar estructuras
			pthread_exit(NULL);
		} else {
			planificador(proximoProceso);
		}

		usleep(10000000);
	}

	return 0;
}

t_pcb* planificador(t_pcb* unPCB){
	// mutex por haber leido de un archivo que puede ser actualizado hasta antes del recv
	algoritmoPlanificador = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"ALGORITMO");
	printf("algoritmoPlanificador %s\n",algoritmoPlanificador);

	if(unPCB == NULL){
		t_pcb* aux = queue_pop(procesosREADY);
		return aux;
	}

	if(strcmp(algoritmoPlanificador,"RR") == 0){
		if(unPCB->quantumRestante == 0){
			if(queue_size(procesosREADY) > 0){
				t_pcb* aux = queue_pop(procesosREADY);
				aux->quantumRestante--;
				return aux;
			}
		} else {
			unPCB->quantumRestante--;
			return unPCB;
		}
	} else if(strcmp(algoritmoPlanificador,"FIFO") == 0){
		return unPCB;
	} else {
		log_error(logger,"Algoritmo mal cargado al config.cfg");
	}

	return 0;
}
