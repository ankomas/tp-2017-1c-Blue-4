/*
 * capaFs.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include "capaFs.h"

void imprimirPorConsola(uint32_t socket,char*data,uint32_t tamanio){
	uint32_t uno = 1;
	if(sendall(socket,"I",&uno) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
	if(sendall(socket,data,&tamanio) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
}

uint32_t abrirFD(t_programa* unPrograma,char* permisos){
	char*path;
	//char* path, char* permisos
	unPrograma->FDCounter++;
	t_entradaTAP * nuevaEntradaTAP = malloc(sizeof(nuevaEntradaTAP));
	nuevaEntradaTAP->flags = permisos;

	t_entradaTGA * nuevaEntradaTGA = buscarFDPorPath(path);
	if(nuevaEntradaTGA != NULL){
		nuevaEntradaTGA->abierto++;
		nuevaEntradaTGA->archivo = path;
	} else {
		if(tienePermisos('c',permisos)){
			nuevaEntradaTGA = malloc(sizeof(nuevaEntradaTGA));
			nuevaEntradaTGA->archivo = path;
			nuevaEntradaTGA->abierto = 1;
			nuevaEntradaTGA->indice = GlobalFDCounter;
			list_add(tablaGlobalArchivos,nuevaEntradaTGA);
			GlobalFDCounter++;
		}else {
			log_error(logger,"No hay permisos para crear un nuevo archivo");
			return 9999;
		}
	}

	nuevaEntradaTAP->globalFD = nuevaEntradaTGA->indice;
	nuevaEntradaTAP->indice = unPrograma->FDCounter;
	list_add(unPrograma->tablaArchivosPrograma,nuevaEntradaTAP);

	return nuevaEntradaTAP->indice;
}

void cerrarFD(t_programa* unPrograma,uint32_t fd){
	uint32_t indiceGlobalFD = buscarFDPorId(fd);
	if(indiceGlobalFD != 9999){
		list_remove(unPrograma->tablaArchivosPrograma,fd);
		t_entradaTGA * aux = list_get(tablaGlobalArchivos,indiceGlobalFD);
		if(aux->abierto == 1){
			list_remove(tablaGlobalArchivos,indiceGlobalFD);
		} else {
			aux->abierto--;
		}
		list_remove(unPrograma->tablaArchivosPrograma,buscarFDArchivoPorId(fd,unPrograma));
	}else{
		log_trace(logger,"Se intento borrar un FD que no existia");
	}
}

uint32_t escribirFD(t_programa* unPrograma,void* data,uint32_t fd, uint32_t offset, uint32_t tamanio){
	uint32_t indiceFD = buscarFDArchivoPorId(fd,unPrograma);
	if(indiceFD != 9999 || fd == 1){
		t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,indiceFD);
		if(tienePermisos('w',entradaFD->flags)){
			if(fd == 1){
				imprimirPorConsola(unPrograma->id,data,tamanio);
			} else {
				// Mandar datos al filesystem
			}
		} else {
			log_trace(logger,"No hay permisos para escribir un nuevo archivo");
			return 9999;
		}
	}
	return 0;
}

uint32_t leerFD(t_programa* unPrograma,uint32_t fd, uint32_t offset, uint32_t tamanio){
	uint32_t indiceFD = buscarFDArchivoPorId(fd,unPrograma);
	if(indiceFD != 9999){
		t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,indiceFD);
		if(tienePermisos('r',entradaFD->flags)){
			// Mandar datos al filesystem
		} else {
			log_trace(logger,"No hay permisos para escribir un nuevo archivo");
			return 9999;
		}
	}
	return 0;
}

t_entradaTGA * buscarFDPorPath(char* path){
	int contador = 0;
	while(contador < list_size(tablaGlobalArchivos)){
		t_entradaTGA * aux = list_get(tablaGlobalArchivos,contador);
		if(strcmp(aux->archivo,path)){
			return aux;
		}
		contador++;
	}
	return NULL;
}

uint32_t buscarFDPorId(uint32_t id){
	int contador = 0;
	while(contador < list_size(tablaGlobalArchivos)){
		t_entradaTGA * aux = list_get(tablaGlobalArchivos,contador);
		if(aux->indice == id){
			return id;
		}
		contador++;
	}
	return 9999;
}

uint32_t buscarFDArchivoPorId(uint32_t id,t_programa*unPrograma){
	// Ojo, es un indexOf, devuelve el indice del FD, no su numero
	uint32_t contador = 0;
	while(contador < list_size(unPrograma->tablaArchivosPrograma)){
		t_entradaTAP * aux = list_get(unPrograma->tablaArchivosPrograma,contador);
		if(aux->indice == id){
			return contador;
		}
		contador++;
	}
	return 9999;
}

bool tienePermisos(char acceso,char*permisos){
	int contador = 0;
	while(contador < string_length(permisos)){
		if(acceso == permisos[contador])
			return true;
		contador++;
	}

	return false;
}
