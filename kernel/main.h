/*
 * main.h
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

#include "commons/config.h"
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

const char PROGRAMA_ID;
const char KERNEL_ID;
const char CPU_ID;
const char FS_ID;
const char UMC_ID;

extern int cantidadProgramasEnSistema,idFS,idUMC,gradoMultiprogramacion,retardo,quantum,tamanioPagina,tamanioStack;
extern uint32_t GlobalFDCounter;
extern t_list* CPUs;
extern t_list* PROGRAMAs;
extern t_dictionary * semaforos;
extern t_dictionary * variablesCompartidas;
extern t_list * tablaGlobalArchivos;
extern t_config *cfg;

extern t_log* logger;

#endif /* MAIN_H_ */
