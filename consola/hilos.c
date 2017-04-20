/*
 * hilos.c
 *
 *  Created on: 19/4/2017
 *      Author: utnso
 */
#include <stdlib.h>
#include <pthread.h>
#include "operacionesDeConsola.h"

void crearHiloPrograma(char* pathProgramaAnsisop)
{
	//pthread_t hiloPrograma;

	pthread_create(&hiloPrograma,NULL,(void*)gestionarProgramaAnsisop,pathProgramaAnsisop);
	// TODO AVERIGUAR DONDE VA ESTA COSA //pthread_join(hiloPrograma,NULL);
	pthread_join(hiloPrograma,NULL);

}
