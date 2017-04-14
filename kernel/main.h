/*
 * main.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "blue4-lib.h"
#include "semaforos.h"
#include "sockets.h"

typedef struct t_pcb {
	int pid;
	int pc;
	void * tablaArchivos;
	void * posicionStack;
	int exitCode;
} t_pcb;

#endif /* MAIN_H_ */
