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
#include <commons/string.h>
#include "blue4-lib.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include <pthread.h>
#include "configuraciones.h"
#include "estructurasAdministrativas.h"

uint32_t tamanioDeTabla() {
	return sizeof(tablaPaginas_t) * configDeMemoria.marcos;
}

uint32_t tamanioDeTablaCache() {
	return sizeof(tablaCache_t) * configDeMemoria.entradasCache;
}

int marcosDeTabla() {
	return cuantosMarcosRepresenta(tamanioDeTabla());
}

int marcosDeTablaCache() {
	return cuantosMarcosRepresenta(tamanioDeTablaCache());
}

tablaPaginas_t* obtenerTablaDePaginas() {
	tablaPaginas_t* tablaDePaginas = (tablaPaginas_t*)memoria;
	return tablaDePaginas;
}

tablaCache_t* obtenerTablaCache() {
	tablaCache_t* tabla = cache;
	return tabla;
}

int lru() { //ACA PASA ALGO MUY TURBIO!
	tablaCache_t* tabla = obtenerTablaCache();
	int marco, minV = 99999, minM = -2;
	for (marco = 1; marco < configDeMemoria.entradasCache; marco++) {
		pthread_mutex_lock(&mutex_tablaCache);
		if (tabla[marco].pid >= 0 && tabla[marco].counter < minV) {
			minV = tabla[marco].counter;
			minM = marco;
		}
		pthread_mutex_unlock(&mutex_tablaCache);
	}
	return minM;
}

void actualizarCounterMarco(uint32_t marco) {
	tablaCache_t* tabla = obtenerTablaCache();
	pthread_mutex_lock(&mutex_tablaCache);
	tabla[marco].counter = contador++;
	pthread_mutex_unlock(&mutex_tablaCache);
}

void escribirCache(uint32_t marco, uint32_t offset, uint32_t tamData,
		void *data) {
	pthread_mutex_lock(&escribiendoMemoriaCache);
	memcpy(cache + marco * configDeMemoria.tamMarco + offset, data, tamData);
	actualizarCounterMarco(marco);
	pthread_mutex_unlock(&escribiendoMemoriaCache);
}

int getMarcoCache(int pid, int pag) {
	tablaCache_t *tabla = obtenerTablaCache();
	int i;
	if (pid < -1 || pag < 0)
		return -2;
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
	return -2;
}

uint32_t estaEnCache(uint32_t pid, uint32_t pag) {
	return getMarcoCache(pid, pag) >= -1;
}

int marcosTabla() {
	return cuantosMarcosRepresenta(tamanioDeTabla());
}

int tratarColision(int hash) {
	if (hash < configDeMemoria.marcos)
		return hash++;
	else
		return marcosTabla();
}

int funcionDeHash( pid, pag) {
	return (pid * 10 + pag) % (maxPA) + marcosTabla();
}

int getMarco(uint32_t pid, uint32_t pagina) {
	// el hash representa el indice en la tabla que seria igual al marco
	int hash, i = 0;
	tablaPaginas_t* tablaDePaginas = obtenerTablaDePaginas();
	hash = funcionDeHash(pid, pagina);

	pthread_mutex_lock(&mutex_tablaDePaginas);
	while (tablaDePaginas[hash].pid != pid
			|| tablaDePaginas[hash].pagina != pagina) {
		hash = tratarColision(hash);
		i++;

		if (i >= maxPA) {
			pthread_mutex_unlock(&mutex_tablaDePaginas);
			return -1;
		} //NO EXISTE EL PID PAPURRI ?)

	}
	pthread_mutex_unlock(&mutex_tablaDePaginas);
	return hash;
}

