/*
 * heap.h
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#ifndef HEAP_H_
#define HEAP_H_

#include "main.h"

typedef struct heapMetadata{
	uint32_t size;
	bool isFree;
}heapMetadata;

typedef struct pagina{
	void*contenido;
	uint32_t numero;
	t_list* bloques;
}paginaHeap;

typedef struct bloque {
	uint32_t posicionInicioBloque;
	heapMetadata * metadata;
}bloque;

void iniciarBloqueHeap(paginaHeap* unaPagina);
int guardarDataHeap(paginaHeap* unaPagina,void* data,int32_t tamData);
int memoriaLibre(paginaHeap* unaPagina);
void compactar();

#endif /* HEAP_H_ */
