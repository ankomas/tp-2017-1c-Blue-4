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
#define int32_min -999999
#define int32_max 999999


void texto_en_color(char * aString){
	pthread_mutex_lock(&mutex_test);
	printf("%s  %s %s \n",KCYN,aString,KNRM);
	pthread_mutex_unlock(&mutex_test);
}

void texto_en_color_de_error(char * aString){
	pthread_mutex_lock(&mutex_test);
	printf("%s [ERROR]: %s %s \n",KRED,aString,KNRM);
	pthread_mutex_unlock(&mutex_test);
}


void texto_en_color_de_exito(char * aString,int variable){
	pthread_mutex_lock(&mutex_test);
	printf("%s [EXITO] %s: %d %s \n",KGRN,aString,variable,KNRM);
	pthread_mutex_unlock(&mutex_test);
}


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
	texto_en_color_de_exito("marcos a usar ",marcosAUsar);
	if((configDeMemoria.marcosDisponibles-marcosAUsar)>=0)
	{
		configDeMemoria.marcosDisponibles-=marcosAUsar;
		texto_en_color_de_exito(" marcos disponibles ",configDeMemoria.marcosDisponibles);
		return 0;
	}
	texto_en_color_de_error("NO PUEDO ASIGAR MAS MARCOS!!!");
	return -1;
}

