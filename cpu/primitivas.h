/*
 * primitivas.h
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#ifndef PRIMITIVAS_H_
#define PRIMITIVAS_H_

#include "pcb.h"

t_puntero posAPuntero(t_pos pos,uint32_t tamPag);
t_pos punteroAPos(t_puntero puntero, uint32_t tamPag);
t_pos proxPos(t_pos posActual,uint32_t tamPag);

t_puntero dummy_definirVariable(t_nombre_variable variable);
t_puntero dummy_obtenerPosicionVariable(t_nombre_variable variable);
void dummy_finalizar(void);
t_valor_variable dummy_dereferenciar(t_puntero puntero);
void dummy_asignar(t_puntero puntero, t_valor_variable variable);
void dummy_irAlLabel(t_nombre_etiqueta etiqueta);
void dummy_retornar(t_valor_variable retorno);
void dummy_llamarSinRetorno(t_nombre_etiqueta etiqueta);
void dummy_llamarConRetorno(t_nombre_etiqueta etiqueta, t_puntero donde_retornar_puntero);
t_valor_variable dummy_obtenerValorCompartida(t_nombre_compartida variable);
t_valor_variable dummy_asignarValorCompartida(t_nombre_compartida variable, t_valor_variable valor);

AnSISOP_funciones funciones;




#endif /* PRIMITIVAS_H_*/
