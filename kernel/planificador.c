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

t_cpu * encontrarCPU(uint32_t i){
	int contador = 0;
	if(list_size(CPUs) > 0){
		t_cpu * cpuAux;
		while(contador < list_size(CPUs)){
			cpuAux = list_get(CPUs,contador);
			if(cpuAux->id == i)
				return cpuAux;
			contador++;
		}
	}
	return NULL;
}

t_programa * encontrarPrograma(uint32_t i){
	int contador = 0;
	if(list_size(PROGRAMAs) > 0){
		t_programa * programaAux;
		while(contador < list_size(PROGRAMAs)){
			programaAux = list_get(PROGRAMAs,contador);
			if(programaAux->id == i)
				return programaAux;
			contador++;
		}
	}
	return NULL;
}

t_cpu * encontrarCPUporPID(uint32_t pid){
	int contador = 0;
	if(list_size(CPUs) > 0){
		t_cpu * cpuAux;
		while(contador < list_size(CPUs)){
			cpuAux = list_get(CPUs,contador);
			if(cpuAux->programaEnEjecucion->pid == pid)
				return cpuAux;
			contador++;
		}
	}
	return NULL;
}

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

t_programa * inicializarPrograma(uint32_t i,uint32_t pidActual){
	//Recibo codigo
	printf("Codigo OP: A\n");
	//char* tamanioCodigoString = malloc(4/*+1*/);
	uint32_t tamanioCodigo;/* = atoi(tamanioCodigoString);*/
	//memset(tamanioCodigoString,0,5);
	if(recv(i,&tamanioCodigo,4,MSG_WAITALL) < 0)
		printf("Error al recibir el tamanio de codigo");

	//memcpy(&tamanioCodigo,tamanioCodigoString,4);
	printf("Tamanio codigo de %i: %i\n",i,tamanioCodigo);
	char* codigo;/* = malloc(tamanioCodigo);*/

	//todo verificar logica
	//reserva cant paginas * tamanio pagina, llena el contenido de ceros

	if(tamanioCodigo%tamanioPagina!=0)
		codigo=calloc(1,((tamanioCodigo/tamanioPagina)+1)*tamanioPagina);
	else
		codigo=calloc(1,(tamanioCodigo/tamanioPagina)*tamanioPagina);
	//codigo=calloc(1,tamanioCodigo);

	if(recv(i,codigo,tamanioCodigo,MSG_WAITALL) < 0)
		printf("Error al recibir el codigo");

	//Envio PID a consola
	enviarPID(i);

	// Inicializo el Programa y su PCB
	t_pcb * nuevoPCB;/* = malloc(sizeof(t_pcb))*/
	t_programa * nuevoProceso = malloc(sizeof(t_programa));

	// Inicializo Metadata
	t_metadata_program* metadata=metadata_desde_literal(codigo);

	// Creo PCB y libero metadata
	nuevoPCB=crearPCB(metadata,pidActual);
	metadata_destruir(metadata);

	nuevoProceso->id = i;
	nuevoProceso->tablaArchivos = NULL;
	nuevoProceso->paginasHeap = dictionary_create();
	nuevoProceso->quantumRestante = quantum;
	nuevoProceso->pcb = nuevoPCB;

	uint32_t cantidadPaginasCodigo = 0;
	if(tamanioPagina == -1)
		anuncio("No se pueden cargar procesos porque la memoria no esta conectada");
	else if(tamanioPagina == 0)
		anuncio("El tamanio de una pagina no puede ser 0");
	else{
		//todo verificar logica
		if(tamanioCodigo%tamanioPagina!=0)
			cantidadPaginasCodigo = (tamanioCodigo/tamanioPagina)+1;
		else
			cantidadPaginasCodigo = tamanioCodigo/tamanioPagina;
	}

	printf("PID: %i, Cantidad de paginas de codigo: %i\n",nuevoPCB->pid,cantidadPaginasCodigo);

	if(cantidadPaginasCodigo == 0)
		killme();

	nuevoProceso->codigo = codigo;
/*
	int wtf;
	printf("\n");
	for(wtf=0;wtf<535;wtf++)
		printf("%c",nuevoProceso->codigo[wtf]);
	printf("\n");
	*/
	nuevoProceso->paginasCodigo = cantidadPaginasCodigo;
	nuevoProceso->pcb->cantPagCod= cantidadPaginasCodigo;

	if(gradoMultiprogramacion > cantidadProgramasEnSistema){
		encolarReady(nuevoProceso);
	}else{
		/*if(send(i,"N",1,0) < 1)
			log_error(logger,"ERROR, el kernel no le pudo enviar el mensaje de que no es posible crear un nuevo programa");*/
		queue_push(procesosNEW,nuevoProceso);
		log_info(logger,"No se pueden aceptar mas programas debido al grado de multiprogramacion definido. Encolando en NEW...");
	}
	cantidadProgramasEnSistema++;
	list_add(PROGRAMAs,nuevoProceso);
	return nuevoProceso;
}

