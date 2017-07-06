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
}pagina;

#endif /* HEAP_H_ */
