/*
 * main.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "commons/log.h"

#include "blue4-lib.h"
#include "capaMemoria.h"
#include "capaFs.h"
#include "consolaKernel.h"
#include "sockets.h"
#include "planificador.h"

typedef struct t_pcb {
	int pid;
	int pc;
	void * tablaArchivos;
	void * posicionStack;
	int exitCode;
} t_pcb;

extern t_log* logger;

#endif /* MAIN_H_ */
