/*
 * heap.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#include "heap.h"

void iniciarPaginaHeap(pagina* unaPagina){
	heapMetadata *nuevoMetadata = malloc(sizeof(heapMetadata));
	nuevoMetadata->isFree = true;
	nuevoMetadata->size = tamanioPagina-sizeof(heapMetadata);
	testi(nuevoMetadata->size);
	unaPagina->contenido = calloc(tamanioPagina,1);
	unaPagina->contenido = nuevoMetadata;
	anuncio(unaPagina->contenido);
}

void guardarDataHeap(){

}

void liberarMemoria(){

}

/*void bajarHeapAMemoria(){

}*/

void escribirHeap(){

}

int memoriaLibre(){
	return 0;
}

int memoriaReservada(){
	return 0;
}

void compactar(){

}
