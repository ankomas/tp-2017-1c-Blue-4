/*
 * operacionesMemoria.h
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#ifndef OPERACIONESMEMORIA_H_
#define OPERACIONESMEMORIA_H_

extern void* memoria;

void inicializarMemoria();
/*
 * 	Crea la memoria inicializada en pid = -2, y carga la tabla de paginas con pid = -1
 */

uint32_t cargarAMemoria(uint32_t, void*, uint32_t);
/*
 * 	Carga en memoria un dato
 * 	No puedo probar si anda :/
 */

void mostrarDeMemoria(uint32_t);
/*
 * 	Muestra un marco
 * 	No anda bien :/
 */

#endif /* OPERACIONESMEMORIA_H_ */
