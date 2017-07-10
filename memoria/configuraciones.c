/*
 * configuraciones.c
 *
 *  Created on: 27/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "memoria.h"
#include "configuraciones.h"

void inicializarDataConfig()
{
	t_config * config=config_create(rutaAbsolutaDe("config.cfg"));
	configDeMemoria.puerto = obtenerConfiguracionString(config,"PUERTO");
	configDeMemoria.marcos = obtenerConfiguracion(config, "MARCOS");
    configDeMemoria.tamMarco = obtenerConfiguracion(config, "MARCO_SIZE");
	configDeMemoria.entradasCache = obtenerConfiguracion(config, "ENTRADAS_CACHE");
	configDeMemoria.cachePorProceso = obtenerConfiguracion(config, "CACHE_X_PROC");
	configDeMemoria.retardo = obtenerConfiguracion(config, "RETARDO_MEMORIA");
	configDeMemoria.marcosDisponibles= configDeMemoria.marcos;
	configDeMemoria.cacheDisponible=configDeMemoria.entradasCache;

	system("clear");
	printf("Bienvenido a la memoria!\n\n");
	printf("Se han tomado las siguientes configuraciones:\n");
	printf("Puerto: %s\n", configDeMemoria.puerto);
	printf("Marcos: %i\n",configDeMemoria.marcos);
	printf("Tamaño de Marco: %i\n", configDeMemoria.tamMarco);
	printf("Entradas Cache: %i\n", configDeMemoria.entradasCache);
	printf("Cache x proceso: %i\n", configDeMemoria.cachePorProceso);
	printf("Retardo: %i\n", configDeMemoria.retardo);
	printf("\nPresione Enter Para continuar\n");
	while ( getchar() != '\n');
	system("clear");
}
