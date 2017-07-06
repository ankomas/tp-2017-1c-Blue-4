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

uint32_t tamanioDeTabla(){
	return sizeof(tablaPaginas_t)*configDeMemoria.marcos;
}

uint32_t tamanioDeTablaCache(){
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



 int lru(){
	 tablaCache_t* tabla = obtenerTablaCache();
	 int marco, minV=0, minM=-1;
	 for(marco = 0; marco < configDeMemoria.entradasCache; marco++){
			pthread_mutex_lock(&mutex_tablaCache);
		 if(tabla[marco].counter != -1 && tabla[marco].counter < minV){
			 minV = tabla[marco].counter;
			 minM = marco;
		 }
			pthread_mutex_unlock(&mutex_tablaCache);
	 }
	 if(minM > 0 && minM < configDeMemoria.entradasCache)
		 return minM;
	 else
		 return -1;
 }

 void actualizarCounterMarco(uint32_t marco){
	 tablaCache_t* tabla = obtenerTablaCache();
	pthread_mutex_lock(&mutex_tablaCache);
	tabla[marco].counter = contador++;
	pthread_mutex_unlock(&mutex_tablaCache);
 }

 void escribirCache(uint32_t marco,uint32_t offset,uint32_t tamData, void *data){
 	pthread_mutex_lock(&escribiendoMemoriaCache);
 	memcpy(cache+marco*configDeMemoria.tamMarco+offset,data, tamData);
 	actualizarCounterMarco(marco);
 	pthread_mutex_unlock(&escribiendoMemoriaCache);
 }

 int getMarcoCache(int pid, int pag) {
	tablaCache_t *tabla = obtenerTablaCache();
	int i;
	if (pid < -1 || pag < 0)
		return -1;
	for (i = 0; i < configDeMemoria.entradasCache; i++) {
		pthread_mutex_lock(&mutex_tablaCache);
		if (tabla[i].pid == pid) {
			if (tabla[i].pagina == pag) {
				pthread_mutex_unlock(&mutex_tablaCache);
				return i;
			}
		}
		pthread_mutex_unlock(&mutex_tablaCache);
	}
	return -1;
}

uint32_t estaEnCache(uint32_t pid, uint32_t pag){
	return getMarcoCache(pid, pag) >= -1;
}

 int marcosTabla(){
 	return cuantosMarcosRepresenta(tamanioDeTabla());
 }

 int tratarColision(int hash)
 {
 	if(hash<configDeMemoria.marcos)
   	return hash++;
   else
   	return marcosTabla();
 }


 int funcionDeHash(pid, pag){
 	return (pid*10+pag)%(maxPA) + marcosTabla();
 }

 int getMarco(uint32_t pid,uint32_t pagina)
 {
 	// el hash representa el indice en la tabla que seria igual al marco
 	int hash,i=0;
   tablaPaginas_t* tablaDePaginas= obtenerTablaDePaginas();
 	hash=funcionDeHash(pid,pagina);

	pthread_mutex_lock(&mutex_tablaDePaginas);
   while(tablaDePaginas[hash].pid!=pid || tablaDePaginas[hash].pagina!=pagina){
   hash = tratarColision(hash);
   i++;

   if(i>=maxPA){
		pthread_mutex_unlock(&mutex_tablaDePaginas);
   	return -1; }//NO EXISTE EL PID PAPURRI ?)

   }
	pthread_mutex_unlock(&mutex_tablaDePaginas);
   return hash;
 }

 int marcoVacio(int marco){
 	tablaPaginas_t *tablaDePaginas = obtenerTablaDePaginas();
 	int resultado;
	pthread_mutex_lock(&mutex_tablaDePaginas);
	resultado=tablaDePaginas[marco].pid== -2;
	pthread_mutex_unlock(&mutex_tablaDePaginas);
 	return resultado;
 }

int nuevoMarco(uint32_t pid, uint32_t pagina) {
	// el hash representa el indice en la tabla que seria igual al marco
	int hash, i = 0;
	hash = funcionDeHash(pid, pagina);
	while (!marcoVacio(hash)) {
		hash = tratarColision(hash);
		i++;

		if (i >= maxPA)
			return -1; //NO HAY MARCOS VACIOS ._.

	}
	return hash;
}

 /*
 int getMarco(int pid, int pag){
	 tablaPaginas_t *tdep = obtenerTablaDePaginas();
	 int i;
	 if(pid==-1 || pag<0)
		 return -1;
	 for(i=0; i<configDeMemoria.marcos; i++){
		 if(tdep[i].pid == pid){
			 if(tdep[i].pagina == pag){
				 return i;}}
	 }
	 return -1;
 }
 */

void cargarCodigo(uint32_t marco, uint32_t pagina, void* data){
	//pthread_mutex_lock(mutexMemoria);
	memcpy(memoria+marco*configDeMemoria.tamMarco,
		   data+pagina*configDeMemoria.tamMarco,
			configDeMemoria.tamMarco);
	printf("cargue en memoria : %s \n",(char*)data);
	//pthread_mutex_unlock(mutexMemoria);
}

void cargarPaginaATabla(uint32_t pid, uint32_t pagina, unsigned marco){
	tablaPaginas_t *tablaDePaginas = obtenerTablaDePaginas();
	pthread_mutex_lock(&mutex_tablaDePaginas);
	tablaDePaginas[marco].pid= pid;
	printf("cargue el pid : %d \n",pid);
	tablaDePaginas[marco].pagina = pagina;
	printf("cargue la pagina : %d \n",pagina);
	pthread_mutex_unlock(&mutex_tablaDePaginas);
	return;
}

void guardaProcesoEn(uint32_t pid, uint32_t paginasRequeridas) {
	uint32_t marco, pagina = 0;
	while (pagina < paginasRequeridas) {
		marco = nuevoMarco(pid, pagina);
		cargarPaginaATabla(pid, pagina, marco);
		pagina++;
	}
}

void agregarNuevoProceso(uint32_t pid, uint32_t paginasRequeridas){
	guardaProcesoEn(pid,paginasRequeridas);
	actualizarMarcosDisponibles(paginasRequeridas);
}

int tieneMarcosSuficientes(int paginasRequeridas){
	return paginasRequeridas <= configDeMemoria.marcosDisponibles;
}

void inicializarPrograma(uint32_t pid,uint32_t paginasRequeridas){
	//pthread_mutex_lock(mutexMemoria);
	agregar_DataDeProcesoActivo(pid,paginasRequeridas);
	printf("tengo marcos suficientes \n");
	agregarNuevoProceso(pid,paginasRequeridas);
	//pthread_mutex_unlock(mutexMemoria);
}


uint32_t finalizarPrograma(uint32_t pid)
{
	tablaPaginas_t* tablaDePaginas = obtenerTablaDePaginas();
	int pagina,paginasMaximas,i=0;
	uint32_t paginasTotales;
	pagina=obtener_PaginaDeInicioDeProcesoActivo(pid);
	int marco=getMarco(pid,pagina);
	if(marco<0)return -1;
	paginasMaximas=obtener_ProximaPaginaAAsignar(pid);
	while(i<paginasMaximas)
	{
		if(marco >=0)
		{
		pthread_mutex_lock(&mutex_tablaDePaginas);
		tablaDePaginas[marco].pid=-2;
		tablaDePaginas[marco].pagina=marco;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		}
		pagina++;
		i++;
		marco=getMarco(pid,pagina);
	}
	obtener_PosicionProcesoActivo(pid);
	eliminar_DataDeProcesoActivo(pid);
	return 0;
}


int asignarPaginasAUnProceso(uint32_t pid,uint32_t paginasRequeridas)
{
	int pagina,marco,i=0;
	tablaPaginas_t* tablaDePaginas;
	int resultado=tieneMarcosSuficientes(paginasRequeridas);
	if(resultado==0)return -1;
	tablaDePaginas=obtenerTablaDePaginas();
	actualizarMarcosDisponibles(paginasRequeridas);
	pagina=obtener_ProximaPaginaAAsignar(pid);
	while(i<paginasRequeridas)
	{
		marco=nuevoMarco(pid,pagina);
		pthread_mutex_lock(&mutex_tablaDePaginas);
		tablaDePaginas[marco].pid=pid;
		tablaDePaginas[marco].pagina=pagina;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		pagina++;
	}
	aumentar_PaginasActualesDeProcesoActivo(pid,paginasRequeridas);
	return 0;
}



int eliminarPaginaDeUnProceso(uint32_t pid,uint32_t paginaAEliminar)
{
	tablaPaginas_t* tablaDePaginas;
	tablaDePaginas=obtenerTablaDePaginas();
	int marco = getMarco(pid,paginaAEliminar);
	if(marco<0)return -1;
	pthread_mutex_lock(&mutex_tablaDePaginas);
	tablaDePaginas[marco].pid= -2;
	tablaDePaginas[marco].pagina= -2;
	pthread_mutex_unlock(&mutex_tablaDePaginas);
	disminuir_PaginasActualesDeProcesoActivo(pid,paginaAEliminar);
	return 0;
}



void* leerMemoria(uint32_t pid,uint32_t pag, uint32_t offset, uint32_t tam){ //REQUIERE FREE
	int marco = getMarco(pid, pag);
	if(marco<0)return NULL;
	void* datos = malloc(tam);
	pthread_mutex_lock(&escribiendoMemoria);
	memcpy(datos,memoria+marco*configDeMemoria.tamMarco+offset, tam);
	pthread_mutex_unlock(&escribiendoMemoria);
	return datos;
}

uint32_t escribirMemoria(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tamData, void *data){
	int marco = getMarco(pid, pag);
	if(marco<0)return -1;
	pthread_mutex_lock(&escribiendoMemoria);
	memcpy(memoria+marco*configDeMemoria.tamMarco+offset,data, tamData);
	pthread_mutex_unlock(&escribiendoMemoria);
	return 0;
}

void* leerCache(uint32_t pid,uint32_t pag, uint32_t offset, uint32_t tam){ //NECESITA UN FREE :D
	uint32_t marco = getMarcoCache(pid, pag);
	void* datos = malloc(tam);
	pthread_mutex_lock(&escribiendoMemoriaCache);
	memcpy(datos,cache+marco*configDeMemoria.tamMarco+offset, tam);
	pthread_mutex_unlock(&escribiendoMemoriaCache);
	actualizarCounterMarco(marco);
	return datos;
}

int agregarProcesoACache(int pid, int pag){
	 int marco = lru();
	 void* data = leerMemoria(pid, pag, 0, configDeMemoria.tamMarco);
	 escribirCache(marco, 0, configDeMemoria.tamMarco, data);
	 return marco;
}

void* leer(uint32_t pid,uint32_t pag, uint32_t offset, uint32_t tam){ // SIGUE NECESITANDO UN FREE
	if(estaEnCache(pid, pag))
		return leerCache(pid, pag, offset, tam);
	else
		return leerMemoria(pid, pag, offset, tam);
}

int escribir(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tamData, void *data){
	int e = escribirMemoria(pid,pag,offset,tamData,data);
	if(estaEnCache(pid,pag))
		escribirCache(getMarcoCache(pid,pag),offset,tamData,data);
	else
		agregarProcesoACache(pid, pag);
	return e;
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
