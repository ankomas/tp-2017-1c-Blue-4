/*
 * dump.c
 *
 *  Created on: 13/7/2017
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
#include <commons/string.h>
#include "blue4-lib.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include <pthread.h>
#include "configuraciones.h"
#include "estructurasAdministrativas.h"

void guardarCadena(char** cadena, int pid, int pag, char* contenido){
	char* pidchar = malloc(sizeof(int));
	char* pagchar = malloc(sizeof(int));
	sprintf(pidchar, "%i", pid);
	sprintf(pagchar, "%i", pag);
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

void escribirCadenaEnArchivo(char* nombre, char* cadena){
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
	texto_en_color("Se Mostrara el contenido de la cache\n");
	//int tam = 37;
	char* dataACopiar;
	char* data = malloc(configDeMemoria.tamMarco);
	tablaCache_t* tabla = obtenerTablaCache();
	int marco, pid, pagina, counter = 0;
	for (marco = 0; marco < configDeMemoria.entradasCache; marco++) {
		pthread_mutex_lock(&mutex_tablaCache);
		pid = tabla[marco].pid;
		pagina = tabla[marco].pagina;
		pthread_mutex_unlock(&mutex_tablaCache);
		if (pid >= 0) {
			dataACopiar = (char*)leerCache(pid, pagina, 0, configDeMemoria.tamMarco);
			memset(data,'\0',configDeMemoria.tamMarco);
			memcpy(data,dataACopiar,configDeMemoria.tamMarco);
			printf("%sPID: %i	PAG: %i	Contenido: %s\n",KGRN, pid, pagina, data);
			guardarCadena(&cadena, pid, pagina, data);
			counter++;
			if(dataACopiar)free(dataACopiar);
		}
	}
	if (counter == 0){
		string_append(&cadena, "La cache esta vacia\n");
		//tam += 20;
		texto_en_color_de_error("La cache esta vacia");
	}
	escribirCadenaEnArchivo("ContenidoCache.txt", cadena);
	free(data);
}

void mostrarMemoria() {
	char* cadena = string_new();
	string_append(&cadena, "Se Mostrara el contenido de la memoria\n");
	//int tam = 39;
	char* dataACopiar;
	char* data =malloc(configDeMemoria.tamMarco);
	tablaPaginas_t* tabla = obtenerTablaDePaginas();
	int marco, pid, pagina, counter = 0;
	for (marco = 0; marco < configDeMemoria.marcos; marco++) {
		pthread_mutex_lock(&mutex_tablaDePaginas);
		pid = tabla[marco].pid;
		pagina = tabla[marco].pagina;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		if (pid >= 0) {
			dataACopiar = (char*)leerMemoria(pid, pagina, 0, configDeMemoria.tamMarco);
			memset(data,'\0',configDeMemoria.tamMarco);
			memcpy(data,dataACopiar,configDeMemoria.tamMarco);
			printf("%sPID: %i	PAG: %i	Contenido: %s\n",KGRN, pid, pagina, data);
			guardarCadena(&cadena, pid, pagina, data);
			if(dataACopiar)free(dataACopiar);
			counter++;
		}
	}
	if (counter == 0){
		//tam += 22;
		string_append(&cadena, "La memoria esta vacia\n");
		texto_en_color_de_error("La Memoria esta vacia\n");
	}
	escribirCadenaEnArchivo("ContenidoMemoria.txt", cadena);
	free(data);
}

void mostrarProcesoEnMemoria(int pid) {
	char* cadena = string_new();
	string_append(&cadena, "Se Mostrara el contenido del proceso: ");
	char* pidchar = malloc(sizeof(int));
	sprintf(pidchar, "%i", pid);
	string_append(&cadena, pidchar);
	free(pidchar);
	string_append(&cadena, "\n");
	//int tam = 39 + sizeof(int);
	char* dataACopiar;
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
			dataACopiar =leerMemoria(pid, pagina, 0, configDeMemoria.tamMarco);
			memset(data,'\0',configDeMemoria.tamMarco);
			memcpy(data,dataACopiar,configDeMemoria.tamMarco);
			printf("%sPID: %i	PAG: %i	Contenido: %s\n",KGRN, pid, pagina,data);
			guardarCadena(&cadena, pid, pagina, data);
			if(dataACopiar)free(dataACopiar);
			counter++;
		}
	}
	if (counter == 0){
		//tam += 28;
		string_append(&cadena, "El proceso no tiene paginas\n");
		texto_en_color_de_error("El proceso no tiene paginas\n");
	}
	escribirCadenaEnArchivo("ContenidoMemoria.txt", cadena);
	free(data);
}


void guardarCadenaTabla(char** cadena, int marco, int pid, int pag){
	char* pidchar = malloc(sizeof(int));
	char* pagchar = malloc(sizeof(int));
	char* marcochar = malloc(sizeof(int));
	sprintf(marcochar, "%i", marco);
	sprintf(pidchar, "%i", pid);
	sprintf(pagchar, "%i", pag);
	string_append(cadena, "Marco: ");
	string_append(cadena, marcochar);
	string_append(cadena, " PID: ");
	string_append(cadena, pidchar);
	string_append(cadena, " PAG: ");
	string_append(cadena, pagchar);
	string_append(cadena, "\n");
	free(marcochar);
	free(pidchar);
	free(pagchar);
}

void escribirCadenaEnArchivoTabla(char* nombre, char* cadena){
	string_append(&cadena, "\n");
	FILE* archivo;
	archivo = fopen(nombre, "ab");
	fwrite(cadena,1,string_length(cadena) ,archivo);
	free(cadena);
	fclose(archivo);
}

void mostrarTablaDePaginas(){
	int i, pid, pag;
	char* cadena = string_new();
	string_append(&cadena, "Se Mostrara el contenido de la tabla de paginas\n"); //tam: 49
	tablaPaginas_t* tablaDePaginas = obtenerTablaDePaginas();
	texto_en_color("Se mostrara a continuacion solo los marcos que se esten usando en la tabla de paginas:\n\n");
	for(i=0; i<configDeMemoria.marcos;i++){
		pthread_mutex_lock(&mutex_tablaDePaginas);
		pid = tablaDePaginas[i].pid;
		pag = tablaDePaginas[i].pagina;
		pthread_mutex_unlock(&mutex_tablaDePaginas);
		if(pid!=-2)
		{
			printf("%s Marco: %i	 PID: %i	Pagina: %i %s \n",KGRN,i,pid, pag,KNRM );
			guardarCadenaTabla(&cadena, i, pid, pag);
		}
	}
	escribirCadenaEnArchivoTabla("ContenidoTablaDePaginas.txt",cadena);
}
