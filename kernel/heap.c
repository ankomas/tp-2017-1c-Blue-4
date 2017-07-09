/*
 * heap.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#include "heap.h"

heapMetadata* crearMetadata(){
	heapMetadata *nuevoMetadata = malloc(sizeof(heapMetadata));
	nuevoMetadata->isFree = true;
	nuevoMetadata->size = tamanioPagina-sizeof(heapMetadata);
	return nuevoMetadata;
}

void iniciarBloqueHeap(paginaHeap* unaPagina){
	heapMetadata *nuevoMetadata = crearMetadata();
	unaPagina->contenido = malloc(tamanioPagina);
	memcpy(unaPagina->contenido,nuevoMetadata,sizeof(heapMetadata));

	bloque * nuevoBloque = malloc(sizeof(bloque));
	unaPagina->bloques = list_create();
	nuevoBloque->posicionInicioBloque = 0;
	nuevoBloque->metadata = nuevoMetadata;
	list_add(unaPagina->bloques,nuevoBloque);
}

int guardarDataHeap(paginaHeap* unaPagina,void* data,int32_t tamData){
	int i;
	if(list_size(unaPagina->bloques) > 0){
		while(i < list_size(unaPagina->bloques)){
			bloque * bloqueAux = list_get(unaPagina->bloques,i);
			if(bloqueAux->metadata->isFree == 1 && bloqueAux->metadata->size >=tamData){
				bloqueAux->metadata->isFree = 0;
				memcpy(
					unaPagina->contenido+
					bloqueAux->posicionInicioBloque+
					sizeof(heapMetadata),
					data,
					tamData
				);
				return 0; //Guardado con exito
			}
			i++;
		}
		return -1; // No hay espacio en esta pagina
	}
	return -2; // La pagina no esta inicializada
}

void liberarMemoria(){

}

/*void bajarHeapAMemoria(){

}*/

int memoriaLibre(){
	return 0;
}

int memoriaReservada(){
	return 0;
}

void compactar(){

}
