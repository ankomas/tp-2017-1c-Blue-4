/*
 * main.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <parser/metadata_program.h>
#include "conexiones.h"

#define ID_KERNEL 2
#define ID_MEMORIA 5

int main() {
	testPCB();
	return 0;
	//int socketKernel,socketMemoria;
	char *ipKernel, *ipMemoria, *puertoKernel, *puertoMemoria;
	t_config* config;
	int socketKernel,socketMemoria;

	config = config_create("config.cfg");

	ipKernel = config_get_string_value(config, "IP_KERNEL");
	ipMemoria = config_get_string_value(config, "IP_MEMORIA");
	puertoKernel = config_get_string_value(config, "PUERTO_KERNEL");
	puertoMemoria = config_get_string_value(config, "PUERTO_MEMORIA");

	printf(
			"IP KERNEL: %s\nPUERTO KERNEL: %s\nIP MEMORIA: %s\nPUERTO MEMORIA: %s\n",
			ipKernel, puertoKernel, ipMemoria, puertoMemoria);

	printf("||||||||||||||||||||||||||||||\n");

	socketMemoria=conectar(puertoMemoria, ipMemoria,ID_MEMORIA);
	socketKernel=conectar(puertoKernel, ipKernel,ID_KERNEL);

	standby(socketKernel);

	cerrarConexion(socketKernel);
	config_destroy(config);
	return 0;
}

