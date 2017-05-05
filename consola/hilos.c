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



void createOperacionesHiloPrograma(dataHilos_t* dataHilo)
{
	//pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
     int socket_kernel=conectarseAlKernel();
	(*dataHilo).socketKernel=socket_kernel;
 	//printf("el socket antes de enviar es: %d \n",(*dataHilo).socketKernel);
	//printf("el path antes de enviar es: %s \n",(*dataHilo).path);
	gestionarProgramaAnsisop(dataHilo);
}

void crearHiloPrograma(char* pathProgramaAnsisop)
{
	//pthread_t hilo;
	// TODO LIBERAR MEMORIA CUANDO SE ELIMINE UN dataHilos_t
	dataHilos_t* dataHilo=malloc(sizeof(dataHilos_t));

	pthread_attr_t hiloDetachable;
	pthread_t hilo;
	pthread_attr_init(&hiloDetachable);
	pthread_attr_setdetachstate(&hiloDetachable,PTHREAD_CREATE_DETACHED);
	// hacer que el hilo se conecte con el kernel y enviarle el codigo de un programa ANSISOP recibiendo de este un PID
	printf("el path antes de asignar es: %s \n",pathProgramaAnsisop);
	dataHilo->path=malloc(strlen(pathProgramaAnsisop)+1);
	memset(dataHilo->path,'\0',strlen(pathProgramaAnsisop)+1);
	memcpy(dataHilo->path,pathProgramaAnsisop,strlen(pathProgramaAnsisop));
	//printf("el path despues de asignar es: %s \n",dataHilo->path);
	dataHilo->hiloPrograma=hiloDetachable;
	dataHilo->hilo=hilo;
	//TODO averiguar como puedo liberar esto
	//free(pathProgramaAnsisop);
	pthread_create(&dataHilo->hilo,&hiloDetachable,(void*)createOperacionesHiloPrograma,(void*)dataHilo);
	// TODO esto va aca ???
	//pthread_attr_destroy(&hiloDetachable);

}


