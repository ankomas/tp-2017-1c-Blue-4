#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "sockets.h"

//esta es mi memoria (Si, Delegue!)

int main(void) {

	int puerto;

	t_config* config;

	config = config_create("config.cfg");

	printf("La información de mi Archivo de Configuración es:\n");

	puerto = config_get_int_value(config, "PUERTO");

	printf("PUERTO: %i,\n", puerto);

	config_destroy(config);

	servidor();

	return 0;

}
