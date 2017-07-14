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
	uint32_t indice;
	char* archivo;
	uint32_t abierto;
}t_entradaTGA;

//TAP es Tabla de Archivos de Proceso
typedef struct t_entradaTAP{
	uint32_t indice;
	char* flags;
	uint32_t globalFD;
}t_entradaTAP;

void imprimirPorConsola();
uint32_t abrirFD(t_programa* unPrograma, char* permisos);
void cerrarFD(t_programa* unPrograma,uint32_t fd);
uint32_t escribirFD(t_programa* unPrograma,void* data,uint32_t fd, uint32_t offset, uint32_t tamanio);
uint32_t leerFD(t_programa* unPrograma,uint32_t fd, uint32_t offset, uint32_t tamanio);

t_entradaTGA * buscarFDPorPath(char* path);
uint32_t buscarFDPorId(uint32_t id);
uint32_t buscarFDArchivoPorId(uint32_t id,t_programa*unPrograma);
bool tienePermisos(char acceso,char*permisos);
#endif /* CAPAFS_H_ */
