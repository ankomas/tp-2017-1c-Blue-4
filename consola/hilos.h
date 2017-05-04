/*
 * hilos.h
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */

#ifndef HILOS_H_
#define HILOS_H_

#include <pthread.h>
#include <semaphore.h>

//pthread_t hilo;

typedef struct
{
	char* path;
	int socketKernel;
	int pidHilo;
	pthread_attr_t hiloPrograma;
	pthread_t hilo;

}dataHilos_t;
//int socket_kernel;
void crearHiloPrograma(char*);

#endif /* HILOS_H_ */
