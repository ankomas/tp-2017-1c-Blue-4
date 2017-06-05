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

int main(void) {
	pthread_mutex_init(&escribiendoMemoria,NULL);
	inicializarDataConfig();
	inicializarMemoria();
	inicializarPrograma(100,1,"HOLA SOY UN CODE ANSISOP");
	pthread_t hiloMostrarMenu;
	pthread_create(&hiloMostrarMenu, NULL, (void *) mostrarMenuMemoria, NULL);
	servidor(puerto);
	pthread_join(hiloMostrarMenu, NULL);
	free(memoria);
	return 0;

}
