#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "conexiones.h"


int main(void) {

	int puerto;
	char* puntoMontaje;

	t_config* config;
/*
	config=config_create("config.cfg");

	printf("La información de mi Archivo de Configuración es:\n");

	puerto=config_get_int_value(config,"PUERTO");

	printf("PUERTO: %i,\n",puerto);
*/
	puntoMontaje=config_get_string_value(config,"PUNTO_MONTAJE");

	printf("PUNTO_MONTAJE: %s\n\n",puntoMontaje);

	config_destroy(config);

	servidor();

	return 0;

}
