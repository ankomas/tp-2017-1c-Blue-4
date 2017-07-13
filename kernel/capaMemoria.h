/*
 * capaMemoria.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef CAPAMEMORIA_H_
#define CAPAMEMORIA_H_

#include "commons/collections/queue.h"
#include "blue4-lib.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

typedef struct t_semaforo{
	int32_t valor;
	t_queue * colaEspera;
} t_semaforo;

int cantidadElementosArrayConfig(char* unaRuta,char*unId);
int obtenerTamanioPagina();
int valorSemaforo(char * unSemaforo);
int inicializarEnMemoria(uint32_t i, uint32_t data, uint32_t data2);
void guardarEnHeap(uint32_t i);
void inicializarVariablesCompartidas();
void inicializarSemaforos();
void semWait(uint32_t i,uint32_t *pid);
#endif /* CAPAMEMORIA_H_ */
