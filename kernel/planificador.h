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
	uint32_t id;
	void * tablaArchivos;
	uint32_t quantumRestante;
	uint32_t paginasCodigo;
	char* codigo;
	t_pcb* pcb;
	t_dictionary * paginasHeap;
} t_programa;

typedef struct t_cpu {
	uint32_t id;
	t_pcb *programaEnEjecucion;
	bool disponible;
	pthread_t hilo;
} t_cpu;

t_cpu * encontrarCPU(uint32_t i);
t_cpu * encontrarCPUporPID(uint32_t pid);
t_programa * encontrarPrograma(uint32_t i);
t_programa * inicializarPrograma(uint32_t i,uint32_t pidActual);
void* programa(t_programa* programa);

t_programa* planificador();
void *cpu();
void encolarReady();
void moverPrograma(t_programa* unPrograma,t_queue* colaOrigen, t_queue* colaDestino);
void semWait(uint32_t i);
void semSignal(uint32_t i);
void guardarEnHeap(uint32_t i);
void leerHeap(uint32_t i);
void guardarVarGlobal(uint32_t i);
void leerVarGlobal(uint32_t i);
#endif /* PLANIFICADOR_H_ */
