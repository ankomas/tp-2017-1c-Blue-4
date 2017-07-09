/*
 * heap.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#include "heap.h"

void iniciarPaginaHeap(pagina* unaPagina){
	// Creo un Metadata
	heapMetadata *nuevoMetadata = malloc(sizeof(heapMetadata));
	nuevoMetadata->isFree = true;
	nuevoMetadata->size = tamanioPagina-sizeof(heapMetadata);
	testi(nuevoMetadata->size);
	testi(nuevoMetadata->isFree);

	//Agrego el metadata a la pagina
	unaPagina->contenido = malloc(tamanioPagina);

	memcpy(unaPagina->contenido,nuevoMetadata,sizeof(heapMetadata));
	heapMetadata *a = malloc(sizeof(heapMetadata));
	memcpy(a,unaPagina->contenido,sizeof(heapMetadata));
	uint32_t b = a->isFree;
	testi(b);

	//testi(*((int*)a));

	/*void * data = malloc(8);
	uint32_t r = 10, s=20;
	memcpy(data,&r,sizeof(uint32_t));
	memcpy(data+4,&s,sizeof(uint32_t));

	uint32_t * r2 = malloc(4);
	uint32_t * s2 = malloc(4);
	memcpy(r2,data,sizeof(uint32_t));
	memcpy(s2,data+4,sizeof(uint32_t));

	printf("r2: %i \n",*((int*)r2));
	printf("s2: %i \n",*((int*)s2));*/
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
