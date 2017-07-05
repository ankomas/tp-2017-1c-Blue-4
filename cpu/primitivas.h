/*
 * primitivas.h
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_

#include "pcb.h"

t_puntero dummy_definirVariable(t_nombre_variable variable);
t_puntero dummy_obtenerPosicionVariable(t_nombre_variable variable);
void dummy_finalizar(void);
t_valor_variable dummy_dereferenciar(t_puntero puntero);
void dummy_asignar(t_puntero puntero, t_valor_variable variable);

AnSISOP_funciones funciones;
AnSISOP_kernel kernel_functions;



#endif /* PRIMITIVAS_H_*/
