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
#include <blue4-lib.h>
#include "pcb.h"

extern t_queue * procesosNEW;
extern t_queue * procesosREADY;
extern t_queue * procesosEXEC;
extern t_queue * procesosBLOCK;
extern t_queue * procesosEXIT;

typedef struct t_programa {
	void * tablaArchivos;
	uint32_t quantumRestante;
	uint32_t paginasCodigo;
	char* codigo;
	t_pcb* pcb;
} t_programa;

t_programa* planificador();
void *cpu();
void encolarReady();
void moverPrograma(t_programa* unPrograma,t_queue* colaOrigen, t_queue* colaDestino);

#endif /* PLANIFICADOR_H_ */
