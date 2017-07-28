/* planificador.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "planificador.h"
#include "capaMemoria.h"
#include "main.h"
#include "error.h"
#include "heapNico.h"
#include <sys/socket.h>
#include <signal.h>
#include <sys/socket.h>

void imprimirDescripcionError(int error){
	switch(error){
		case 0:
			textoAzul("El programa finalizo correctamente");
			break;
		case -1:
			textoAzul("No se pudieron reservar recursos para ejecutar el programa.");
			break;
		case -2:
			textoAzul("El programa intento acceder a un archivo que no existe.");
			break;
		case -3:
			textoAzul("El programa intento leer un archivo sin permisos.");
			break;
		case -4:
			textoAzul("El programa intento escribir un archivo sin permisos.");
			break;
		case -5:
			textoAzul("Excepcion de memoria.");
			break;
		case -6:
			textoAzul("Finalizado a traves de desconexión de consola.");
			break;
		case -7:
			textoAzul("Finalizado a traves del comando Finalizar Programa de la consola.");
			break;
		case -8:
			textoAzul("Se intentó reservar mas memoria que el tamaño de una pagina");
			break;
		case -9:
			textoAzul("No se pueden asignar mas páginas al proceso");
			break;
		case -10:
			textoAzul("La cpu que estaba ejecutando el programa se desconecto");
			break;
		case -11:
			textoAzul("Error lectura memoria");
			break;
		case -12:
			textoAzul("Error escribir memoria");
			break;
		case -13:
			textoAzul("Etiqueta Inexistente");
			break;
		case -14:
			textoAzul("Variable compartida inexistente");
			break;
		case -15:
			textoAzul("Semaforo inexistente");
			break;
		case -16:
			textoAzul("Error al reservar en heap");
			break;
		case -17:
			textoAzul("Error al borrar archivo");
			break;
		case -18:
			textoAzul("Error al cerrrar archivo");
			break;
		case -19:
			textoAzul("Programa eliminado por el usuario.");
			break;
		case -20:
			textoAzul("Error sin definicion");
			break;
		case -21:
			textoAzul("La cpu que estaba ejecutando el programa se desconecto");
			break;
		case -30:
			textoAzul("No existe variable");
			break;
		default:
			break;
	}
}

char *algoritmoPlanificador;

t_cpu * encontrarCPU(uint32_t i){
	pthread_mutex_lock(&mutex_cpu);
	int contador = 0;
	if(list_size(CPUs) > 0){
		t_cpu * cpuAux;
		while(contador < list_size(CPUs)){
			cpuAux = list_get(CPUs,contador);
			if(cpuAux->id == i){
				pthread_mutex_unlock(&mutex_cpu);
				return cpuAux;
			}
			contador++;
		}
	}
	pthread_mutex_unlock(&mutex_cpu);
	return NULL;
}

t_programa * encontrarPrograma(uint32_t i){
	pthread_mutex_lock(&mutex_programas);
	int contador = 0;
	if(list_size(PROGRAMAs) > 0){
		t_programa * programaAux;
		while(contador < list_size(PROGRAMAs)){
			programaAux = list_get(PROGRAMAs,contador);
			if(programaAux->id == i){
				pthread_mutex_unlock(&mutex_programas);
				return programaAux;
			}
			contador++;
		}
	}
	pthread_mutex_unlock(&mutex_programas);
	return NULL;
}

t_programa * encontrarProgramaPorPID(uint32_t pid){
	pthread_mutex_lock(&mutex_programas);
	int contador = 0;
	if(list_size(PROGRAMAs) > 0){
		t_programa * programaAux;
		while(contador < list_size(PROGRAMAs)){
			programaAux = list_get(PROGRAMAs,contador);
			if(programaAux->pcb->pid == pid){
				pthread_mutex_unlock(&mutex_programas);
				return programaAux;
			}
			contador++;
		}
	}
	pthread_mutex_unlock(&mutex_programas);
	return NULL;
}

t_cpu * encontrarCPUporPID(uint32_t pid){
	pthread_mutex_lock(&mutex_cpu);
	int contador = 0;
	if(list_size(CPUs) > 0){
		t_cpu * cpuAux;
		while(contador < list_size(CPUs)){
			cpuAux = list_get(CPUs,contador);
			if(cpuAux->programaEnEjecucion->pid == pid){
				pthread_mutex_unlock(&mutex_cpu);
				return cpuAux;
			}
			contador++;
		}
	}
	pthread_mutex_unlock(&mutex_cpu);
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
	if(error == -1){
		nuevoProceso->pcb->exitCode = -1;
		queue_push(procesosEXIT,nuevoProceso);
		log_error(logger,"No se pudieron reservar recursos para ejecutar el programa");
	}
}

t_cpu* indiceProximaCPULibre(){
	pthread_mutex_lock(&mutex_cpu);
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
	pthread_mutex_unlock(&mutex_cpu);
	return NULL;
}

t_programa * inicializarPrograma(uint32_t i,uint32_t pidActual){
	uint32_t tamanioCodigo;
	if(recv(i,&tamanioCodigo,4,MSG_WAITALL) < 0)
		log_error(logger,"Error al recibir el tamanio de codigo de nuevo programa");

	//memcpy(&tamanioCodigo,tamanioCodigoString,4);
	//log_info("Tamanio codigo de %i: %i\n",i,tamanioCodigo);
	char* codigo;/* = malloc(tamanioCodigo);*/

	//todo verificar logica
	//reserva cant paginas * tamanio pagina, llena el contenido de ceros

	if(tamanioCodigo%tamanioPagina!=0)
		codigo=calloc(1,((tamanioCodigo/tamanioPagina)+1)*tamanioPagina);
	else
		codigo=calloc(1,(tamanioCodigo/tamanioPagina)*tamanioPagina);
	//codigo=calloc(1,tamanioCodigo);

	if(recv(i,codigo,tamanioCodigo,MSG_WAITALL) < 0)
		log_error(logger,"Error al recibir el codigo de nuevo programa");

	//Envio PID a consola
	enviarPID(i);

	// Inicializo el Programa y su PCB
	t_pcb * nuevoPCB;/* = malloc(sizeof(t_pcb))*/
	t_programa * nuevoProceso = malloc(sizeof(t_programa));

	// Inicializo Metadata
	t_metadata_program* metadata=metadata_desde_literal(codigo);

	pthread_mutex_lock(&mutex_colasPlanificacion);
	// Creo PCB y libero metadata
	nuevoPCB=crearPCB(metadata,pidActual);
	pthread_mutex_unlock(&mutex_colasPlanificacion);
	metadata_destruir(metadata);

	nuevoProceso->id = i;
	nuevoProceso->debeFinalizar = 0;
	nuevoProceso->tablaArchivosPrograma = list_create();
	nuevoProceso->FDCounter = 2;
	nuevoProceso->paginasHeap = list_create();
	pthread_mutex_lock(&mutex_colasPlanificacion);
	nuevoProceso->quantumRestante = quantum;
	pthread_mutex_unlock(&mutex_colasPlanificacion);
	nuevoProceso->pcb = nuevoPCB;
	nuevoProceso->rafagasEjecutadas = 0;
	nuevoProceso->cantidadSyscallsEjecutadas = 0;
	nuevoProceso->cantidadAlocarEjecutados = 0;
	nuevoProceso->cantidadAlocarEjecutadosBytes = 0;
	nuevoProceso->cantidadLiberarEjecutados = 0;
	nuevoProceso->cantidadLiberarEjecutadosBytes = 0;

	/*
	Probando el imprimir:
	uint32_t p = 3;
	send(i,"P",1,0);
	send(i,&p,4,0);
	send(i,"ASD",3,0);*/

	uint32_t cantidadPaginasCodigo = 0;
	if(tamanioPagina == -1)
		log_error(logger,"No se pueden cargar procesos porque la memoria no esta conectada");
	else if(tamanioPagina == 0)
		log_error(logger,"El tamanio de una pagina no puede ser 0");
	else{
		//todo verificar logica
		if(tamanioCodigo%tamanioPagina!=0)
			cantidadPaginasCodigo = (tamanioCodigo/tamanioPagina)+1;
		else
			cantidadPaginasCodigo = tamanioCodigo/tamanioPagina;
	}

	log_info(logger,"PID: %i, Cantidad de paginas de codigo: %i\n",nuevoPCB->pid,cantidadPaginasCodigo);

	if(cantidadPaginasCodigo == 0)
		killme();

	nuevoProceso->codigo = codigo;
	nuevoProceso->paginasCodigo = cantidadPaginasCodigo;
	nuevoProceso->pcb->cantPagCod= cantidadPaginasCodigo;
	nuevoProceso->pcb->ultimaPosUsada.pag=cantidadPaginasCodigo;

	pthread_mutex_lock(&mutex_colasPlanificacion);
	if(gradoMultiprogramacion > cantidadProgramasEnSistema-queue_size(procesosEXIT)-queue_size(procesosNEW)){
		encolarReady(nuevoProceso);
	}else{
		/*if(send(i,"N",1,0) < 1)
			log_error(logger,"ERROR, el kernel no le pudo enviar el mensaje de que no es posible crear un nuevo programa");*/
		queue_push(procesosNEW,nuevoProceso);
		log_info(logger,"No se pueden aceptar mas programas debido al grado de multiprogramacion definido. Encolando en NEW...");
	}
	cantidadProgramasEnSistema++;
	list_add(PROGRAMAs,nuevoProceso);
	pthread_mutex_unlock(&mutex_colasPlanificacion);
	return nuevoProceso;
}

