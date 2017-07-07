/*
 * main.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include <pthread.h>

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

int cantidadProgramasEnSistema,gradoMultiprogramacion,retardo,quantum;
int idFS;
int idUMC;
int tamanioPagina;
int tamanioStack;

pthread_t hiloPlanificador,hiloConsolaKernel;

t_list * PROGRAMAs;
t_list * CPUs;
t_dictionary * semaforos;
t_dictionary * variablesCompartidas;
t_queue * procesosNEW;
t_queue * procesosREADY;
t_queue * procesosEXEC;
t_queue * procesosBLOCK;
t_queue * procesosEXIT;

t_log* logger;

void inicializarDatos(){
	logger = log_create("logger.log",rutaAbsolutaDe("Debug/kernel"),false,LOG_LEVEL_TRACE);
	log_trace(logger,"Iniciando Kernel...");

	pthread_mutex_init(&mutex_test,NULL);

	PROGRAMAs = list_create();
	CPUs = list_create();
	semaforos = dictionary_create();
	variablesCompartidas = dictionary_create();
	inicializarSemaforos();
	inicializarVariablesCompartidas();

	cantidadProgramasEnSistema = 0;
	gradoMultiprogramacion = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"),"GRADO_MULTIPROG");
	quantum = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"),"QUANTUM");
	tamanioStack = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"),"STACK_SIZE");
	idFS =  conectar(obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_FS"),  obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"IP_FS"),FS_ID_INT);
	idUMC = conectar(obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_UMC"), obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"IP_UMC"),UMC_ID_INT);
	test( obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_UMC") );
	tamanioPagina = obtenerTamanioPagina();
	retardo = obtenerConfiguracion(rutaAbsolutaDe("config.cfg"),"RETARDO");

	if(idFS <= 0){
		log_error(logger,"No se pudo conectar al FileSystem");
	} else {
		log_trace(logger,"Conexion establecida con el FileSystem");
	}

	if(idUMC <= 0){
		log_error(logger,"No se pudo conectar a la Memoria");
	} else {
		log_trace(logger,"Conexion establecida con la Memoria");
	}

	procesosNEW = queue_create();
	procesosREADY = queue_create();
	procesosEXEC = queue_create();
	procesosBLOCK = queue_create();
	procesosEXIT = queue_create();

	pthread_create(&hiloConsolaKernel, NULL, consolaKernel,NULL);
}

int main(){
	anuncio("\n-- Datos del sistema --");
	anuncio(concat(2,"IP a utilizar: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"IP")));
	anuncio(concat(2,"Puerto a utilizar: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO_PROG")));

	pagina* unaPagina = malloc(sizeof(pagina));
	iniciarPaginaHeap(unaPagina);

	inicializarDatos();

	anuncio(concat(2,"Tamanio de Pagina: ",string_itoa(tamanioPagina)));

	//testi(valorSemaforo("SEM3"));

	servidor();
	pthread_join(hiloConsolaKernel,0);


	return 0;
}
