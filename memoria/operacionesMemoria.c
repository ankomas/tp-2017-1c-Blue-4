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

int tamanioDeTabla(){
	return sizeof(tablaPaginas_t)*configDeMemoria.marcos;
}

int tamanioDeTablaCache(){
	return sizeof(tablaCache_t)*configDeMemoria.entradasCache;
}

 int marcosDeTabla(){
	 return cuantosMarcosRepresenta(tamanioDeTabla());
 }

 int marcosDeTablaCache(){
	 return cuantosMarcosRepresenta(tamanioDeTablaCache());
 }

 tablaPaginas_t* obtenerTablaDePaginas(){
	 tablaPaginas_t* tablaDePaginas = memoria;
	 return tablaDePaginas;
 }

 tablaCache_t* obtenerTablaCache(){
	 tablaCache_t* tabla = cache;
	 return tabla;
 }

 int proximoCache(){
	 tablaCache_t* tabla = obtenerTablaCache();
	 int marco, minV=0, minM=-1;
	 for(marco = 0; marco < configDeMemoria.entradasCache; marco++){
		 if(tabla[marco].counter != -1 && tabla[marco].counter < minV){
			 minV = tabla[marco].counter;
			 minM = marco;
		 }
	 }
	 if(minM > 0 && minM < configDeMemoria.entradasCache)
		 return minM;
	 else
		 return -1;
 }

 int getMarcoCache(int pid, int pag) {
	tablaCache_t *tabla = obtenerTablaCache();
	int i;
	if (pid < -1 || pag < 0)
		return -1;
	for (i = 0; i < configDeMemoria.marcos; i++) {
		if (tabla[i].pid == pid) {
			if (tabla[i].pagina == pag) {
				return i;
			}
		}
	}
	return -1;
}

 int getMarco(int pid, int pag){
	 tablaPaginas_t *tdep = obtenerTablaDePaginas();
	 int i;
	 if(pid<-1 || pag<0)
		 return -1;
	 for(i=0; i<configDeMemoria.marcos; i++){
		 if(tdep[i].pid == pid){
			 if(tdep[i].pagina == pag){
				 return i;}}
	 }
	 return -1;
 }

void cargarCodigo(uint32_t marco, uint32_t pagina, void* data){
	//pthread_mutex_lock(mutexMemoria);
	memcpy(memoria+marco*configDeMemoria.tamMarco,
		   data+pagina*configDeMemoria.tamMarco,
			configDeMemoria.tamMarco);
	printf("cargue en memoria : %s \n",data);
	//pthread_mutex_unlock(mutexMemoria);
}



void cargarPaginaA(tablaPaginas_t *tablaDePaginas,uint32_t pid, uint32_t pagina, unsigned marco){
	tablaDePaginas[marco].pid= pid;
	printf("cargue el pid : %d \n",pid);
	tablaDePaginas[marco].pagina = pagina;
	printf("cargue la pagina : %d \n",pagina);
	return;
}
void guardaProcesoEn(tablaPaginas_t *tablaDePaginas, uint32_t pid, uint32_t paginasRequeridas,void* data){
	unsigned marco = 0,pagina = 0;
	while(pagina < paginasRequeridas){
		if(tablaDePaginas[marco].pid == -2){
			cargarPaginaA(tablaDePaginas,pid,pagina,marco);
			cargarCodigo(marco,pagina,data);
			pagina++;
		}
		marco++;
	}
}
void agregarNuevoProceso(uint32_t pid, uint32_t paginasRequeridas,void* data){
	tablaPaginas_t *tablaDePaginas = obtenerTablaDePaginas();
	guardaProcesoEn(tablaDePaginas,pid,paginasRequeridas,data);
	actualizarMarcosDisponibles(paginasRequeridas);
}

int tieneMarcosSuficientes(int paginasRequeridas){
	return paginasRequeridas <= configDeMemoria.marcosDisponibles;
}

void inicializarPrograma(uint32_t pid,uint32_t paginasRequeridas, void* data){


		//pthread_mutex_lock(mutexMemoria);
		printf("tengo marcos suficientes \n");
		agregarNuevoProceso(pid,paginasRequeridas,data);
		//pthread_mutex_unlock(mutexMemoria);
}

void* leerMemoria(uint32_t pid,uint32_t pag, uint32_t offset, uint32_t tam){
	int marco = getMarco(pid, pag);
	void* datos = malloc(tam);
	memcpy(datos,memoria+marco*configDeMemoria.tamMarco+offset, tam);
	return datos;
}

void escribirMemoria(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tamData, void *data){
	int marco = getMarco(pid, pag);
	pthread_mutex_lock(&escribiendoMemoria);
	memcpy(memoria+marco*configDeMemoria.tamMarco+offset,data, tamData);
	pthread_mutex_unlock(&escribiendoMemoria);
}

void* leerCache(uint32_t pid,uint32_t pag, uint32_t offset, uint32_t tam){
	uint32_t marco = getMarcoCache(pid, pag);
	void* datos = malloc(tam);
	memcpy(datos,cache+marco*configDeMemoria.tamMarco+offset, tam);
	return datos;
}

void escribirCache(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tamData, void *data){
	int marco = getMarcoCache(pid, pag);
	pthread_mutex_lock(&escribiendoMemoriaCache);
	memcpy(cache+marco*configDeMemoria.tamMarco+offset,data, tamData);
	pthread_mutex_unlock(&escribiendoMemoriaCache);
}

/*int buscarB(int pid, int pag) {
	tablaPaginas_t *tabla = obtenerTablaDePaginas();
	int inf = 0;
	int sup = configDeMemoria.marcos;
	int marco;
	while (inf <= sup) {
		marco = (inf + sup) / 2;
		while (tabla[marco].pid == pid) {
			if (tabla[marco].pagina == pag)
				return marco;
			if (tabla[marco].pagina > pag)
				marco--;
			if (tabla[marco].pagina < pag)
				marco++;
		}
		if (tabla[marco].pid > pid)
			sup = marco;
		if (tabla[marco].pid < pid || tabla[marco].pid == -2)
			inf = marco;

	}
	return -1;
}*/
