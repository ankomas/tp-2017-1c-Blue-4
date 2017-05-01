/*
 * consolaKernel.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "planificador.h"

#define LISTADO_PROCESOS '1'
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


void listarProcesos(){
	//printf("cantidad procesos actuales: %d\n",queue_size(procesosREADY));
}

void *consolaKernel(){
	while(1){
		printf("soy la consola del kernel\n");
		char opcion[1];
		scanf("%s", opcion);
		printf("opcion elegida: %c\n", opcion[0]);

		if(opcion[0] == LISTADO_PROCESOS){
			listarProcesos();
		} else if(opcion[0] == INFO_PROCESO){
			scanf("%s", opcion);
			if(opcion[0] == CANTIDAD_RAFAGAS_EJECUTADAS){

			} else if(opcion[0] == CANTIDAD_OP_PRIVILEGIADAS){

			} else if(opcion[0] == OBTENER_TABLA_ARCHIVOS_ABIERTOS){

			} else if(opcion[0] == CANTIDAD_PAGINAS_HEAP){
				scanf("%s", opcion);
				if(opcion[0] == CANTIDAD_ACCIONES_ALOCAR){

				} else if(opcion[0] == CANTIDAD_ACCIONES_LIBERAR){

				}
			} else if(opcion[0] == CANTIDAD_SYSCALLS_EJECUTADAS){

			}
		} else if(opcion[0] == TABLA_GLOBAL_ARCHIVOS){

		} else if(opcion[0] == MODIFICAR_GRADO_MULTIPROGRAMACION){

		} else if(opcion[0] == FINALIZAR_PROCESO){

		} else if(opcion[0] == DETENER_PLANIFICACION){

		}

	}
	return 0;
}
