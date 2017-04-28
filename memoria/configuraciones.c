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
//#include "configuraciones.h"
#include "memoria.h"

void configurarTodo()
{
	puerto = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO");
	marcos = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "MARCOS");
	tamMarco = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "MARCO_SIZE");
	entradasCache = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "ENTRADAS_CACHE");
	cachePorProceso = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "CACHE_X_PROC");
	retardo = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"), "RETARDO_MEMORIA");

	system("clear");
	printf("Bienvenido a la memoria!\n\n");
	printf("Se han tomado las siguientes configuraciones:\n");
	printf("Puerto: %s\n", puerto);
	printf("Marcos: %i\n",marcos);
	printf("Tamaño de Marco: %i\n", tamMarco);
	printf("Entradas Cache: %i\n", entradasCache);
	printf("Cache x proceso: %i\n", cachePorProceso);
	printf("Retardo: %i\n", retardo);
	printf("\nPresione Cualquier tecla Para continuar\n");
	while ( getchar() != '\n');
	system("clear");
}
