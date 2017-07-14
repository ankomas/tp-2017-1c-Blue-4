/*
 * memoria.h
 *
 *  Created on: 27/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

#include <commons/collections/list.h>

pthread_t mutex_tabla, mutex_memoria;
pthread_mutex_t mutex_operacion;
int32_t contador;

typedef struct {
	uint32_t pid;
	uint32_t pagina;
}tablaPaginas_t;

typedef struct {
	uint32_t pid;
	uint32_t pagina;
	int32_t counter;
}tablaCache_t;

typedef struct{
	uint32_t pid;
	uint32_t paginas;
	t_list* listaPaginas;
	uint32_t paginaDeInicio;
	uint32_t proximaPaginaAAsignar;
}procesoActivo_t;

typedef struct{
	uint32_t pid;
	uint32_t* paginas;
}procesoActivoCache_t;

procesoActivoCache_t procesosActivosCache;
procesoActivo_t* procesosActivos; //Array de procesos Activos

uint32_t maxPA; //Maxima cantidad de procesos Activos

void mostrarMenuMemoria();

int servidor();

/*Las siguientes FUnciones definidas estan comentadas porque faltan definir tipos.-

Funciones segun Enunciado TP

solicitarMemoriaAlIniciar();

crearEstructurasAdministrativas();

validarPedidoRecibido();

simularAccesoMemoria();

responderSolicitudDeMemoria();

int funcionHashing(int pid, int numeroPagina);

guardarArchivo();

//MEMORIA CACHE

eliminarPaginas();

//OPERACIONES DE MEMORIA

inicializarPrograma(int idPrograma, int pagRequeridas);

solicitarBytesDePagina(int idPrograma, int numPagina, int *offset, size_t tamanio);

almacenarBytesEnPagina(int idPrograma, int numPagina, int *offset, size_t tamanio, char* buffer);

asignarPaginaAProceso(int idPrograma, int pagRequeridas);

finalizarPrograma(int idPrograma);*/

#endif /* MEMORIA_H_ */

