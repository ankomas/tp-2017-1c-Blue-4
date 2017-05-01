/*
 * main.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "commons/collections/dictionary.h"
#include "commons/log.h"
#include "commons/string.h"

#include "blue4-lib.h"
#include "capaMemoria.h"
#include "capaFs.h"
#include "consolaKernel.h"
#include "sockets.h"
#include "planificador.h"

extern int cantidadProgramasEnSistema,idFS,idUMC,gradoMultiprogramacion,retardo;
extern t_list * CONSOLAs;
extern t_list* CPUs;

typedef struct t_pcb {
	uint32_t pid;
	uint32_t pc;
	void * tablaArchivos;
	void * posicionStack;
	uint32_t exitCode;
	uint32_t cantidadPaginasAsignadas;
} t_pcb;

typedef struct t_cpu {
	uint32_t id;
	t_pcb *programaEnEjecucion;
	bool disponible;
	pthread_t hilo;
} t_cpu;

typedef struct t_consola {
	uint32_t id;
	bool disponible;
} t_consola;

extern t_log* logger;

#endif /* MAIN_H_ */
