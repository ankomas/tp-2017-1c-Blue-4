/*
 * menuMemoria.c
 *
 *  Created on: 26/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <string.h>
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
	//todo mutexear retardo!!!
	texto_en_color_de_exito("El retardo actual es", configDeMemoria.retardo);
	texto_en_color("\nIngrese el nuevo retardo en ms");
	scanf("%i", &nuevoRetardo);
	configDeMemoria.retardo = nuevoRetardo;
	texto_en_color_de_exito("El nuevo Retardo es", configDeMemoria.retardo);
}

//TODO falta hacer este metodo (sigue la misma logica que el dump del contenido de memoria)
void dumpCache() {
	mostrarCache();
};


void dumpEstructuras() {
	mostrarTablaDePaginas();
	texto_en_color("\nA conticuacion se mostrara el estado de los procesos activos: \n\n");
	uint32_t resultadoDelListado = listar_DataDeTodosLosProcesosActivos();
	if(resultadoDelListado==-1)
	{
		texto_en_color_de_error("No existen procesos activos de momento \n\n");
		return;
	}
}


void impresionDeLecturaDeDataDePaginas(int i,uint32_t paginas,uint32_t pid)
{
	int aciertos=0;
	char* data;
	char* data_string;
	while(i<paginas)
		{
			data=leerMemoria(pid,i,0,configDeMemoria.tamMarco);
			if(data)
			{
				data_string=malloc(configDeMemoria.tamMarco+1);
				memset(data_string,'\0',configDeMemoria.tamMarco+1);
				memcpy(data_string,data,configDeMemoria.tamMarco);
				free(data);
				pthread_mutex_lock(&mutex_test);
				printf("%s El PID: %d posee en memoria: %s %s \n",KGRN,pid,data,KNRM);
				pthread_mutex_unlock(&mutex_test);
				aciertos++;
				free(data_string);
			}
			i++;
		}
		if(aciertos==0)texto_en_color_de_error("El proceso no posee ninguna pagina");
}


void contenidoDeTodosLosProcesos()
{
	int i=0,aciertos=0;
	uint32_t pid,paginas;
	while(i<maxPA)
	{
		pthread_mutex_lock(&mutex_procesosActivos);
		pid = procesosActivos[i].pid;
		pthread_mutex_unlock(&mutex_procesosActivos);
		if(pid!=-2)
		{
			paginas=obtener_paginasActualesDeProcesoActivo(pid);
			impresionDeLecturaDeDataDePaginas(i,paginas,pid);
			aciertos++;
		}
		i++;
	}
	if(aciertos==0)texto_en_color_de_error("No se registra algun proceso en la memoria \n");
}


void contenidoPorPID()
{
	int pid,i=0;
	uint32_t paginas;
	texto_en_color("ingrese PID:");
	scanf("%d",&pid);
	mostrarProcesoEnMemoria(pid);

}

void dumpContenido() { //BIEEEEN RANCIO
	char opcion[1];
	do{

		texto_en_color("indique la accion que desea realizar: \n");
		texto_en_color("1- Informar los datos almacenados en la memoria de todos los procesos ");
		texto_en_color("2- Informar los datos almacenados en la memoria de un proceso en particular");
		texto_en_color("3- Ir al menu principal \n");
		texto_en_color("4- test\n");

		scanf("%s",opcion);
		switch(opcion[0])
			{
				//case '1': contenidoDeTodosLosProcesos(); break;
				case '1': mostrarMemoria(); break;
				case '2': contenidoPorPID(); break;
				case '3': break;
				default : break;
			}
		}while(opcion[0]!='3');
	/*
	printf("La memoria contiene %s \n\n", (char*) memoria);
	*/
}

void dump()
{
	int opcion; //int PID;
	texto_en_color(	"Reportar estado actual de:\n"
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


void flush()
{
	//todo mutexear cache!!!
	int entradasCache=configDeMemoria.entradasCache;
	tablaCache_t* tablaCache = (tablaCache_t*)cache;
	uint32_t marcos_EstructurasAdministrativas = cuantosMarcosRepresenta(tamanioDeTablaCache());
	int i=marcos_EstructurasAdministrativas;
	pthread_mutex_lock(&mutex_tablaCache);
	texto_en_color("Limpiando la memoria cache........ \n\n");
	while(i<entradasCache)
	{
		//printf("el pid de la cache es: %d \n",tablaCache[i].pid);
		tablaCache[i].pid=-2;
		tablaCache[i].pagina=i;
		i++;
	}
	texto_en_color("Limpieza de cache terminada\n\n");
	pthread_mutex_unlock(&mutex_tablaCache);
}

void sizeMemory()
{
	int framesTotales = configDeMemoria.marcos;
	pthread_mutex_lock(&mutex_marcos);
	int framesDisponibles = configDeMemoria.marcosDisponibles;
	pthread_mutex_unlock(&mutex_marcos);
	int framesOcupados = framesTotales-framesDisponibles;
	texto_en_color_de_exito("Los frames totales en la memoria son",framesTotales);
	texto_en_color_de_exito("Los frames disponibles en la memoria son",framesDisponibles);
	texto_en_color_de_exito("Los frames ocupados en la memoria son",framesOcupados);
}


void sizePID()
{
	int resultado;
	int pid;
	texto_en_color("ingrese PID");
	scanf("%d",&pid);
	resultado=listar_DataDeProcesoActivo(pid);
	if(resultado==-1)texto_en_color_de_error("No existe un proceso con el PID ingresado \n");
}


void size()
{
	char opcion[1];
	do{

		texto_en_color("indique la accion que desea realizar: \n");
		texto_en_color("1- Informar el tamaño de la memoria");
		texto_en_color("2- Informar el tamaño de un proceso en particular");
		texto_en_color("3- Ir al menu principal \n");

		scanf("%s",opcion);
		switch(opcion[0])
				{
				case '1': sizeMemory(); break;
				case '2': sizePID(); break;
				case '3': break;
				default : break;
				}
	}while(opcion[0]!='3');
}


void cantRunAway(){
	system("clear");
	printf("	Saliendo de memoria...\n");
	sleep(4);
	printf("\n\n	TE LA CREISTE WE XDXD \n\n\n");
	sleep(2);
	printf("\n\n	Nah, enserio, no podes irte!\n");
}

void mostrarMenuMemoria() {
	char opcion[1];
	do
	{

		texto_en_color(	"Ingrese un comando:\n\n"
				"Modificar el retardo: Presione 1\n"
				"Reportar estado actual: Presione 2\n"
				"Limpiar el contenido de la cache: Presione 3\n"
				"Indicar  tipos de tamaños en cuanto a la memoria y algun proceso: Presione 4\n"
				"Para limpiar la pantalla: Presione 5\n");
		scanf("%s", opcion);
		switch(opcion[0])
		{
		case '1': modificarRetardo(); break;
		case '2': dump(); break;
		case '3': flush(); break;
		case '4': size(); break;
		case '5': system("clear"); break;
		//case '7': cantRunAway(); break;
		default:break;
		}
	}while(1);
}
