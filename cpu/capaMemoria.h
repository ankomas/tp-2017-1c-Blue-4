/*
 * capaMemoria.h
 *
 *  Created on: 3/7/2017
 *      Author: utnso
 */

#ifndef CAPAMEMORIA_H_
#define CAPAMEMORIA_H_

#include <blue4-lib.h>

int cargarDeMemoria(int socket,uint32_t pid,uint32_t pag, uint32_t off,uint32_t size, package_t* paqueteParametro);
char* pedirProgramaAMemoria(t_pcb2 *pcb,int socket);

#endif /* CAPAMEMORIA_H_ */
