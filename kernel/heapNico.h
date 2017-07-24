/*
 * heapNico.h
 *
 *  Created on: 13/7/2017
 *      Author: utnso
 */

#ifndef HEAPNICO_H_
#define HEAPNICO_H_

#include <commons/collections/list.h>
#include "planificador.h"

typedef struct{
	uint32_t pid;
	uint32_t nPag;
	uint32_t tamDisp;
}t_heap;

typedef struct{
	bool free;
	uint32_t size;
} t_metadata_heap;

t_puntero allocEnHeap(t_programa *programa,uint32_t tam);
int liberar(t_programa* programa,t_puntero puntero);

#endif /* HEAPNICO_H_ */
