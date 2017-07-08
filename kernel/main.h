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
#include "heap.h"
#include "pcb.h"

extern int cantidadProgramasEnSistema,idFS,idUMC,gradoMultiprogramacion,retardo,quantum,tamanioPagina,tamanioStack;
extern t_list* CPUs;
extern t_dictionary * semaforos;
extern t_dictionary * variablesCompartidas;



typedef struct t_cpu {
	uint32_t id;
	t_pcb *programaEnEjecucion;
	bool disponible;
	pthread_t hilo;
} t_cpu;

extern t_log* logger;

#endif /* MAIN_H_ */
