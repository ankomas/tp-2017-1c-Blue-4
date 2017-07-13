/*
 * primitivasKernel.h
 *
 *  Created on: 13/7/2017
 *      Author: utnso
 */

#ifndef PRIMITIVASKERNEL_H_
#define PRIMITIVASKERNEL_H_

#include "pcb.h"

//semaforos
void dummy_wait(t_nombre_semaforo identificador_semaforo);
void dummy_signal(t_nombre_semaforo identificador_semaforo);

AnSISOP_kernel kernel_functions;

#endif /* PRIMITIVASKERNEL_H_ */
