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
     if(socket_kernel==-1)
     {
    	 textoRojo("\n NO SE PUDO CONECTAR AL KERNEL");
    	 free(dataHilo->path);
    	 free(dataHilo);
    	 pthread_exit(NULL);
    	 return;
     }
	(*dataHilo).socketKernel=socket_kernel;
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
	dataHilo->path=malloc(strlen(pathProgramaAnsisop)+1);
	memset(dataHilo->path,'\0',strlen(pathProgramaAnsisop)+1);
	memcpy(dataHilo->path,pathProgramaAnsisop,strlen(pathProgramaAnsisop));
	dataHilo->hiloPrograma=hiloDetachable;
	dataHilo->hilo=hilo;
	pthread_create(&dataHilo->hilo,&hiloDetachable,(void*)createOperacionesHiloPrograma,(void*)dataHilo);

}


