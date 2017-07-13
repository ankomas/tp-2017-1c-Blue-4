/*
 * primitivasKernel.c
 *
 *  Created on: 13/7/2017
 *      Author: utnso
 */

#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "conexiones.h"
#include "primitivas.h"
#include "primitivasKernel.h"
#include "capaKernel.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

//Semaforos

void dummy_wait(t_nombre_semaforo identificador_semaforo){
	char res;
	printf("Llamada a "YEL"WAIT"RESET" %s\n",identificador_semaforo);

	res=semWait(identificador_semaforo);

	switch(res){
	case 'Y':
		return;
	case 'N':
		setExitCode(&pcb_global,"el semaforo no existe",15);
		return;
	case 'B':
		finPrograma_global='B';
		return;
	}
}

void dummy_signal(t_nombre_semaforo identificador_semaforo){
	char res;

	printf("Llamada a "YEL"SIGNAL"RESET" %s\n",identificador_semaforo);

	res=semSignal(identificador_semaforo);

	switch(res){
	case 'Y':
		return;
	case 'N':
		setExitCode(&pcb_global,"el semaforo no existe",15);
		return;
	}
}

void (*AnSISOP_wait)(t_nombre_semaforo identificador_semaforo);
void (*AnSISOP_signal)(t_nombre_semaforo identificador_semaforo);

//Heap

t_puntero (*AnSISOP_reservar)(t_valor_variable espacio);
void (*AnSISOP_liberar)(t_puntero puntero);

//Filesystem

t_descriptor_archivo (*AnSISOP_abrir)(t_direccion_archivo direccion, t_banderas flags);
void (*AnSISOP_borrar)(t_descriptor_archivo descriptor_archivo);
void (*AnSISOP_cerrar)(t_descriptor_archivo descriptor_archivo);
void (*AnSISOP_moverCursor)(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion);
void (*AnSISOP_escribir)(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio);
void (*AnSISOP_leer)(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio);

