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


t_config* config;


void inicializarEstructurasAdministrativas()
{
	char* ruta=rutaAbsolutaDe("configConsola.cfg");
	config = config_create(ruta);
	free(ruta);
	dataDeHilos = list_create();
	dataDeProcesos= list_create();
	pthread_mutex_init(&mutexDataDeHilos,NULL);
	pthread_mutex_init(&mutexAListas,NULL);
	pthread_mutex_init(&mutexDataDeProcesos,NULL);
	//sem_init(&semaforo,0,0);

}



int main()
{

	inicializarEstructurasAdministrativas();
	menuDeControl();
	crearMenuPrincipal();
	return 0;
}

