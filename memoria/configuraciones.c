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
	configDeMemoria.puerto = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO");
	configDeMemoria.marcos = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "MARCOS");
    configDeMemoria.tamMarco = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "MARCO_SIZE");
	configDeMemoria.entradasCache = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "ENTRADAS_CACHE");
	configDeMemoria.cachePorProceso = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "CACHE_X_PROC");
	configDeMemoria.retardo = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "RETARDO_MEMORIA");
	configDeMemoria.marcosDisponibles= configDeMemoria.marcos;

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