void* cpu(t_cpu * cpu){
	void liberarCPU(t_programa* programaDeCPU){
		moverPrograma(programaDeCPU,procesosEXEC,procesosEXIT);
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
			uint32_t tamARecibir=0;
			//char* streamTamPaquete = intToStream(paquete.data_size);
			//send al proximoProceso->id
			if(sendall(cpu->id, "0", &tamChar) < 0)
				liberarCPU(proximoPrograma);

			if(sendall(cpu->id, (char*)&paquete.data_size, &tamUint) < 0)
				liberarCPU(proximoPrograma);

			if(sendall(cpu->id, paquete.data, &paquete.data_size) < 0)
				liberarCPU(proximoPrograma);

			free(paquete.data);

			recv(cpu->id,res,1,MSG_WAITALL);
			if(res[0]!= 'Y')
				liberarCPU(proximoPrograma);

			recv(cpu->id,res,1,MSG_WAITALL);

			// Verifico si aun le falta ejecutar al proceso
			if(res[0] == 'F'){
				if(proximoPrograma->pcb->exitCode == EXIT_OK){
					anuncio("Programa finalizo con exito");
				} else if(proximoPrograma->pcb->exitCode < 0){
					anuncio(concat(2,"Ocurrio un error #",string_itoa(proximoPrograma->pcb->exitCode)));
				}
				moverPrograma(proximoPrograma,procesosEXEC,procesosEXIT);
				proximoPrograma = NULL;
			} else if(res[0] == 'Y'){
				if(recv(cpu->id,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
					liberarCPU(proximoPrograma);

				res=realloc(res,tamARecibir);
				printf("Tam a recibir: %i\n",tamARecibir);
				anuncio("PCB RECIBIDO DEL CPU");
				if(recv(cpu->id,res,tamARecibir,MSG_WAITALL) <= 0)
					liberarCPU(proximoPrograma);
				else{
					liberarPCB(*(proximoPrograma->pcb));
					*(proximoPrograma->pcb)=deserializarPCB(res);
				}

				//TODO El planificador debe desencolar procesos ya terminados
				proximoPrograma = planificador(proximoPrograma);
			} else if(res[0] == 'B'){
				guardarVarGlobal(cpu->id);
			}  else if(res[0] == 'C'){
				leerVarGlobal(cpu->id);
			} else if(res[0] == 'W'){
				semWait(cpu->id);
			} else if(res[0] == 'S'){
				semSignal(cpu->id);
			} /*else if(res[0] == 'H'){
				leerHeap(cpu->id);
			} else if(res[0] == 'G'){
				guardarEnHeap(cpu->id);
			}*/

			// Esta Y debe ser reemplazada por el codigo que devuelva la cpu, cuando finalice tiene que limpiar las estructuras incluyendo cpu
		} else {
			proximoPrograma = planificador(NULL);
		}

		usleep(500);
	}

	return 0;
}

void* programa(t_programa *programa){
	while(1){
		usleep(5000);
	}
}

void moverPrograma(t_programa* unPrograma,t_queue* colaOrigen, t_queue* colaDestino){
	if(!list_is_empty(colaOrigen->elements)){
		int aux = 0;
		t_programa *programaAux = list_get(colaOrigen->elements,aux);
		while (programaAux->pcb->pid != unPrograma->pcb->pid && aux < list_size(colaOrigen->elements)){
			programaAux = list_get(colaOrigen->elements,aux);
			aux++;
		}

		if(programaAux->pcb->pid == unPrograma->pcb->pid){
			queue_push(colaDestino,unPrograma);
			list_remove(colaOrigen->elements, aux);
		}
	}
}

t_programa* planificador(t_programa* unPrograma){
	// mutex por haber leido de un archivo que puede ser actualizado hasta antes del recv
	algoritmoPlanificador = obtenerConfiguracionString(cfg,"ALGORITMO");
	//printf("algoritmoPlanificador %s\n",algoritmoPlanificador);

	if(unPrograma == NULL){
		if(queue_size(procesosREADY) > 0){
			t_programa* aux = queue_pop(procesosREADY);
			queue_push(procesosEXEC,aux);
			unPrograma = aux;
		} else if(queue_size(procesosNEW) > 0){
			t_programa* aux = queue_pop(procesosNEW);
			testi(queue_size(procesosNEW));
			encolarReady(aux);
			queue_push(procesosEXEC,aux);
			unPrograma = aux;
		} else {
			unPrograma = NULL;
		}
	}

	if(strcmp(algoritmoPlanificador,"RR") == 0){
		if(unPrograma->quantumRestante == 0){
			unPrograma->quantumRestante--;
			return unPrograma;
		} else {
			return NULL;
		}
	} else if(strcmp(algoritmoPlanificador,"FIFO") == 0){
		return unPrograma;
	} else {
		log_error(logger,"Algoritmo mal cargado al config.cfg");
	}
	free(algoritmoPlanificador);
	return 0;
}
