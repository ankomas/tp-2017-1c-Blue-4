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


t_configMemoria obtenerDataConfig()
{
	t_configMemoria configMemoria;
	configMemoria.puerto = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO");
	configMemoria.marcos = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "MARCOS");
    configMemoria.tamMarco = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "MARCO_SIZE");
	configMemoria.entradasCache = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "ENTRADAS_CACHE");
	configMemoria.cachePorProceso = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "CACHE_X_PROC");
	configMemoria.retardo = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "RETARDO_MEMORIA");

	system("clear");
	printf("Bienvenido a la memoria!\n\n");
	printf("Se han tomado las siguientes configuraciones:\n");
	printf("Puerto: %s\n", configMemoria.puerto);
	printf("Marcos: %i\n",configMemoria.marcos);
	printf("Tamaño de Marco: %i\n", configMemoria.tamMarco);
	printf("Entradas Cache: %i\n", configMemoria.entradasCache);
	printf("Cache x proceso: %i\n", configMemoria.cachePorProceso);
	printf("Retardo: %i\n", configMemoria.retardo);
	printf("\nPresione Enter Para continuar\n");
	while ( getchar() != '\n');
	system("clear");

	return configMemoria;
}
