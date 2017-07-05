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
	uint32_t offset=0;
	//Para encolarlo a Ready hay que tener suficiente memoria
	if(inicializarEnMemoria(idUMC, nuevoProceso->pcb->pid,paginasNecesarias) >= 0){
		// Guardo las paginas del codigo
		int resultadoGuardarEnMemoria = -1;
		int contadorPaginas = 0;
		while(contadorPaginas < nuevoProceso->paginasCodigo){
			resultadoGuardarEnMemoria = guardarEnMemoria(idUMC, nuevoProceso->pcb->pid,contadorPaginas,0,tamanioPagina,nuevoProceso->codigo+offset);
			contadorPaginas++;
			offset+=tamanioPagina;
		}
		if(resultadoGuardarEnMemoria == 0){
			// Guardo las paginas del stack en las paginas siguientes codigo
			contadorPaginas = 0;
			//todo paginasCodigo?, tamStack es la cantidad de paginas?, hace falta inicializar el stack? o solo reservar las paginas alcanza?
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
	t_programa * proximoPrograma;
	proximoPrograma = planificador(NULL);
	while(1){
		//TODO falta mutex en todos los accesos a las colas
		if(proximoPrograma != 0){
			t_pcb proximoPCB = *(proximoPrograma->pcb);
			package_t paquete = serializarPCB(proximoPCB);
			char* streamTamPaquete = intToStream(paquete.data_size);
			//send al proximoProceso->id
			send(cpu->id,"0",1,0);

			if(sendall(cpu->id, streamTamPaquete, &paquete.data_size) < 0)
				return 0;
			if(sendall(cpu->id, paquete.data, &paquete.data_size) < 0)
				return 0;

			//TODO verificar el recv
			//recv(cpuLibre->id,tamPaquete,3,MSG_WAITALL);
			if(1 == 2){
				//TODO checkear errores en el futuro
				pthread_exit(NULL);
			} else if(1==3){
				//checkear si termino bien para limpiar estructuras
				pthread_exit(NULL);
			} else {
				proximoPrograma = planificador(proximoPrograma);
			}
		} else {
			proximoPrograma = planificador(NULL);
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
		if(queue_size(procesosREADY) > 0){
			t_programa* aux = queue_pop(procesosREADY);
			return aux;
		} else {
			return 0;
		}
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
