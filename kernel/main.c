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

const char PROGRAMA_ID = '1';
const char KERNEL_ID = '2';
const char CPU_ID = '3';
const char FS_ID = '4';
const char UMC_ID = '5';

const char PROGRAMA_ID_INT = 1;
const char KERNEL_ID_INT = 2;
const char CPU_ID_INT = 3;
const char FS_ID_INT = 4;
const char UMC_ID_INT = 5;

int cantidadProgramasEnSistema,gradoMultiprogramacion,retardo,quantum;
int idFS;
int idUMC;
int tamanioPagina;
int detenerPlanificacion;
int retardoQuantum;
int32_t tamanioStack;

pthread_t hiloPlanificador,hiloConsolaKernel;

t_list * PROGRAMAs;
t_list * CPUs;
t_list * tablaGlobalArchivos;
t_dictionary * paginasHeap;
t_dictionary * semaforos;
t_dictionary * variablesCompartidas;
t_queue * procesosNEW;
t_queue * procesosREADY;
t_queue * procesosEXEC;
t_queue * procesosBLOCK;
t_queue * procesosEXIT;
t_config *cfg;
uint32_t GlobalFDCounter;

t_log* logger;
pthread_mutex_t mutex_programas;
pthread_mutex_t mutex_cpu;
pthread_mutex_t mutex_colasPlanificacion;
pthread_mutex_t mutex_semaforos;
pthread_mutex_t mutex_fs;
pthread_mutex_t mutex_varGlobales;

void inicializarDatos(){
	logger = log_create("logger.log",rutaAbsolutaDe("Debug/kernel"),true,LOG_LEVEL_TRACE);
	log_trace(logger,"Iniciando Kernel...");

	pthread_mutex_init(&mutex_programas,NULL);
	pthread_mutex_init(&mutex_cpu,NULL);
	pthread_mutex_init(&mutex_test,NULL);
	pthread_mutex_init(&mutex_colasPlanificacion,NULL);
	pthread_mutex_init(&mutex_semaforos,NULL);
	pthread_mutex_init(&mutex_fs,NULL);
	pthread_mutex_init(&mutex_varGlobales,NULL);
	cfg=config_create(rutaAbsolutaDe("config.cfg"));

	GlobalFDCounter = 0;
	detenerPlanificacion = 0;

	PROGRAMAs = list_create();
	CPUs = list_create();
	paginasHeap = dictionary_create();
	semaforos = dictionary_create();
	variablesCompartidas = dictionary_create();
	tablaGlobalArchivos = list_create();
	inicializarSemaforos();
	inicializarVariablesCompartidas();

	cantidadProgramasEnSistema = 0;
	gradoMultiprogramacion = obtenerConfiguracion(cfg,"GRADO_MULTIPROG");
	quantum = obtenerConfiguracion(cfg,"QUANTUM");
	tamanioStack = obtenerConfiguracion(cfg,"STACK_SIZE");
	idFS =  conectar(obtenerConfiguracionString(cfg,"PUERTO_FS"),  obtenerConfiguracionString(cfg,"IP_FS"),FS_ID_INT);
	idUMC = conectar(obtenerConfiguracionString(cfg,"PUERTO_UMC"), obtenerConfiguracionString(cfg,"IP_UMC"),UMC_ID_INT);
	tamanioPagina = obtenerTamanioPagina();
	if(tamanioPagina <= 0){
		anuncio("No se conecto a la memoria, abortando...");
	}
	if(idFS <= 0){
		anuncio("No se conecto al FileSystem, abortando...");
	}
	retardo = obtenerConfiguracion(cfg,"QUANTUM_SLEEP");

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

	if(tamanioPagina <= 0 || idFS <= 0 ){
		killme();
	}

	pthread_create(&hiloConsolaKernel, NULL, consolaKernel,NULL);
}

int main(){
	inicializarDatos();

	anuncio("\n-- Datos del sistema --");
	anuncio(concat(2,"IP a utilizar: ",obtenerConfiguracionString(cfg,"IP")));
	anuncio(concat(2,"Puerto a utilizar: ",obtenerConfiguracionString(cfg,"PUERTO_PROG")));

	//paginaHeap* unaPagina = malloc(sizeof(paginaHeap));
	//iniciarBloqueHeap(unaPagina);
	//return 0;

	anuncio(concat(2,"Tamanio de Pagina: ",string_itoa(tamanioPagina)));

	//testi(valorSemaforo("SEM3"));

	servidor();
	pthread_join(hiloConsolaKernel,0);


	return 0;
}
