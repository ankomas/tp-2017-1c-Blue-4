/*
 * main.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "main.h"

t_queue * procesosNEW;
t_queue * procesosREADY;
t_queue * procesosEXEC;
t_queue * procesosBLOCK;
t_queue * procesosEXIT;

t_log* logger;

void inicializarDatos(){
	log_trace(logger,"Iniciando Kernel...");
	logger = log_create("logger.log",rutaAbsolutaDe("Debug/kernel"),true,LOG_LEVEL_TRACE);

	procesosNEW = queue_create();
	procesosREADY = queue_create();
	procesosEXEC = queue_create();
	procesosBLOCK = queue_create();
	procesosEXIT = queue_create();
}

int main(){

	inicializarDatos();

	//testi(valorSemaforo("SEM3"));

	servidor();

	return 0;
}
