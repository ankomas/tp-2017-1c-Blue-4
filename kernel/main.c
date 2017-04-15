/*
 * main.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "main.h"

const char CONSOLA_ID = '1';
const char KERNEL_ID = '2';
const char CPU_ID = '3';
const char FS_ID = '4';
const char UMC_ID = '5';

int idFS;
int idUMC;

t_list * CONSOLAs;
t_list * CPUs;
t_queue * procesosNEW;
t_queue * procesosREADY;
t_queue * procesosEXEC;
t_queue * procesosBLOCK;
t_queue * procesosEXIT;

t_log* logger;

void inicializarDatos(){
	logger = log_create("logger.log",rutaAbsolutaDe("Debug/kernel"),true,LOG_LEVEL_TRACE);
	log_trace(logger,"Iniciando Kernel...");


	CONSOLAs = list_create();
	CPUs = list_create();
	idFS = conectar("8084", "127.0.0.1",FS_ID);
	idUMC = conectar("8085", "127.0.0.1",UMC_ID);
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
