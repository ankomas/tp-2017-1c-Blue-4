/*
 * consolaKernel.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "planificador.h"
#include <commons/string.h>
#include "main.h"

#define LISTADO_PROCESOS '1'
	#define PROCESOS_TODOS '0'
	#define PROCESOS_NEW '1'
	#define PROCESOS_READY '2'
	#define PROCESOS_EXEC '3'
	#define PROCESOS_BLOCK '4'
	#define PROCESOS_EXIT '5'
#define INFO_PROCESO '2'
	#define CANTIDAD_RAFAGAS_EJECUTADAS '1'
	#define CANTIDAD_OP_PRIVILEGIADAS '2'
	#define OBTENER_TABLA_ARCHIVOS_ABIERTOS '3'
	#define CANTIDAD_PAGINAS_HEAP '4'
		#define CANTIDAD_ACCIONES_ALOCAR '1'
		#define CANTIDAD_ACCIONES_LIBERAR '2'
	#define CANTIDAD_SYSCALLS_EJECUTADAS '5'
#define TABLA_GLOBAL_ARCHIVOS '3'
#define MODIFICAR_GRADO_MULTIPROGRAMACION '4'
#define FINALIZAR_PROCESO '5'
#define DETENER_PLANIFICACION '6'


void imprimirPIDs(t_queue* colaConsultada){
	int i = 0;
	while(queue_size(colaConsultada) > 0 && i < queue_size(colaConsultada)){
		t_programa *aux = list_get(colaConsultada->elements,i);
		anuncio(string_itoa(aux->pcb->pid));
		i++;
	}
}

void listarProcesos(int opcion){
	t_queue *colaConsultada = NULL;
	char* mensaje = string_new();
	if(opcion == 1){
		colaConsultada = procesosNEW;
		string_append(&mensaje,"nuevos");
	} else if(opcion == 2){
		colaConsultada = procesosREADY;
		string_append(&mensaje,"listos");
	} else if(opcion == 3){
		colaConsultada = procesosEXEC;
		string_append(&mensaje,"en ejecucion");
	} else if(opcion == 4){
		colaConsultada = procesosBLOCK;
		string_append(&mensaje,"bloqueados");
	} else if(opcion == 5){
		colaConsultada = procesosEXIT;
		string_append(&mensaje,"exit");
	}


	if(colaConsultada != NULL){
		printf("Cantidad procesos actuales %s: %d\n",mensaje,queue_size(colaConsultada));
		imprimirPIDs(colaConsultada);
	} else {
		int cantidadTotalProcesos =
				queue_size(procesosNEW)+
				queue_size(procesosREADY)+
				queue_size(procesosEXEC)+
				queue_size(procesosBLOCK)+
				queue_size(procesosEXIT);
		printf("Cantidad procesos totales: %d\n",cantidadTotalProcesos);
		imprimirPIDs(procesosNEW);
		imprimirPIDs(procesosREADY);
		imprimirPIDs(procesosEXEC);
		imprimirPIDs(procesosBLOCK);
		imprimirPIDs(procesosEXIT);
	}

	free(mensaje);

}

void mostrarYAgregarRuta(char* rutaOpciones,char*ruta){
	strcpy(rutaOpciones,ruta);
	//string_append(&rutaOpciones,mensaje);
	printf("%s",rutaOpciones);
}


void imprimirTGA(){
	int contador = 0;
	textoAzul("\n------------- Tabla Global de Archivos ------------- ");
	while(list_size(tablaGlobalArchivos) > contador){
		t_entradaTGA * entradaAux = list_get(tablaGlobalArchivos,contador);
		char* linea = concat(3,string_itoa(entradaAux->abierto)," | ",entradaAux->archivo);
		textoAzul(linea);
		free(linea);
		contador++;
	}
	textoAzul("---------------------------------------------------- ");
}

void imprimirTAP(t_programa *nuevoProceso){
	int contador = 0;
	textoAzul("\n------------- Tabla de archivos de proceso ------------- ");
	while(list_size(nuevoProceso->tablaArchivosPrograma) > contador){
		t_entradaTAP * entradaAux = list_get(nuevoProceso->tablaArchivosPrograma,contador);
		char* linea = concat(3,string_itoa(entradaAux->globalFD)," | ",entradaAux->flags);
		textoAzul(linea);
		free(linea);
		contador++;
	}
	textoAzul("-------------------------------------------------------- ");
}

void *consolaKernel(){
	int opcionInt = 0;
	while(1){
		char rutaOpciones[512] = "";
		anuncio("\n-- Consola del kernel --");
		anuncio("Por favor seleccione la accion que desea realizar:");
		printf("1 - Listar procesos\n");
		printf("2 - Obtener informacion de un proceso\n");
		printf("3 - Obtener la tabla global de archivos\n");
		printf("4 - Modificar el grado de multiprogramacion del sistema\n");
		printf("5 - Finalizar un proceso\n");
		printf("6 - Detener la planificacion\n");
		char opcion[1];
		scanf("%s", opcion);

		if(opcion[0] == LISTADO_PROCESOS){
			mostrarYAgregarRuta(rutaOpciones," >> Listar procesos");
			anuncio("\nPor favor seleccione el tipo de proceso que desea observar:");
			printf("0 - Todos los procesos\n");
			printf("1 - Procesos nuevos\n");
			printf("2 - Procesos listos\n");
			printf("3 - Procesos en ejecucion \n");
			printf("4 - Procesos bloqueados\n");
			printf("5 - Procesos terminados\n");
			scanf("%s", opcion);
			opcionInt = opcion[0] - '0';
			if(opcionInt >= 0 && opcionInt <= 5 )
				listarProcesos(opcionInt);
		} else if(opcion[0] == INFO_PROCESO){
			opcionInt = 0;
			mostrarYAgregarRuta(rutaOpciones," >> Mostrar info de proceso");
			anuncio("\nPor favor ingrese el PID del proceso para mostrar info sobre el mismo:");
			scanf("%d", &opcionInt);
			t_programa* programaEncontrado = encontrarProgramaPorPID(opcionInt);
			if(programaEncontrado != NULL){
				anuncio("\nPor favor ingrese lo que desea conocer de este proceso:");
				printf("1 - Cantidad de rafagas ejecutadas\n");
				printf("2 - Cantidad Operaciones privilegiadas ejecutadas\n");
				printf("3 - Tabla de archivos abiertos \n");
				printf("4 - Cantidad de paginas de Heap\n");
				printf("5 - Cantidad de syscalls realizadas\n");
				scanf("%s", opcion);
				if(opcion[0] == CANTIDAD_RAFAGAS_EJECUTADAS){
					mostrarYAgregarRuta(rutaOpciones," >> Cantidad rafagas ejecutadas");
					printf("\nCantidad de rafagas ejecutadas: %i",programaEncontrado->rafagasEjecutadas);
				} else if(opcion[0] == CANTIDAD_OP_PRIVILEGIADAS){
					mostrarYAgregarRuta(rutaOpciones," >> Cantidad Operaciones privilegiadas");
					printf("Fueron realizadas %i",programaEncontrado->cantidadSyscallsEjecutadas);
				} else if(opcion[0] == OBTENER_TABLA_ARCHIVOS_ABIERTOS){
					mostrarYAgregarRuta(rutaOpciones," >> Mostrar tabla archivos abiertos");
					imprimirTAP(programaEncontrado);
				} else if(opcion[0] == CANTIDAD_PAGINAS_HEAP){
					mostrarYAgregarRuta(rutaOpciones," >> Cantidad paginas heap");
					printf("Cantidad Paginas de Heap utilizadas: %i",list_size(programaEncontrado->paginasHeap));
					anuncio("\nDesea conocer la cantidad de acciones alocar o liberar?:");
					printf("1 - Cantidad de acciones alocar realizados\n");
					printf("2 - Cantidad de acciones liberar realizados\n");
					scanf("%s", opcion);
					if(opcion[0] == CANTIDAD_ACCIONES_ALOCAR){
						mostrarYAgregarRuta(rutaOpciones," >> Cantidad acciones alocar");
						printf("\nCantidad ALOCAR realizados: %i",programaEncontrado->cantidadAlocarEjecutados);
						printf("\nCantidad ALOCAR realizados en bytes: %i",programaEncontrado->cantidadAlocarEjecutadosBytes);
					} else if(opcion[0] == CANTIDAD_ACCIONES_LIBERAR){
						mostrarYAgregarRuta(rutaOpciones," >> Cantidad acciones liberar");
						printf("\nCantidad LIBERAR realizados: %i",programaEncontrado->cantidadLiberarEjecutados);
						printf("\nCantidad LIBERAR en bytes: %i",programaEncontrado->cantidadLiberarEjecutadosBytes);
					}
				} else if(opcion[0] == CANTIDAD_SYSCALLS_EJECUTADAS){
					// ? no son lo mismo que op privilegiadas?
					//printf("Cantidad de Syscalls ejecutadas: %i",proceso->cantidadSyscallsEjecutadas;
				}
			}
		} else if(opcion[0] == TABLA_GLOBAL_ARCHIVOS){
			mostrarYAgregarRuta(rutaOpciones," >> Mostrar Tabla global de archivos");
			imprimirTGA();
		} else if(opcion[0] == MODIFICAR_GRADO_MULTIPROGRAMACION){
			mostrarYAgregarRuta(rutaOpciones," >> Modificar el grado de multiprogramacion");
			opcionInt = 0;
			printf("\nIngrese el nuevo grado de multiprogramacion:\n");
			scanf("%d", &opcionInt);
			gradoMultiprogramacion = opcionInt;
			char * texto = concat(2,"Nuevo grado de multiprogramacion: ",string_itoa(opcionInt));
			log_info(logger,texto);
			free(texto);
		} else if(opcion[0] == FINALIZAR_PROCESO){
			mostrarYAgregarRuta(rutaOpciones," >> Finalizar proceso");
			opcionInt = 0;
			printf("\nIngrese el PID del programa que desea finalizar:\n");
			scanf("%d", &opcionInt);
			t_programa* programaEncontrado = encontrarProgramaPorPID(opcionInt);
			if(programaEncontrado != NULL){
				//finalizar programa
			}
		} else if(opcion[0] == DETENER_PLANIFICACION){
			mostrarYAgregarRuta(rutaOpciones," >> Cambiar el estado de planificacion");
			if(detenerPlanificacion == 0){
				log_info(logger,"La planificacion ha sido detenida por el usuario");
				detenerPlanificacion = 1;
			}else{
				detenerPlanificacion = 0;
				log_info(logger,"La planificacion ha sido resumida por el usuario");
			}
		}
	}
	return 0;
}
