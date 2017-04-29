/*
 * planificador.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef PLANIFICADOR_H_
#define PLANIFICADOR_H_

#include <stdint.h>
#include "commons/collections/queue.h"

extern t_queue * procesosNEW;
extern t_queue * procesosREADY;
extern t_queue * procesosEXEC;
extern t_queue * procesosBLOCK;
extern t_queue * procesosEXIT;

void *planificador();

#endif /* PLANIFICADOR_H_ */
