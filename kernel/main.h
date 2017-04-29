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

extern int cantidadProgramasEnSistema,idFS,idUMC,gradoMultiprogramacion;

extern t_list * CONSOLAs;
extern t_list* CPUs;

typedef struct t_pcb {
	int pid;
	int pc;
	void * tablaArchivos;
	void * posicionStack;
	int exitCode;
	int cantidadPaginasAsignadas;
} t_pcb;

typedef struct t_cpu {
	int id;
	t_pcb *programaEnEjecucion;
	bool disponible;
} t_cpu;

typedef struct t_consola {
	int id;
	bool disponible;
} t_consola;

extern t_log* logger;

#endif /* MAIN_H_ */
