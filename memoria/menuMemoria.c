/*
 * menuMemoria.c
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "configuraciones.h"
#include "memoria.h"

void modificarRetardo()
{
	int nuevoRetardo;
	printf("El retardo actual es: %i ms\n", retardo);
	printf("Ingrese el nuevo retardo en ms\n");
	scanf("%i", &nuevoRetardo);
	retardo = nuevoRetardo;
	printf("El nuevo Retardo es: %i\n", retardo);
}

void dumpMenu()
{
	int opcion;
	printf(	"Reportar estado actual de:\n"
			"Cache: Presione 1\n"
			"Estructuras de memoria: Presione 2\n"
			"Contenido de memoria: Presione 3\n");
	scanf("%i", &opcion);
	switch(opcion)
	{
	case 1: dumpCache(); break;
	case 2: dumpEstructurasM(); break;
	case 3: dumpContenidoM(); break;
	}
}

// Estas funciones no estan implementadas

void dumpCache() {};
void dumpEstructurasM() {};
void dumpContenidoM() {};
void flush() {};
void memorySize() {};
void PIDSize() {};

void mostrarMenuMemoria() {
	int opcion;
	while(1)
	{
		printf(	"\nIngrese un comando:\n"
				"Modificar el retardo: Presione 1\n"
				"Reportar estado actual: Presione 2\n"
				"Limpiar el contenido de la cache: Presione 3\n"
				"Indicar el tamaño de la memoria: Presione 4\n"
				"Indicar el tamaño de un proceso: Presione 5\n"
				"Para salir del menu: Presione 6\n\n");
		scanf("%d", &opcion);
		switch(opcion)
		{
		case 1: modificarRetardo(); break;
		case 2: dumpMenu(); break;
		case 3: flush(); break;
		case 4: memorySize(); break;
		case 5: PIDSize(); break;
		}
		if(opcion == 6)
			break;
	}
}
