/*
 * operacionesMemoria.h
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#ifndef OPERACIONESMEMORIA_H_
#define OPERACIONESMEMORIA_H_

#include "memoria.h"

pthread_mutex_t escribiendoMemoria, escribiendoMemoriaCache;

void inicializarPrograma(uint32_t ,uint32_t , void* );


/**
 * LeerMemoria(pid, pag, offset, tamañoLectura)
 * Valor de Retorno: void* memoriaBuscada
 */
void *leerMemoria(uint32_t, uint32_t, uint32_t, uint32_t);

/**
 * escribirMemoria(pid, pag, offset, tamañoEscritura, datosAEscribir)
 * Escribe la memoria. Funcion sin Retorno.
 */
void escribirMemoria(uint32_t, uint32_t, uint32_t, uint32_t, void*);

void *leerCache(uint32_t, uint32_t, uint32_t, uint32_t);

void escribirCache(uint32_t, uint32_t, uint32_t, uint32_t, void*);

tablaPaginas_t* obtenerTablaDePaginas();

/**
 *
 * @param (int marcos) cantidad de marcosa evaluar
 * @return bool
 */
int tieneMarcosSuficientes(int );

#endif /* OPERACIONESMEMORIA_H_ */
