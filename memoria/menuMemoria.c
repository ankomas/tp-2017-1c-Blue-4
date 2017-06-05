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
#include "estructurasAdministrativas.h"
#include "operacionesMemoria.h"

void modificarRetardo()
{
	int nuevoRetardo;
	printf("El retardo actual es: %i ms\n", configDeMemoria.retardo);
	printf("Ingrese el nuevo retardo en ms\n");
	scanf("%i", &nuevoRetardo);
	configDeMemoria.retardo = nuevoRetardo;
	printf("El nuevo Retardo es: %i\n", configDeMemoria.retardo);
}

void dumpCache() {};

void mostrarTablaDePaginas(){
	int i;
	tablaPaginas_t* tablaDePaginas = obtenerTablaDePaginas();
	//for(i=0; i<configDeMemoria.marcos;i++)
	for(i=0; i<50;i++)
		printf("Marco: %i	 PID: %i	Pagina: %i\n",i,tablaDePaginas[i].pid, tablaDePaginas[i].pagina );
}
void dumpEstructuras() { //Falta lista de procesos Activos
	mostrarTablaDePaginas();
	//mostrarListaProcesosActivos();
};



void dumpContenido() { //BIEEEEN RANCIO
	printf("La memoria contiene %s", (char*) memoria);
};

void dump()
{
	int opcion; //int PID;
	printf(	"Reportar estado actual de:\n"
			"Cache: Presione 1\n"
			"Estructuras de memoria: Presione 2\n"
			"Contenido de memoria: Presione 3\n");
	scanf("%i", &opcion);
	switch(opcion)
	{
	case 1: dumpCache(); break;
	case 2: dumpEstructuras(); break;
	case 3: {
		//printf( "Ingrese el PID del proceso o ingrese -2 si quiere ver el contenido en memoria de todos los procesos\n");
		//scanf("%i", &PID);
		//dumpContenido(PID);
		dumpContenido();
		break;
	}
	}
}

// Estas funciones no estan implementadas

void flush() {};
void memorySize() {};
void PIDSize() {};

void cantRunAway(){
	system("clear");
	printf("	Saliendo de memoria...\n");
	sleep(4);
	printf("\n\n	TE LA CREISTE WE XDXD \n\n\n");
	sleep(2);
	printf("\n\n	Nah, enserio, no podes irte!\n");
}

void mostrarMenuMemoria() {
	int opcion;
	while(1)
	{

		printf(	"Ingrese un comando:\n"
				"Modificar el retardo: Presione 1\n"
				"Reportar estado actual: Presione 2\n"
				"Limpiar el contenido de la cache: Presione 3\n"
				"Indicar el tamaño de la memoria: Presione 4\n"
				"Indicar el tamaño de un proceso: Presione 5\n"
				"Para limpiar la pantalla: Presione 6\n"
				"Para salir del menu: Presione 7\n");
		scanf("%d", &opcion);
		switch(opcion)
		{
		case 1: modificarRetardo(); break;
		case 2: dump(); break;
		case 3: flush(); break;
		case 4: memorySize(); break;
		case 5: PIDSize(); break;
		case 6: system("clear"); break;
		case 7: cantRunAway(); break;
		}
	}
}
