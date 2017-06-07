#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <pthread.h>
#include "configuraciones.h"
#include "menuMemoria.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include "estructurasAdministrativas.h"

char* puerto;
int marcos, tamMarco, entradasCache, cachePorProceso, retardo;


void inicializarEstructurasAdministrativas()
{
	pthread_mutex_init(&escribiendoMemoria,NULL);
	pthread_mutex_init(&escribiendoMemoriaCache,NULL);
	inicializarDataConfig();
	inicializarMemoria();
	inicializarCache();
}


int main(void) {

	inicializarEstructurasAdministrativas();
	inicializarPrograma(100,1,"HOLA SOY UN CODE ANSISOP");
	printf("la wea dice: %s\n",leerMemoria(100,0,0,25));
	escribirMemoria(100,0,0,20,"tu vieja en tanga me gusta");
	printf("la wea dice: %s\n",leerMemoria(100,0,0,20));
	asignarPaginasAUnProceso(100,20);
	escribirMemoria(100,18,0,16,"salva es re mani");
	printf("la wea dice: %s\n",leerMemoria(100,18,0,16));
	pthread_t hiloMostrarMenu;
	pthread_create(&hiloMostrarMenu, NULL, (void *) mostrarMenuMemoria, NULL);
	servidor(puerto);
	pthread_join(hiloMostrarMenu, NULL);
	free(memoria);
	return 0;

}
