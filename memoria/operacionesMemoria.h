/*
 * operacionesMemoria.h
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#ifndef OPERACIONESMEMORIA_H_
#define OPERACIONESMEMORIA_H_

#include "memoria.h"

void inicializarPrograma(uint32_t ,uint32_t , void* );

/**
 * 	Devuelve la tabla de paginas!
 * 	@Valor de Retorno: tablaPaginas_t*
 * 	NOTA: HACER EL FREE!
 */
tablaPaginas_t* obtenerTablaDePaginas();

#endif /* OPERACIONESMEMORIA_H_ */
