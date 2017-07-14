/*
 * capaMemoria.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef CAPAMEMORIA_H_
#define CAPAMEMORIA_H_

#include "commons/collections/queue.h"
#include "blue4-lib.h"
#include "main.h"
#include "planificador.h"
#include <stdio.h>
#include <string.h>

typedef struct t_semaforo{
	int32_t valor;
	t_queue * colaEspera;
} t_semaforo;

int cantidadElementosArrayConfig(char* unaRuta,char*unId);
int obtenerTamanioPagina();
int valorSemaforo(char * unSemaforo);
int inicializarEnMemoria(uint32_t i, uint32_t pid,uint32_t paginasNecesarias);

int guardarEnMemoria(uint32_t i, uint32_t pid,uint32_t pagina,uint32_t offset,uint32_t tamanioContenido,char*contenido);
int cargarDeMemoria(int socket,uint32_t pid,uint32_t pag, uint32_t off,uint32_t size, package_t* paqueteParametro);

int pedirPagias(uint32_t pid,uint32_t cantPaginas);
int liberarPagina(uint32_t pid,uint32_t pagina);

void guardarEnHeap(uint32_t i,t_list * paginasHeap,uint32_t *pid);
int guardarHeapNico(uint32_t socket,t_programa* programa);
void liberarHeapNico(int cpu,t_programa *proximoPrograma);

void inicializarVariablesCompartidas();
void inicializarSemaforos();
void semWait(uint32_t i,uint32_t *pid);
#endif /* CAPAMEMORIA_H_ */
