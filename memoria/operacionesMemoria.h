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
pthread_mutex_t mutex_tablaDePaginas,mutex_tablaCache;

uint32_t tamanioDeTabla();

uint32_t tamanioDeTablaCache();

int inicializarPrograma(uint32_t ,uint32_t);

tablaPaginas_t* obtenerTablaDePaginas();
tablaCache_t* obtenerTablaCache();

/**
 * Leer (pid, pag, offset, tamañoLectura)
 * lee de memoria o cache
 * @return void* memoriaBuscada en caso de exito || NULL en caso de error
 */
void *leer(uint32_t, uint32_t, uint32_t, uint32_t);

/**
 * escribirMemoria(pid, pag, offset, tamañoEscritura, datosAEscribir)
 * Escribe la memoria y el cache
 * @return 0 en caso de exito || -1 en caso de error
 */
int escribir(uint32_t, uint32_t, uint32_t, uint32_t, void*);

void *leerMemoria(uint32_t, uint32_t, uint32_t, uint32_t);

uint32_t escribirMemoria(uint32_t, uint32_t, uint32_t, uint32_t, void*);

void *leerCache(uint32_t, uint32_t, uint32_t, uint32_t);

void escribirCache(uint32_t, uint32_t, uint32_t, void*);

tablaPaginas_t* obtenerTablaDePaginas();

/**
 *
 * @param (int marcos) cantidad de marcosa evaluar
 * @return bool
 */
int tieneMarcosSuficientes(int );

/**
 * Elimina la pagina dada por parametro de un proceso
 * @param uint32_t pid
 * @param uint32_t pagina a eliminar
 * @return 0 en caso de exito || -1 en caso de error
 */
int eliminarPaginaDeUnProceso(uint32_t ,uint32_t );

/**
 * funcion que sirve para que el kernel finalize un proceso
 * setea la tabla de paginas en -2 correspondiente al lugar donde estaba ese pid.
 *
 * @param uint32_t pid (identificador unico de un proceso)
 * @return 0 en casod exito || -1 en caso de error
 */
uint32_t finalizarPrograma(uint32_t );


/**
 * Agranda un proceso dandole mas paginas
 * @param uint32_t pid (identificador unico de un proceso)
 * @param uint32_t paginas requeridas
 * @return 0 en caso de exito || -1 en caso de error
 */
int asignarPaginasAUnProceso(uint32_t ,uint32_t );

int agregarProcesoACache(int, int);

int validarPIDPAG(int, int);

#endif /* OPERACIONESMEMORIA_H_ */