void liberarCPU(t_cpu* cpu, t_programa* programaDeCPU,bool force){
		log_error(logger,"Se esta por eliminar una CPU");
		pthread_mutex_lock(&mutex_colasPlanificacion);
		int resFinalizarPrograma = finalizarProcesoMemoria(programaDeCPU->pcb->pid,force);

		if(resFinalizarPrograma == 0){
			log_trace(logger,"Un programa ha sido movido a EXIT");
			cantidadMemoryLeak(programaDeCPU);
		} else {
			programaDeCPU->pcb->exitCode = -5;
		}
		pthread_mutex_unlock(&mutex_colasPlanificacion);
		pthread_mutex_lock(&mutex_fs);
		cerrarFDsiProcesoMuere(cpu->id,programaDeCPU);
		pthread_mutex_unlock(&mutex_fs);
		eliminarSiHayCPU(cpu->id);
		programaDeCPU->pcb->exitCode= -21;
		send(programaDeCPU->id,"F",1,0);
		pthread_exit(&cpu->hilo);
	}

void* cpu(t_cpu * cpu){
	//log_info("Cpu: #%i\n",cpu->id);
	t_programa * proximoPrograma;
	t_programa * preConfirmacion;
	pthread_mutex_lock(&mutex_colasPlanificacion);
	proximoPrograma = planificador(NULL,cpu,0);
	if(proximoPrograma == NULL){
		proximoPrograma = planificador(NULL,cpu,1);
	}
	pthread_mutex_unlock(&mutex_colasPlanificacion);
	char*res = malloc(1);

	void liberarCPUSinPrograma(){
		log_error(logger,"Se esta por eliminar una CPU,");
		eliminarSiHayCPU(cpu->id);
		pthread_mutex_unlock(&mutex_colasPlanificacion);
		pthread_exit(&cpu->hilo);
	}

	void nico(){
		printf("Nico\n");
		if(proximoPrograma!=NULL){
			proximoPrograma->debeFinalizar=1;
		}
	}

	while(1){
		signal(SIGPIPE, nico);
		if(proximoPrograma != 0 && proximoPrograma!=NULL){
			char* aString1 = string_itoa(proximoPrograma->pcb->pid);
			char* aString2 = string_itoa(cpu->id);
			char* aString = concat(4,"Ejecutando el proceso ",aString1," en la CPU ",aString2);
			log_warning(logger,aString);
			free(aString1);
			free(aString2);
			free(aString);
			pthread_mutex_lock(&mutex_colasPlanificacion);
			t_pcb proximoPCB = *(proximoPrograma->pcb);
			package_t paquete = serializarPCB(proximoPCB);
			uint32_t tamUint=sizeof(uint32_t);
			uint32_t tamARecibir=0;
			pthread_mutex_unlock(&mutex_colasPlanificacion);

			res=realloc(res,1);

			if(sendall(cpu->id, (char*)&paquete.data_size, &tamUint) < 0)
				liberarCPU(cpu,proximoPrograma,true);

			if(sendall(cpu->id, paquete.data, &paquete.data_size) < 0)
				liberarCPU(cpu,proximoPrograma,true);

			free(paquete.data);

			recv(cpu->id,res,1,MSG_WAITALL);
			if(res[0]!= 'Y'){
				log_error(logger,"La CPU no recibio el PCB");
				liberarCPU(cpu,proximoPrograma,true);
			}

			while(1){
				recv(cpu->id,res,1,MSG_WAITALL);
				pthread_mutex_lock(&mutex_colasPlanificacion);
				if(proximoPrograma->debeFinalizar == 1 || cpu->debeFinalizar)
					res[0] = 'F';
				pthread_mutex_unlock(&mutex_colasPlanificacion);

				// Verifico si aun le falta ejecutar al proceso
				if(res[0] == 'F'){
					if(cpu->debeFinalizar){
						liberarCPU(cpu,proximoPrograma,true);
					} else {
						proximoPrograma->rafagasEjecutadas++;
						if(recv(cpu->id,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
							liberarCPU(cpu,proximoPrograma,true);
						res=realloc(res,tamARecibir);
						if(recv(cpu->id,res,tamARecibir,MSG_WAITALL) <= 0)
							liberarCPU(cpu,proximoPrograma,true);
						else{
							liberarPCB(*(proximoPrograma->pcb));
							pthread_mutex_lock(&mutex_colasPlanificacion);
							*(proximoPrograma->pcb)=deserializarPCB(res);
							pthread_mutex_unlock(&mutex_colasPlanificacion);
							free(res);
							res=NULL;
						}
						pthread_mutex_lock(&mutex_colasPlanificacion);
						if(proximoPrograma->pcb->exitCode == 0){
							log_info(logger,"Programa finalizo con exito");
						} else if(proximoPrograma->pcb->exitCode < 0){
							imprimirDescripcionError(proximoPrograma->pcb->exitCode);
						}

						send(proximoPrograma->id,"F",1,0);


						if(finalizarProcesoMemoria(proximoPrograma->pcb->pid,true) == 0){
							char * string = concat(3,"Moviendo el proceso ",string_itoa(proximoPrograma->pcb->pid)," a EXIT");
							cantidadMemoryLeak(proximoPrograma);
							log_trace(logger,string);
							free(string);
						}else
							log_trace(logger,"Fallo el liberar memoria");

						pthread_mutex_unlock(&mutex_colasPlanificacion);
					}
					proximoPrograma = NULL;
					break;
				} else if(res[0] == 'Y'){
					proximoPrograma->rafagasEjecutadas++;
					if(recv(cpu->id,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
						liberarCPU(cpu,proximoPrograma,true);

					res=realloc(res,tamARecibir);
					log_info(logger,"Tam PCB a recibir: %i\n",tamARecibir);
					log_trace(logger,"PCB RECIBIDO DEL CPU");
					if(recv(cpu->id,res,tamARecibir,MSG_WAITALL) <= 0)
						liberarCPU(cpu,proximoPrograma,true);
					else{
						pthread_mutex_lock(&mutex_colasPlanificacion);
						liberarPCB(*(proximoPrograma->pcb));
						*(proximoPrograma->pcb)=deserializarPCB(res);
						free(res);
						res=NULL;
						pthread_mutex_unlock(&mutex_colasPlanificacion);
					}

					pthread_mutex_lock(&mutex_colasPlanificacion);
					/*char * string = concat(3,"Moviendo el proceso ",string_itoa(proximoPrograma->pcb->pid)," de EXEC a READY");
					log_trace(logger,string);
					free(string);
					moverPrograma(proximoPrograma,procesosEXEC,procesosREADY);*/
					preConfirmacion = planificador(proximoPrograma,cpu,0);
					if(preConfirmacion == NULL){
						proximoPrograma = planificador(proximoPrograma,cpu,1);
					} else {
						proximoPrograma = preConfirmacion;
					}
					pthread_mutex_unlock(&mutex_colasPlanificacion);
					break;

				}else if(res[0] == 'S'){
					pthread_mutex_lock(&mutex_colasPlanificacion);
					pthread_mutex_lock(&mutex_semaforos);
					log_trace(logger,"Llamada a SIGNAL");
					semSignal(cpu->id);
					proximoPrograma->cantidadSyscallsEjecutadas++;
					pthread_mutex_unlock(&mutex_semaforos);
					pthread_mutex_unlock(&mutex_colasPlanificacion);
				} else if(res[0] == 'A'){
					pthread_mutex_lock(&mutex_varGlobales);
					log_trace(logger,"Llamada a Guardar Var Global");
					guardarVarGlobal(cpu->id);
					proximoPrograma->cantidadSyscallsEjecutadas++;
					pthread_mutex_unlock(&mutex_varGlobales);
				}  else if(res[0] == 'O'){
					pthread_mutex_lock(&mutex_varGlobales);
					log_trace(logger,"Llamada a Leer Var Global");
					leerVarGlobal(cpu->id);
					proximoPrograma->cantidadSyscallsEjecutadas++;
					pthread_mutex_unlock(&mutex_varGlobales);
				} else if(res[0] == 'W'){
					pthread_mutex_lock(&mutex_colasPlanificacion);
					pthread_mutex_lock(&mutex_semaforos);
					log_trace(logger,"Llamada a WAIT");
					proximoPrograma->cantidadSyscallsEjecutadas++;
					if(semWait(cpu->id,proximoPrograma->pcb->pid,proximoPrograma)){
						proximoPrograma = NULL;
						pthread_mutex_unlock(&mutex_semaforos);
						pthread_mutex_unlock(&mutex_colasPlanificacion);
						break;
					}
					pthread_mutex_unlock(&mutex_semaforos);
					pthread_mutex_unlock(&mutex_colasPlanificacion);
				} else if(res[0] == 'H'){
					log_trace(logger,"Llamada a Leer Heap");
					leerHeap(cpu->id);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'a'){
					pthread_mutex_lock(&mutex_fs);
					log_trace(logger,"Llamada a AbrirFD");
					abrirFD(cpu->id,proximoPrograma);
					pthread_mutex_unlock(&mutex_fs);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'b'){
					pthread_mutex_lock(&mutex_fs);
					log_trace(logger,"Llamada a BorrarFD");
					borrarFD(cpu->id,proximoPrograma);
					pthread_mutex_unlock(&mutex_fs);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'c'){
					pthread_mutex_lock(&mutex_fs);
					log_trace(logger,"Llamada a CerrarFD");
					cerrarFD(cpu->id,proximoPrograma);
					pthread_mutex_unlock(&mutex_fs);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'm'){
					pthread_mutex_lock(&mutex_fs);
					log_trace(logger,"Llamada a MoverPunteroFD");
					moverPunteroFD(cpu->id,proximoPrograma);
					pthread_mutex_unlock(&mutex_fs);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'l'){
					pthread_mutex_lock(&mutex_fs);
					log_trace(logger,"Llamada a LeerFD");
					leerFD(cpu->id,proximoPrograma);
					pthread_mutex_unlock(&mutex_fs);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'e'){
					pthread_mutex_lock(&mutex_fs);
					log_trace(logger,"Llamada a EscribirFD");
					escribirFD(cpu->id,proximoPrograma);
					pthread_mutex_unlock(&mutex_fs);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'G'){
					//guardarEnHeap(cpu->id,proximoPrograma->paginasHeap,&proximoPrograma->id);
					log_trace(logger,"Llamada a GuardarHeap");
					guardarHeapNico(cpu->id,proximoPrograma);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				} else if(res[0] == 'L'){
					log_trace(logger,"Llamada a LiberarHeap");
					liberarHeapNico(cpu->id,proximoPrograma);
					proximoPrograma->cantidadSyscallsEjecutadas++;
				}
			}
			// Esta Y debe ser reemplazada por el codigo que devuelva la cpu, cuando finalice tiene que limpiar las estructuras incluyendo cpu
		} else {
			pthread_mutex_lock(&mutex_colasPlanificacion);
			proximoPrograma = planificador(NULL,cpu,0);
			if(proximoPrograma == NULL){
				proximoPrograma = planificador(NULL,cpu,1);
			}
			pthread_mutex_unlock(&mutex_colasPlanificacion);

		}
		usleep(500);
	}

	return 0;
}

void* programa(t_programa *programa){

	char * charsito = malloc(1);
	int rev = recv(programa->id,charsito,1,0);
	pthread_mutex_lock(&mutex_colasPlanificacion);
	bool _condicion3(t_programa* self){
		return self->pcb->pid==programa->pcb->pid;
	}
	t_programa * aux = list_find(procesosEXIT->elements,(void*)_condicion3);
	if(charsito[0] == 'F' && aux == NULL){
		programa->pcb->exitCode = -7;
		log_trace(logger,"La consola finalizo un programa");
	}
	if(charsito[0] == '0' && aux == NULL){
		programa->pcb->exitCode = -6;
		log_trace(logger,"La consola finalizo un programa por desconexion");
	}
	if(rev <= 0 && aux == NULL){
		programa->pcb->exitCode = -6;
		log_trace(logger,"La consola finalizo un programa por desconexion");
	}

	int resFinalizarPrograma = finalizarProcesoMemoria(programa->pcb->pid,false);

	if(resFinalizarPrograma == 0){
		log_trace(logger,"Un programa ha sido movido a EXIT");
		cantidadMemoryLeak(programa);
	} else {
		//log_trace(logger,"Excepcion de memoria. No se pudo liberar recursos del programa");
		//programa->pcb->exitCode = -5;
	}
	pthread_mutex_unlock(&mutex_colasPlanificacion);
	return NULL;
}

void moverPrograma(t_programa* unPrograma,t_queue* colaOrigen, t_queue* colaDestino){
	if(!list_is_empty(colaOrigen->elements)){
		int aux = 0;
		t_programa *programaAux = list_get(colaOrigen->elements,aux);
		while (programaAux->pcb->pid != unPrograma->pcb->pid && aux < list_size(colaOrigen->elements)){
			programaAux = list_get(colaOrigen->elements,aux);
			if(programaAux->pcb->pid == unPrograma->pcb->pid){
				break;
			}
			aux++;
		}

		if(programaAux->pcb->pid == unPrograma->pcb->pid){
			queue_push(colaDestino,unPrograma);
			list_remove(colaOrigen->elements, aux);
		}
	}
}

t_programa* planificador(t_programa* unPrograma,t_cpu* cpu,uint32_t confirmado){
	while(detenerPlanificacion == 1){
		usleep(1000);
	}
	usleep(retardo);
	uint32_t confirmacionEnviada = 0;
	uint32_t uno = 1;
	// mutex por haber leido de un archivo que puede ser actualizado hasta antes del recv
	char* rutaConfigActualizada = rutaAbsolutaDe("config.cfg");
	t_config* cfgActualizada = config_create(rutaConfigActualizada);
	quantum = obtenerConfiguracion(cfgActualizada,"QUANTUM");
	algoritmoPlanificador = obtenerConfiguracionString(cfgActualizada,"ALGORITMO");
	retardo = obtenerConfiguracion(cfgActualizada,"QUANTUM_SLEEP");
	while(quantum == -1 || algoritmoPlanificador == -1){
		free(cfgActualizada);
		free(rutaConfigActualizada);
		quantum = obtenerConfiguracion(cfgActualizada,"QUANTUM");
		algoritmoPlanificador = obtenerConfiguracionString(cfgActualizada,"ALGORITMO");
		retardo = obtenerConfiguracion(cfgActualizada,"QUANTUM_SLEEP");
		rutaConfigActualizada = rutaAbsolutaDe("config.cfg");
		cfgActualizada = config_create(rutaConfigActualizada);
	}

	// Si la multiprogramacion me lo permite, saco los procesos de NEW y los mando a READY
	if(queue_size(procesosNEW) > 0 && gradoMultiprogramacion > cantidadProgramasEnSistema-queue_size(procesosEXIT)-queue_size(procesosNEW)){
		log_trace(logger,"Moviendo el proceso de New a READY");
		t_programa* aux = queue_pop(procesosNEW);
		encolarReady(aux);
		unPrograma = NULL;
	}

	if(queue_size(procesosREADY)>0 || unPrograma !=NULL){
		int quedaQuantum = 1;

		if(strcmp(algoritmoPlanificador,"RR") == 0){
			if(unPrograma != NULL && unPrograma->quantumRestante == 0)
				quedaQuantum= 0;
		}

		if(confirmado == 0 && quedaQuantum == 1){
			if(encontrarCPU(cpu->id) == NULL){
				return NULL;
			}
			if(sendall(cpu->id,"0",&uno) < 0){
				return NULL;
			}
			if(sendall(cpu->id,"0",&uno) < 0){
				return NULL;
			}
		}
	}

	if(unPrograma == NULL){
		if(queue_size(procesosREADY) > 0){
			t_programa* aux = NULL;
			usleep(100);
			if(encontrarCPU(cpu->id) == NULL){
				/*log_error(logger,"Se esta por eliminar una CPU...");
				eliminarSiHayCPU(cpu->id);
				pthread_exit(&cpu->hilo);
				pthread_mutex_unlock(&mutex_colasPlanificacion);*/
				return NULL;
			}else{
				if(confirmado == 0){
					return NULL;
				} else {
					confirmacionEnviada = 1;
					aux = queue_pop(procesosREADY);
					if(strcmp(algoritmoPlanificador,"RR") == 0){
						if(aux->quantumRestante == 0){
							aux->quantumRestante = quantum;
							queue_push(procesosREADY,aux);
							return NULL;
						}else{
							queue_push(procesosEXEC,aux);
							unPrograma = aux;
							char * string = concat(3,"Moviendo el proceso ",string_itoa(unPrograma->pcb->pid)," de READY a EXEC");
							log_trace(logger,string);
							free(string);
						}
					} else {
						queue_push(procesosEXEC,aux);
						unPrograma = aux;
						char * string = concat(3,"Moviendo el proceso ",string_itoa(unPrograma->pcb->pid)," de READY a EXEC");
						log_trace(logger,string);
						free(string);
					}

				}
			}
		} else {
			unPrograma = NULL;
		}
	}

	if(strcmp(algoritmoPlanificador,"RR") == 0){

		config_destroy(cfgActualizada);
		free(rutaConfigActualizada);
		if(unPrograma != NULL && confirmado == 1){
			if(unPrograma->quantumRestante == 0){
				unPrograma->quantumRestante = quantum;
				moverPrograma(unPrograma,procesosEXEC,procesosREADY);
				return NULL;
			} else {
				unPrograma->quantumRestante--;
				return unPrograma;
			}

		} else {
			return NULL;
		}

	} else if(strcmp(algoritmoPlanificador,"FIFO") == 0){

		config_destroy(cfgActualizada);
		free(rutaConfigActualizada);
		if(unPrograma != NULL && confirmado == 1){
			return unPrograma;
		} else {
			return NULL;
		}

	} else {
		log_error(logger,"Algoritmo mal cargado al config.cfg");
	}
	config_destroy(cfgActualizada);
	free(rutaConfigActualizada);
	return NULL;

}
