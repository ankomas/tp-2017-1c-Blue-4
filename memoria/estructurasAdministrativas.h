/*
 * estructurasAdministrativas.h
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */

#ifndef ESTRUCTURASADMINISTRATIVAS_H_
#define ESTRUCTURASADMINISTRATIVAS_H_

void* memoria, *cache;

void inicializarMemoria();
/*
 * 	Crea la memoria inicializada en pid = -2, y carga la tabla de paginas con pid = -1
 */

void inicializarCache();

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

/**
 * actualiza los marcos disponibles en mi sistema
 * @param marcos
 * @return codigo de exito o error.
 */
int actualizarMarcosDisponibles(int );


/**
 * Cant de marcos que requiere una cant de memoria
 * @Parametro: Tamaño
 * @Valor de Retorno: Entero
 */
int cuantosMarcosRepresenta(int);

/**
 * Funcion creada para inicializar la memoria
 */
void inicializarMemoria();

#endif /* ESTRUCTURASADMINISTRATIVAS_H_ */
