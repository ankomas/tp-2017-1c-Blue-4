/*
 * capaMemoria.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef CAPAMEMORIA_H_
#define CAPAMEMORIA_H_

#include "blue4-lib.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

int cantidadElementosArrayConfig(char* unaRuta,char*unId);
int obtenerTamanioPagina();
int valorSemaforo(char * unSemaforo);
int solicitarMemoria(uint32_t i, uint32_t data, uint32_t data2);
void inicializarVariablesCompartidas();
void inicializarSemaforos();
#endif /* CAPAMEMORIA_H_ */
