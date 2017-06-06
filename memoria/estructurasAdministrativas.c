/*
 * estructurasAdministrativas.c
 *
 *  Created on: 16/5/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "blue4-lib.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include <pthread.h>
#include "configuraciones.h"
#include "operacionesMemoria.h"
#include "estructurasAdministrativas.h"

int cuantosMarcosRepresenta(int unTamanio){
	int tamMarco=configDeMemoria.tamMarco;
	if(unTamanio % tamMarco)
		return unTamanio/tamMarco + 1;
	else
		return unTamanio/tamMarco;
}

void asignarTamanioAMemoria(){
	memoria = calloc(configDeMemoria.marcos,configDeMemoria.tamMarco);
}

void inicializarTablaDePaginas(tablaPaginas_t* tablaDePaginas){
	uint32_t marco;
	for(marco = 0; marco < configDeMemoria.marcos ; marco ++){
		tablaDePaginas[marco].pid = -2;
		tablaDePaginas[marco].pagina = marco;
	}
	return;
}

tablaPaginas_t* crearTablaDePaginas(){
	tablaPaginas_t *tablaDePaginas = malloc(tamanioDeTabla());
	inicializarTablaDePaginas(tablaDePaginas);
	return tablaDePaginas;
}

int actualizarMarcosDisponibles(int marcosAUsar)
{
	if((configDeMemoria.marcosDisponibles-marcosAUsar)>0)
	{
		printf("marcos antes de actualizar : %d \n",configDeMemoria.marcosDisponibles);
		configDeMemoria.marcosDisponibles-=marcosAUsar;
		printf("marcos actualizados : %d \n",configDeMemoria.marcosDisponibles);
		return 0;
	}
	return -1;
}

void reservarEstructurasEnTablaDePaginas(tablaPaginas_t* tablaDePaginas){
	uint32_t cantidadMarcos = cuantosMarcosRepresenta(tamanioDeTabla());
	uint32_t marco;
	actualizarMarcosDisponibles(cantidadMarcos);
	for(marco = 0; marco < cantidadMarcos ; marco++){
		tablaDePaginas[marco].pid = -1;
        // Como son los primeros marcos coinciden con el numero de pagina, solo pasa aca!
        tablaDePaginas[marco].pagina = marco;
        //printf("tablaDePaginas[marco].pid : %d \n",tablaDePaginas[marco].pid);
        //printf("tablaDePaginas[marco].pagina : %d \n",tablaDePaginas[marco].pagina);
	}
}

void cargarTablaDePaginasAMemoria(){
	tablaPaginas_t* tablaDePaginas = crearTablaDePaginas();
	reservarEstructurasEnTablaDePaginas(tablaDePaginas);
	memcpy(memoria,tablaDePaginas,tamanioDeTabla());
    free(tablaDePaginas);
}

void inicializarMemoria(){
	asignarTamanioAMemoria();
	cargarTablaDePaginasAMemoria();
}

void asignarTamanioACache(){
	cache = calloc(configDeMemoria.entradasCache, configDeMemoria.tamMarco);
}
void inicializarTablaCache(tablaCache_t * tabla){
	uint32_t marco;
	contador = 0;
	for(marco = 0; marco < configDeMemoria.entradasCache; marco++){
		tabla[marco].pid = -2;
		tabla[marco].pagina = marco;
		tabla[marco].counter = contador;
	}
}

tablaCache_t * crearTablaCache(){
	tablaCache_t *tabla = malloc(tamanioDeTablaCache());
	inicializarTablaCache(tabla);
	return tabla;
}

int actualizarCacheDisponible(uint32_t marcosAUsar){
	if((configDeMemoria.cacheDisponible-marcosAUsar)>0)
		{
			printf("marcos antes de actualizar : %d \n",configDeMemoria.cacheDisponible);
			configDeMemoria.cacheDisponible-=marcosAUsar;
			printf("marcos actualizados : %d \n",configDeMemoria.cacheDisponible);
			return 0;
		}
		return -1;
}

void reservarEstructurasEnTablaCache(tablaCache_t *tabla){
	uint32_t cantidadMarcos = cuantosMarcosRepresenta(tamanioDeTablaCache());
	uint32_t marco;
	actualizarCacheDisponible(cantidadMarcos);
	for(marco = 0; marco < cantidadMarcos ; marco++){
			tabla[marco].pid = -1;
	        tabla[marco].pagina = marco;
	        tabla[marco].counter = -1;
	}
}

void cargarTablaDeCache(){
	tablaCache_t* tabla = crearTablaCache();
	reservarEstructurasEnTablaCache(tabla);
	memcpy(cache,tabla,tamanioDeTablaCache());
	free(tabla);
}

void inicializarCache(){
	asignarTamanioACache();
	cargarTablaDeCache();
}
