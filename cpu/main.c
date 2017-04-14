/*
 * main.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "conexiones.h"

int main(){
	int puertoKernel,puertoMemoria,socketKernel,socketMemoria;
	char *ipKernel,*ipMemoria;
	t_config* config;

	config=config_create("config.cfg");

	ipKernel=config_get_string_value(config,"IP_KERNEL");
	ipMemoria=config_get_string_value(config,"IP_MEMORIA");
	puertoKernel=config_get_int_value(config,"PUERTO_KERNEL");
	puertoMemoria=config_get_int_value(config,"PUERTO_MEMORIA");

	printf("IP KERNEL: %s\nPUERTO KERNEL: %i\nIP MEMORIA: %s\nPUERTO MEMORIA: %i\n",ipKernel,puertoKernel,ipMemoria,puertoMemoria);

	config_destroy(config);

	//socketKernel=conectar(puertoKernel,ipKernel);
	//socketMemoria=conectar(puertoMemoria,ipMemoria);

	return 0;
}

