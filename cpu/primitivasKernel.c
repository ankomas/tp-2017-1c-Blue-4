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

//Semaforos

void dummy_wait(t_nombre_semaforo identificador_semaforo);

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

