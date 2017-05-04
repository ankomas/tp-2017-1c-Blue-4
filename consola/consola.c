/*
 * consola.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */


#include "conexiones.h"
#include <blue4-lib.h>
#include <commons/config.h>
#include<stdio.h>
#include"operacionesDeConsola.h"
#include "hilos.h"
#include "funcionesAuxiliares.h"


void inicializarEstructurasAdministrativas()
{
	dataDeHilos = list_create();
	pthread_mutex_init(&mutexDataDeHilos,NULL);
	pthread_mutex_init(&mutexAListas,NULL);
	sem_init(&semaforo,0,0);

}



int main()
{

	inicializarEstructurasAdministrativas();
	menuDeControl();
	//pthread_create(&hiloPrincipal,NULL,(void*)crearMenuPrincipal,NULL);
	//pthread_join(hiloPrincipal,NULL);
	crearMenuPrincipal();
	return 0;
}

