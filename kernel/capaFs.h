/*
 * capaFs.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef CAPAFS_H_
#define CAPAFS_H_

//TGA es Tabla Global de Archivos
typedef struct t_entradaTGA{
	char* archivo;
	uint32_t abierto;
}t_entradaTGA;

//TAP es Tabla de Archivos de Proceso
typedef struct t_entradaTAP{
	char* flags;
	uint32_t abierto;
}t_entradaTAP;

void imprimirPorConsola();
uint32_t abrirFD(char* path);
void cerrarFD(uint32_t fd);
void escribirFD(uint32_t fd);
void leerFD(uint32_t fd);


#endif /* CAPAFS_H_ */
