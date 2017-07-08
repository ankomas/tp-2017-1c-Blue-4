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
#include "error.h"
#include <sys/socket.h>

const int EXIT_OK = 1;
//El programa finalizó correctamente.

const int EXIT_RESOURCES_NOT_ASSIGNED = -1;
//No se pudieron reservar recursos para ejecutar el programa.

const int EXIT_FILE_DOES_NOT_EXIST = -2;
//El programa intentó acceder a un archivo que no existe.

const int EXIT_UNAUTHORIZED_READ = -3;
//El programa intentó leer un archivo sin permisos.

const int EXIT_UNAUTHORIZED_WRITE = -4;
//El programa intentó escribir un archivo sin permisos.

const int EXIT_MEMORY_EXCEPTION = -5;
//Excepción de memoria.

const int EXIT_CONSOLE_DISCONNECTED = -6;
//Finalizado a través de desconexión de consola.

const int EXIT_CONSOLE_TERMINATED = -7;
//Finalizado a través del comando Finalizar Programa de la consola.

const int EXIT_MAX_SIZE_PAGE_OVERFLOW = -8;
//Se intentó reservar más memoria que el tamaño de una página

const int EXIT_MAX_AMOUNT_PAGES_PROCESS = -9;
//No se pueden asignar más páginas al proceso

const int EXIT_NOT_DEFINED = -20;
//Error sin definición

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
		//if(resultadoGuardarEnMemoria == 0){
			// Guardo las paginas del stack en las paginas siguientes codigo
			//contadorPaginas = 0;
			//todo paginasCodigo?, tamStack es la cantidad de paginas?, hace falta inicializar el stack? o solo reservar las paginas alcanza?
			/*while(contadorPaginas < tamanioStack){
				resultadoGuardarEnMemoria = guardarEnMemoria(idUMC, nuevoProceso->pcb->pid,nuevoProceso->paginasCodigo+1,0,tamanioStack,"");
				contadorPaginas++;
			}*/
			if(resultadoGuardarEnMemoria == 0){
				queue_push(procesosREADY,nuevoProceso);
				error = 0;
			}
		//}
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
	void liberarCPU(t_programa* programaDeCPU){
		eliminarSiHayPROGRAMAs(programaDeCPU);
		eliminarSiHayCPU(cpu->id);
		pthread_exit(&cpu->hilo);
	}

	printf("cpu: %i\n",cpu->id);
	t_programa * proximoPrograma;
	proximoPrograma = planificador(NULL);
	char*res = malloc(1);
	while(1){
		//TODO falta mutex en todos los accesos a las colas
		if(proximoPrograma != 0){
			t_pcb proximoPCB = *(proximoPrograma->pcb);
			package_t paquete = serializarPCB(proximoPCB);
			uint32_t tamUint=sizeof(uint32_t),tamChar=1;
			char* streamTamPaquete = intToStream(paquete.data_size);
			//send al proximoProceso->id
			anuncio("aaaa");
			if(sendall(cpu->id, "0", &tamChar) < 0)
				liberarCPU(proximoPrograma);

			if(sendall(cpu->id, streamTamPaquete, &tamUint) < 0)
				liberarCPU(proximoPrograma);


			if(sendall(cpu->id, paquete.data, &paquete.data_size) < 0)
				liberarCPU(proximoPrograma);

			recv(cpu->id,res,1,MSG_WAITALL);
			if(res[0]!= 'Y')
				liberarCPU(proximoPrograma);

			uint32_t tamARecibir=0;
			recv(cpu->id,res,1,MSG_WAITALL);

			// Verifico si aun le falta ejecutar al proceso
			if(res[0] == 'F'){
				if(proximoPrograma->pcb->exitCode == EXIT_OK){
					anuncio("Programa finalizo con exito");
				} else if(proximoPrograma->pcb->exitCode < 0){
					anuncio(concat(2,"Ocurrio un error #",string_itoa(proximoPrograma->pcb->exitCode)));
				}
				liberarCPU(proximoPrograma);
			} else {
				if(recv(cpu->id,&tamARecibir,sizeof(uint32_t),0) <= 0)
					liberarCPU(proximoPrograma);

				res=realloc(res,tamARecibir);

				printf("Tam a recibir: %i\n",tamARecibir);

				anuncio("PCB RECIBIDO DEL CPU");

				if(recv(cpu->id,res,tamARecibir,0) <= 0)
					liberarCPU(proximoPrograma);
				else
					*(proximoPrograma->pcb)=deserializarPCB(res);

				//TODO El planificador debe desencolar procesos ya terminados
				proximoPrograma = planificador(proximoPrograma);
			}

			// Esta Y debe ser reemplazada por el codigo que devuelva la cpu, cuando finalice tiene que limpiar las estructuras incluyendo cpu
		} else {
			proximoPrograma = planificador(NULL);
		}

		usleep(500000);
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