void aumentarMarcosDisponibles(int marcosAAumentar)
{
	configDeMemoria.marcosDisponibles+=marcosAAumentar;
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


void inicializarProcesosActivos()
{
	int i=0;
	procesosActivos = calloc(maxPA, sizeof(procesoActivo_t));
	while(i<maxPA)
	{
		procesosActivos[i].pid=-2;
		procesosActivos[i].paginas=-2;
		procesosActivos[i].paginaDeInicio=-2;
		procesosActivos[i].paginasMaximas=-2;
		i++;
	}
}


void inicializarMemoria(){
	asignarTamanioAMemoria();
	cargarTablaDePaginasAMemoria();
	maxPA = configDeMemoria.marcos - cuantosMarcosRepresenta(tamanioDeTabla());
	inicializarProcesosActivos();
}

void asignarTamanioACache(){
	cache = calloc(configDeMemoria.entradasCache, configDeMemoria.tamMarco);
}
void inicializarTablaCache(tablaCache_t * tabla){
	uint32_t marco;
	contador = int32_min;
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
			configDeMemoria.cacheDisponible-=marcosAUsar;
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

uint32_t obtener_PosicionLibre()
{
	uint32_t i=0;
	while(i<maxPA)
	{
		pthread_mutex_lock(&mutex_procesosActivos);
		if(procesosActivos[i].pid==-2){
			pthread_mutex_unlock(&mutex_procesosActivos);
			return i;
		}
		pthread_mutex_unlock(&mutex_procesosActivos);
		i++;
	}
	//printf("No hay mas espacio para procesos activos!!!\n");
	return -1;
}

uint32_t obtener_PosicionProcesoActivo(uint32_t pid)
{
	uint32_t i=0;
	while(i<maxPA)
	{
		pthread_mutex_lock(&mutex_procesosActivos);
		if(procesosActivos[i].pid==pid)
		{
			pthread_mutex_unlock(&mutex_procesosActivos);
			return i;
		}
		pthread_mutex_unlock(&mutex_procesosActivos);
		i++;
	}
	//printf("No se encuentra al proceso buscado");
	return -1;
}


void actualizar_PaginaDeInicioDeProcesoActivo(uint32_t posicionProceso,uint32_t valor)
{
	procesosActivos[posicionProceso].paginaDeInicio+=valor;
}

uint32_t obtener_PaginaDeInicioDeProcesoActivo(uint32_t pid)
{
	uint32_t paginaDeInicio;
	uint32_t posicion_PidBuscado=obtener_PosicionProcesoActivo(pid);
	if(posicion_PidBuscado==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	paginaDeInicio=procesosActivos[posicion_PidBuscado].paginaDeInicio;
	pthread_mutex_unlock(&mutex_procesosActivos);
	return paginaDeInicio;
}

uint32_t obtener_PaginasMaximas(uint32_t pid)
{
	uint32_t proximaPagina=0;
	uint32_t *aux;
	int i=0;
	uint32_t posicion_PidBuscado=obtener_PosicionProcesoActivo(pid);
	if(posicion_PidBuscado==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);

	proximaPagina=procesosActivos[posicion_PidBuscado].paginasMaximas;
	/*bool _byPag(uint32_t* pag){
		return proximaPagina==*pag;
	}

	while(i<list_size(procesosActivos[posicion_PidBuscado].listaPaginas)){
		aux=list_find(procesosActivos[posicion_PidBuscado].listaPaginas,(void*)_byPag);
		if(aux==NULL)
			break;
		proximaPagina++;
	}*/
	//uint32_t *nuevaPos=malloc(sizeof(uint32_t));
	//memcpy(nuevaPos,&proximaPagina,sizeof(uint32_t));
	//printf("  		Pag agregada: %i\n",*nuevaPos);
	//list_add(procesosActivos[posicion_PidBuscado].listaPaginas,nuevaPos);
	pthread_mutex_unlock(&mutex_procesosActivos);
	return proximaPagina;
}


int32_t agregar_DataDeProcesoActivo(uint32_t pid,uint32_t paginasActuales)
{
	uint32_t posicionLibre;
	posicionLibre=obtener_PosicionLibre();
	if(posicionLibre==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	procesosActivos[posicionLibre].pid=pid;
	procesosActivos[posicionLibre].paginas=paginasActuales;
	procesosActivos[posicionLibre].paginaDeInicio=0;
	procesosActivos[posicionLibre].paginasMaximas=paginasActuales;
	//procesosActivos[posicionLibre].listaPaginas=list_create();
	pthread_mutex_unlock(&mutex_procesosActivos);
	return posicionLibre;
}


uint32_t eliminar_DataDeProcesoActivo(uint32_t pid)
{
	uint32_t posicion_PidBuscado=obtener_PosicionProcesoActivo(pid);
	if(posicion_PidBuscado==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	procesosActivos[posicion_PidBuscado].pid=-2;
	procesosActivos[posicion_PidBuscado].paginas=-2;
	procesosActivos[posicion_PidBuscado].paginaDeInicio=-2;
	procesosActivos[posicion_PidBuscado].paginasMaximas=-2;
	//void _destroy(uint32_t* self){
	//	free(self);
	//}
	//list_destroy_and_destroy_elements(procesosActivos[posicion_PidBuscado].listaPaginas,(void*)_destroy);
	pthread_mutex_unlock(&mutex_procesosActivos);
	return 0;
}


uint32_t aumentar_PaginasActualesDeProcesoActivo(uint32_t pid,uint32_t paginas)
{
	uint32_t posicion_PidBuscado=obtener_PosicionProcesoActivo(pid);
	if(posicion_PidBuscado==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	procesosActivos[posicion_PidBuscado].paginas+=paginas;
	procesosActivos[posicion_PidBuscado].paginasMaximas+=paginas;
	pthread_mutex_unlock(&mutex_procesosActivos);
	return 0;
}


uint32_t disminuir_PaginasActualesDeProcesoActivo(uint32_t pid,uint32_t pagina)
{
	uint32_t paginaDeInicio;
	uint32_t posicion_PidBuscado=obtener_PosicionProcesoActivo(pid);
	if(posicion_PidBuscado==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	procesosActivos[posicion_PidBuscado].paginas-=1;

	/*bool _byPag(uint32_t* pag){
		return pagina==*pag;
	}
	void _destroy(uint32_t *elem){
		//printf("Borrar: %i\n",*elem);
		free(elem);
	}
	list_remove_and_destroy_by_condition(procesosActivos[posicion_PidBuscado].listaPaginas,(void*)_byPag,(void*)_destroy);
*/
	paginaDeInicio=procesosActivos[posicion_PidBuscado].paginaDeInicio;
	if(pagina==paginaDeInicio)
		actualizar_PaginaDeInicioDeProcesoActivo(posicion_PidBuscado,1);
	pthread_mutex_unlock(&mutex_procesosActivos);
	return 0;
}

uint32_t obtener_paginasActualesDeProcesoActivo(uint32_t pid)
{
	uint32_t paginas;
	uint32_t posicion_PidBuscado=obtener_PosicionProcesoActivo(pid);
	if(posicion_PidBuscado==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	paginas = procesosActivos[posicion_PidBuscado].paginas;
	pthread_mutex_unlock(&mutex_procesosActivos);
	return paginas;
}

uint32_t listar_DataDeTodosLosProcesosActivos()
{
	uint32_t i=0,numeroProcesos=0;
	while(i<maxPA)
	{
		pthread_mutex_lock(&mutex_procesosActivos);
		if(procesosActivos[i].pid != -2)
		{
			printf("Pid_procesoActivo : %d \n",procesosActivos[i].pid);
			printf("PaginasActuales_procesoActivo : %d \n\n",procesosActivos[i].paginas);
			numeroProcesos++;
		}
		pthread_mutex_unlock(&mutex_procesosActivos);
		i++;
	}
	if(numeroProcesos)return 0;
	return -1;
}


uint32_t listar_DataDeProcesoActivo(uint32_t pid)
{
	uint32_t posicionProceso=obtener_PosicionProcesoActivo(pid);
	if(posicionProceso==-1)return -1;
	pthread_mutex_lock(&mutex_procesosActivos);
	printf("Pid de proceso : %d \n",procesosActivos[posicionProceso].pid);
	printf("El proceso ocupa  %d frames \n\n",procesosActivos[posicionProceso].paginas);
	pthread_mutex_unlock(&mutex_procesosActivos);
	return 0;
}
