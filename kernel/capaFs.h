/*
 * capaFs.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef CAPAFS_H_
#define CAPAFS_H_

#include "main.h"
#include "planificador.h"

//TGA es Tabla Global de Archivos
typedef struct t_entradaTGA{
	char* archivo;
	uint32_t abierto;
}t_entradaTGA;

//TAP es Tabla de Archivos de Proceso
typedef struct t_entradaTAP{
	char* flags;
	uint32_t globalFD;
}t_entradaTAP;

void imprimirPorConsola();
uint32_t abrirFD(t_programa* unPrograma,char* path, char* permisos);
void cerrarFD(t_programa* unPrograma,uint32_t fd);
void escribirFD(t_programa* unPrograma,uint32_t fd, uint32_t offset, uint32_t tamanio);
void leerFD(t_programa* unPrograma,uint32_t fd, uint32_t offset, uint32_t tamanio);

#endif /* CAPAFS_H_ */
