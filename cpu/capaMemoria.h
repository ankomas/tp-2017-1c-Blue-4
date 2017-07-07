/*
 * capaMemoria.h
 *
 *  Created on: 3/7/2017
 *      Author: utnso
 */

#ifndef CAPAMEMORIA_H_
#define CAPAMEMORIA_H_

#include <blue4-lib.h>

int pedirTamGlobal(int socket);
int guardarEnMemoria(uint32_t i, uint32_t pid,uint32_t pagina,uint32_t offset,uint32_t tamanioContenido,char*contenido);
int cargarDeMemoria(int socket,uint32_t pid,uint32_t pag, uint32_t off,uint32_t size, package_t* paqueteParametro);
char* pedirProgramaAMemoria(t_pcb2 *pcb,int socket);
char* pedirLineaAMemoria(t_pcb2* pcb,uint32_t start,uint32_t off);

#endif /* CAPAMEMORIA_H_ */
