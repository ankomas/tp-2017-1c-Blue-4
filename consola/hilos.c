/*
 * hilos.c
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */
#include <stdlib.h>
#include <pthread.h>
#include "operacionesDeConsola.h"
#include "conexiones.h"
#include "funcionesAuxiliares.h"
#include "hilos.h"



void createOperacionesHiloPrograma(dataHilos_t dataHilo)
{
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    socket_kernel=conectarseAlKernel();
	dataHilo.socket=socket_kernel;
	gestionarProgramaAnsisop(dataHilo);
}

void crearHiloPrograma(char* pathProgramaAnsisop)
{
	//pthread_t hilo;
	dataHilos_t dataHilo;

	//pthread_attr_init(&hiloDetachable);
	//pthread_attr_setdetachstate(&hiloDetachable,PTHREAD_CREATE_DETACHED);
	// hacer que el hilo se conecte con el kernel y enviarle el codigo de un programa ANSISOP recibiendo de este un PID

	dataHilo.path=pathProgramaAnsisop;
	dataHilo.hiloPrograma=hiloDetachable;
	pthread_create(&hilo,NULL,(void*)createOperacionesHiloPrograma,(void*)&dataHilo);
	// TODO AVERIGUAR DONDE VA ESTA COSA
	//pthread_join(hiloPrograma,NULL);
	//pthread_attr_destroy(&hiloDetachable);

}


