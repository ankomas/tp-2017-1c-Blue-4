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

t_puntero dummy_reservar(t_valor_variable espacio);
void dummy_liberar(t_puntero puntero);

t_descriptor_archivo dummy_abrir(t_direccion_archivo direccion, t_banderas flags);
void dummy_borrar(t_descriptor_archivo descriptor_archivo);
void dummy_cerrar(t_descriptor_archivo descriptor_archivo);

AnSISOP_kernel kernel_functions;

#endif /* PRIMITIVASKERNEL_H_ */
