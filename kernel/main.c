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

const char CONSOLA_ID_INT = 1;
const char KERNEL_ID_INT = 2;
const char CPU_ID_INT = 3;
const char FS_ID_INT = 4;
const char UMC_ID_INT = 5;

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

	idFS = conectar(obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_FS"), obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"IP_FS"),FS_ID_INT);
	idUMC = conectar(obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_UMC"), obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"IP_UMC"),UMC_ID_INT);

	if(idFS <= 0){
		log_error(logger,"No se pudo conectar al FileSystem");
	} else {
		log_trace(logger,"Conexion establecida con el FileSystem");
	}

	if(idUMC <= 0){
		log_error(logger,"No se pudo conectar a la UMC");
	} else {
		log_trace(logger,"Conexion establecida con la UMC");
	}

	procesosNEW = queue_create();
	procesosREADY = queue_create();
	procesosEXEC = queue_create();
	procesosBLOCK = queue_create();
	procesosEXIT = queue_create();
}

int main(){

	anuncio(concat(2,"IP a utilizar: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"IP")));
	anuncio(concat(2,"PUERTO a utilizar: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_PROG")));
	inicializarDatos();

	//testi(valorSemaforo("SEM3"));

	servidor();

	return 0;
}
