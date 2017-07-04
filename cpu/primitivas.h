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

#ifdef FUNCIONES_
AnSISOP_funciones funciones = {
		.AnSISOP_definirVariable		= dummy_definirVariable,
		.AnSISOP_obtenerPosicionVariable= dummy_obtenerPosicionVariable,
		.AnSISOP_finalizar 				= dummy_finalizar,
		.AnSISOP_dereferenciar			= dummy_dereferenciar,
		.AnSISOP_asignar				= dummy_asignar,

};
AnSISOP_kernel kernel_functions = { };
#endif /* FUNCIONES_*/


#endif /* PRIMITIVAS_H_*/