int marcoVacio(int marco) {
	tablaPaginas_t *tablaDePaginas = obtenerTablaDePaginas();
	int resultado;
	pthread_mutex_lock(&mutex_tablaDePaginas);
	resultado = tablaDePaginas[marco].pid == -2;
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

void cargarCodigo(uint32_t marco, uint32_t pagina, void* data) {
	//pthread_mutex_lock(mutexMemoria);
	memcpy(memoria + marco * configDeMemoria.tamMarco,
			data + pagina * configDeMemoria.tamMarco, configDeMemoria.tamMarco);
	printf("cargue en memoria : %s \n", (char*) data);
	//pthread_mutex_unlock(mutexMemoria);
}

void cargarPaginaATabla(uint32_t pid, uint32_t pagina, unsigned marco) {
	tablaPaginas_t *tablaDePaginas = obtenerTablaDePaginas();
	pthread_mutex_lock(&mutex_tablaDePaginas);
	tablaDePaginas[marco].pid = pid;
	printf("cargue el pid : %d \n", pid);
	tablaDePaginas[marco].pagina = pagina;
	printf("cargue la pagina : %d \n", pagina);
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

void agregarNuevoProceso(uint32_t pid, uint32_t paginasRequeridas) {
	guardaProcesoEn(pid, paginasRequeridas);
	actualizarMarcosDisponibles(paginasRequeridas);
}

int tieneMarcosSuficientes(int paginasRequeridas) {
	return paginasRequeridas <= configDeMemoria.marcosDisponibles;
}

void inicializarPrograma(uint32_t pid, uint32_t paginasRequeridas) {
	//pthread_mutex_lock(mutexMemoria);
	agregar_DataDeProcesoActivo(pid, paginasRequeridas);
	printf("tengo marcos suficientes \n");
	agregarNuevoProceso(pid, paginasRequeridas);
	//pthread_mutex_unlock(mutexMemoria);
}

uint32_t finalizarPrograma(uint32_t pid) {
	tablaPaginas_t* tablaDePaginas = obtenerTablaDePaginas();
	int pagina, paginasMaximas, i = 0;
	pagina = obtener_PaginaDeInicioDeProcesoActivo(pid);
	int marco = getMarco(pid, pagina);
	if (marco < 0)
		return -1;
	paginasMaximas = obtener_ProximaPaginaAAsignar(pid);
	while (i < paginasMaximas) {
		if (marco >= 0) {
			pthread_mutex_lock(&mutex_tablaDePaginas);
			tablaDePaginas[marco].pid = -2;
			tablaDePaginas[marco].pagina = marco;
			pthread_mutex_unlock(&mutex_tablaDePaginas);
		}
		pagina++;
		i++;
		marco = getMarco(pid, pagina);
	}
	obtener_PosicionProcesoActivo(pid);
	eliminar_DataDeProcesoActivo(pid);
	return 0;
}

int asignarPaginasAUnProceso(uint32_t pid, uint32_t paginasRequeridas) {
	int pagina, marco, i = 0;
	tablaPaginas_t* tablaDePaginas;
	int resultado = tieneMarcosSuficientes(paginasRequeridas);
	if (resultado == 0)
		return -1;
	tablaDePaginas = obtenerTablaDePaginas();
	actualizarMarcosDisponibles(paginasRequeridas);
	pagina = obtener_ProximaPaginaAAsignar(pid);
	while (i < paginasRequeridas) {
		marco = nuevoMarco(pid, pagina);
		pthread_mutex_lock(&mutex_tablaDePaginas);
		tablaDePaginas[marco].pid = pid;
		tablaDePaginas[marco].pagina = pagina;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		pagina++;
		i++;
	}
	aumentar_PaginasActualesDeProcesoActivo(pid, paginasRequeridas);
	return 0;
}

int eliminarPaginaDeUnProceso(uint32_t pid, uint32_t paginaAEliminar) {
	tablaPaginas_t* tablaDePaginas;
	tablaDePaginas = obtenerTablaDePaginas();
	int marco = getMarco(pid, paginaAEliminar);
	if (marco < 0)
		return -1;
	pthread_mutex_lock(&mutex_tablaDePaginas);
	tablaDePaginas[marco].pid = -2;
	tablaDePaginas[marco].pagina = -2;
	pthread_mutex_unlock(&mutex_tablaDePaginas);
	disminuir_PaginasActualesDeProcesoActivo(pid, paginaAEliminar);
	return 0;
}

void* leerMemoria(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tam) { //REQUIERE FREE
	int marco = getMarco(pid, pag);
	if (marco < 0)
		return NULL;
	void* datos = malloc(tam);
	usleep(configDeMemoria.retardo);
	pthread_mutex_lock(&escribiendoMemoria);
	memcpy(datos, memoria + marco * configDeMemoria.tamMarco + offset, tam);
	pthread_mutex_unlock(&escribiendoMemoria);
	return datos;
}

uint32_t escribirMemoria(uint32_t pid, uint32_t pag, uint32_t offset,
		uint32_t tamData, void *data) {
	int marco = getMarco(pid, pag);
	if (marco < 0)
		return -1;
	usleep(configDeMemoria.retardo);
	pthread_mutex_lock(&escribiendoMemoria);
	memcpy(memoria + marco * configDeMemoria.tamMarco + offset, data, tamData);
	pthread_mutex_unlock(&escribiendoMemoria);
	return 0;
}

void* leerCache(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tam) { //NECESITA UN FREE :D
	uint32_t marco = getMarcoCache(pid, pag);
	void* datos = malloc(tam);
	pthread_mutex_lock(&escribiendoMemoriaCache);
	memcpy(datos, cache + marco * configDeMemoria.tamMarco + offset, tam);
	pthread_mutex_unlock(&escribiendoMemoriaCache);
	actualizarCounterMarco(marco);
	return datos;
}

int nuevoMarcoDelMismoProceso(int pid) {
	tablaCache_t* tabla = obtenerTablaCache();
	int marco, paginas = 0, counter = 0, marcoNuevo = -1;
	for (marco = 0; marco < configDeMemoria.entradasCache; marco++) {
		pthread_mutex_lock(&mutex_tablaCache);
		if (tabla[marco].pid == pid) {
			paginas++;
			if (counter == 0 || tabla[marco].counter < counter) {
				marcoNuevo = marco;
				counter = tabla[marco].counter;
			}
		}
		pthread_mutex_unlock(&mutex_tablaCache);
	}
	if (paginas < configDeMemoria.cachePorProceso)
		return -1;
	return marcoNuevo;
}

int buscarMarcoVacioCache() {
	tablaCache_t* tabla = obtenerTablaCache();
	int marco = 0;
	while (marco < configDeMemoria.entradasCache) {
		if (tabla[marco].pid == -2)
			return marco;
		marco++;
	}
	return -2;
}

int agregarProcesoACache(int pid, int pag) {
	int marco = nuevoMarcoDelMismoProceso(pid);
	if (marco < 0) {
		if (configDeMemoria.cacheDisponible > 0) {
			marco = buscarMarcoVacioCache();
			configDeMemoria.cacheDisponible--;
		} else
			marco = lru();
	}
	tablaCache_t* tabla = obtenerTablaCache();
	tabla[marco].pid = pid;
	tabla[marco].pagina = pag;
	tabla[marco].counter = contador++;
	return marco;
}

void copiarMemoriaACache(int pid, int pag) {
	int marco = getMarcoCache(pid, pag);
	void* data = leerMemoria(pid, pag, 0, configDeMemoria.tamMarco);
	escribirCache(marco, 0, configDeMemoria.tamMarco, data);
}

void* leer(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tam) { // SIGUE NECESITANDO UN FREE
	if (estaEnCache(pid, pag))
		return leerCache(pid, pag, offset, tam);
	else
		return leerMemoria(pid, pag, offset, tam);
}

int escribir(uint32_t pid, uint32_t pag, uint32_t offset, uint32_t tamData,
		void *data) {
	int e = escribirMemoria(pid, pag, offset, tamData, data);
	if (estaEnCache(pid, pag))
		escribirCache(getMarcoCache(pid, pag), offset, tamData, data);
	else {
		agregarProcesoACache(pid, pag);
		copiarMemoriaACache(pid, pag);
	}
	return e;
}
/*
int cantPaginasEnCache(int pid) {
	tablaCache_t* tabla = obtenerTablaCache();
	int marco, paginas = 0;
	for (marco = 0; marco < configDeMemoria.entradasCache; marco++) {
		pthread_mutex_lock(&mutex_tablaCache);
		if (tabla[marco].pid == pid)
			paginas++;
		pthread_mutex_unlock(&mutex_tablaCache);
	}
	return paginas;
}*/

void guardarCadena(char** cadena, int pid, int pag, char* contenido){
	char* pidchar = malloc(sizeof(int)+1);
	char* pagchar = malloc(sizeof(int)+1);
	sprintf(pidchar, "%i", pid);
	sprintf(pagchar, "%i", pag);
	memset(pidchar,'\0',sizeof(int)+1);
	memset(pagchar,'\0',sizeof(int)+1);
	string_append(cadena, "PID: ");
	string_append(cadena, pidchar);
	string_append(cadena, " PAG: ");
	string_append(cadena, pagchar);
	string_append(cadena, " Contenido: ");
	string_append(cadena, contenido);
	string_append(cadena, "\n");
	free(pidchar);
	free(pagchar);
}

void escribirCadenaEnArchivo(char* nombre, char* cadena, int counter, int tamI){
	//int tam = (sizeof(int)*2 + 24 + configDeMemoria.tamMarco)* counter + tamI+1;
	string_append(&cadena, "\n");
	FILE* archivo;
	archivo = fopen(nombre, "ab");
	fwrite(cadena,1,string_length(cadena) ,archivo);
	free(cadena);
	fclose(archivo);
}

void mostrarCache() {
	char* cadena = string_new();
	string_append(&cadena, "Se Mostrara el contenido de la cache\n");
	int tam = 37;
	char* data = malloc(configDeMemoria.tamMarco);
	tablaCache_t* tabla = obtenerTablaCache();
	int marco, pid, pagina, counter = 0;
	for (marco = 0; marco < configDeMemoria.entradasCache; marco++) {
		pthread_mutex_lock(&mutex_tablaCache);
		pid = tabla[marco].pid;
		pagina = tabla[marco].pagina;
		pthread_mutex_unlock(&mutex_tablaCache);
		if (pid >= 0) {
			data = (char*)leerCache(pid, pagina, 0, configDeMemoria.tamMarco);
			printf("PID: %i	PAG: %i	Contenido: %s\n", pid, pagina, data);
			guardarCadena(&cadena, pid, pagina, data);
			counter++;
		}
	}
	if (counter == 0){
		string_append(&cadena, "La cache esta vacia\n");
		tam += 20;
		texto_en_color_de_error("La cache esta vacia");
	}
	escribirCadenaEnArchivo("ContenidoCache.txt", cadena, counter, tam);
	free(data);
}

void mostrarMemoria() {
	char* cadena = string_new();
	string_append(&cadena, "Se Mostrara el contenido de la memoria\n");
	int tam = 39;
	char* data = malloc(configDeMemoria.tamMarco);
	tablaPaginas_t* tabla = obtenerTablaDePaginas();
	int marco, pid, pagina, counter = 0;
	for (marco = 0; marco < configDeMemoria.marcos; marco++) {
		pthread_mutex_lock(&mutex_tablaDePaginas);
		pid = tabla[marco].pid;
		pagina = tabla[marco].pagina;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		if (pid >= 0) {
			data = (char*)leerMemoria(pid, pagina, 0, configDeMemoria.tamMarco);
			printf("PID: %i	PAG: %i	Contenido: %s\n", pid, pagina, data);
			guardarCadena(&cadena, pid, pagina, data);
			counter++;
		}
	}
	if (counter == 0){
		tam += 22;
		string_append(&cadena, "La memoria esta vacia\n");
		texto_en_color_de_error("La Memoria esta vacia\n");
	}
	escribirCadenaEnArchivo("ContenidoMemoria.txt", cadena, counter, tam);
	free(data);
}

void mostrarProcesoEnMemoria(int pid) {
	char* cadena = string_new();
	string_append(&cadena, "Se Mostrara el contenido del proceso: ");
	char* pidchar = malloc(sizeof(int));
	printf("el pid es: %d \n",pid);
	sprintf(pidchar, "%i", pid);
	string_append(&cadena, pidchar);
	free(pidchar);
	string_append(&cadena, "\n");
	int tam = 39 + sizeof(int);
	char* data = malloc(configDeMemoria.tamMarco);
	tablaPaginas_t* tabla = obtenerTablaDePaginas();
	int marco, pagina,resultado, counter = 0;
	for (marco = 0; marco < configDeMemoria.marcos; marco++) {
		pthread_mutex_lock(&mutex_tablaDePaginas);
		resultado =pid == tabla[marco].pid;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		if (resultado) {
			pthread_mutex_lock(&mutex_tablaDePaginas);
			pagina = tabla[marco].pagina;
			pthread_mutex_unlock(&mutex_tablaDePaginas);
			printf("pagina %d \n",pagina);
			data =leerMemoria(pid, pagina, 0, configDeMemoria.tamMarco);
			printf("PID: %i	PAG: %i	Contenido: %s\n", pid, pagina,data);
			if(data)
			guardarCadena(&cadena, pid, pagina, data);
			//if(data)free(data);
			counter++;
		}
	}
	if (counter == 0){
		tam += 28;
		string_append(&cadena, "El proceso no tiene paginas\n");
		texto_en_color_de_error("El proceso no tiene paginas\n");
	}
	escribirCadenaEnArchivo("ContenidoMemoria.txt", cadena, counter, tam);
	free(data);
}
