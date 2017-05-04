/*
 * planificador.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_
#include "commons/collections/queue.h"
#include <stdint.h>

extern t_queue * procesosREADY;
extern t_queue * procesosEXEC;
extern t_queue * procesosBLOCK;
extern t_queue * procesosEXIT;

typedef struct t_pcb {
	uint32_t pid;
	uint32_t pc;
	void * tablaArchivos;
	void * posicionStack;
	uint32_t exitCode;
	uint32_t cantidadPaginasAsignadas;
	uint32_t quantumRestante;
} t_pcb;

t_pcb* planificador();
void *cpu();

#endif /* PLANIFICADOR_H_ */
