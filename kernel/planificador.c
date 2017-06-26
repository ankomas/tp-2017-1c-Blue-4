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
	uint32_t paginasNecesarias = nuevoProceso->paginasCodigo+tamanioStack;
	uint32_t error = 1;
	//Para encolarlo a Ready hay que tener suficiente memoria
	if(inicializarEnMemoria(idUMC, nuevoProceso->pcb->pid,paginasNecesarias) >= 0){
		// Guardo las paginas del codigo
		int resultadoGuardarEnMemoria = -1;
		int contadorPaginas = 0;
		while(contadorPaginas < nuevoProceso->paginasCodigo){
			resultadoGuardarEnMemoria = guardarEnMemoria(idUMC, nuevoProceso->pcb->pid,0,0,nuevoProceso->paginasCodigo,nuevoProceso->codigo);
			contadorPaginas++;
		}
		if(resultadoGuardarEnMemoria == 0){
			// Guardo las paginas del stack en las paginas siguientes codigo
			contadorPaginas = 0;
			while(contadorPaginas < nuevoProceso->paginasCodigo){
				resultadoGuardarEnMemoria = guardarEnMemoria(idUMC, nuevoProceso->pcb->pid,nuevoProceso->paginasCodigo+1,0,tamanioStack,"");
				contadorPaginas++;
			}
			if(resultadoGuardarEnMemoria == 0){
				queue_push(procesosREADY,nuevoProceso);
				error = 0;
			}
		}
	}
	if(error == 1){
		log_error(logger,"ERROR, el kernel no pudo solicitar memoria correctamente");
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

t_programa* planificador(t_programa* unPrograma){
	// mutex por haber leido de un archivo que puede ser actualizado hasta antes del recv
	algoritmoPlanificador = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"ALGORITMO");
	printf("algoritmoPlanificador %s\n",algoritmoPlanificador);

	if(unPrograma == NULL){
		t_pcb* aux = queue_pop(procesosREADY);
		return aux;
	}

	if(strcmp(algoritmoPlanificador,"RR") == 0){
		if(unPrograma->quantumRestante == 0){
			if(queue_size(procesosREADY) > 0){
				t_programa* aux = queue_pop(procesosREADY);
				aux->quantumRestante--;
				return aux;
			}
		} else {
			unPrograma->quantumRestante--;
			return unPrograma;
		}
	} else if(strcmp(algoritmoPlanificador,"FIFO") == 0){
		return unPrograma;
	} else {
		log_error(logger,"Algoritmo mal cargado al config.cfg");
	}

	return 0;
}
