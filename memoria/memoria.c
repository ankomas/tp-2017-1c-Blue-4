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
#include "memoria.h"

char* puerto;
int marcos, tamMarco, entradasCache, cachePorProceso, retardo;
//esta es mi memoria (Si, Delegue!)

int main(void) {

	configurarTodo();
	void* memoria = malloc(8192); //Se crea un gran frame
	pthread_t hiloMostrarMenu;
	pthread_create(&hiloMostrarMenu, NULL, (void *) mostrarMenuMemoria, NULL);
	servidor(puerto);
	pthread_join(hiloMostrarMenu, NULL);
	free(memoria);

	return 0;

}
