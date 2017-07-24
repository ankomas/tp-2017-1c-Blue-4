/*
 * capaFs.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef CAPAFS_H_
#define CAPAFS_H_

#include "main.h"
#include "heapNico.h"
#include "planificador.h"

//TGA es Tabla Global de Archivos
typedef struct t_entradaTGA{
	uint32_t indice;
	char* archivo;
	uint32_t abierto;
}t_entradaTGA;

//TAP es Tabla de Archivos de Proceso
typedef struct t_entradaTAP{
	uint32_t indice;
	char* flags;
	uint32_t globalFD;
	uint32_t cursor;
}t_entradaTAP;

void imprimirPorConsola();
uint32_t abrirFD(uint32_t i,t_programa* unPrograma);
bool cerrarFD(uint32_t i, t_programa* unPrograma);
bool escribirFD(uint32_t i,t_programa* unPrograma);
bool borrarFD(uint32_t i,t_programa* unPrograma);
char* leerFD(uint32_t i,t_programa* unPrograma);

t_entradaTGA * buscarFDPorPath(char* path);
uint32_t buscarFDPorId(uint32_t id);
uint32_t buscarFDArchivoPorId(uint32_t id,t_programa*unPrograma);
bool tienePermisos(char acceso,char*permisos);
char* recibirPath(uint32_t i);
char* recibirPermisos(uint32_t i);
void cantidadMemoryLeak(t_programa* unPrograma);
#endif /* CAPAFS_H_ */
