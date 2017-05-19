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
#include "estructurasAdministrativas.h"


 tablaPaginas_t* obtenerTablaDePaginas(){ //Requiere hacer free
	 tablaPaginas_t* tablaDePaginas;
	 int marcosALeer = cuantosMarcosRepresenta(sizeof(tablaPaginas_t)*configDeMemoria.marcos);
	 tablaDePaginas = malloc(marcosALeer);
	 memcpy(tablaDePaginas, memoria, marcosALeer);
	 return tablaDePaginas;
 }

void cargarCodigo(uint32_t marco, uint32_t pagina, void* codigo){
	//pthread_mutex_lock(mutexMemoria);
	memcpy(memoria+marco*configDeMemoria.tamMarco,
		   codigo+pagina*configDeMemoria.tamMarco,
			configDeMemoria.tamMarco);
	printf("cargue en memoria : %s \n",codigo);
	//pthread_mutex_unlock(mutexMemoria);
}



void cargarPaginaA(tablaPaginas_t *tablaDePaginas,uint32_t pid, uint32_t pagina, unsigned marco){
	tablaDePaginas[marco].pid= pid;
	printf("cargue el pid : %d \n",pid);
	tablaDePaginas[marco].pagina = pagina;
	printf("cargue la pagina : %d \n",pagina);
	return;
}
void guardaProcesoEn(tablaPaginas_t *tablaDePaginas, uint32_t pid, uint32_t paginasRequeridas,void* codigo){
	unsigned marco = 0,pagina = 0;
	while(pagina < paginasRequeridas){
		if(tablaDePaginas[marco].pid == -2){
			cargarPaginaA(tablaDePaginas,pid,pagina,marco);
			cargarCodigo(marco,pagina,codigo);
			pagina++;
		}
		marco++;
	}
}
void agregarNuevoProceso(uint32_t pid, uint32_t paginasRequeridas,void* codigo){
	tablaPaginas_t *tablaDePaginas = obtenerTablaDePaginas();
	guardaProcesoEn(tablaDePaginas,pid,paginasRequeridas,codigo);
	actualizarMarcosDisponibles(paginasRequeridas);
	free(tablaDePaginas);
}

int tieneMarcosSuficientes(int paginasRequeridas){
	return paginasRequeridas <= configDeMemoria.marcosDisponibles;
}

void inicializarPrograma(uint32_t pid,uint32_t paginasRequeridas, void* codigo){


	if(tieneMarcosSuficientes(paginasRequeridas)){
		//pthread_mutex_lock(mutexMemoria);
		printf("tengo marcos suficientes \n");
		agregarNuevoProceso(pid,paginasRequeridas,codigo);
		//pthread_mutex_unlock(mutexMemoria);
	}
	else{
		// mandarle error al nucleo!! send(sos un boludo y no tenes espacio)
	}
}
