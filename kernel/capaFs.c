/*
 * capaFs.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include "capaFs.h"
#include <sys/socket.h>

///////////////// Funciones que interactuan con el FS

bool archivoValido(char* path,uint32_t tamPath){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);
	send(idFS,"V",1,0);
	if(sendall(idFS,path, &tamPath) < 0){
		log_error(logger,"Ocurrio un problema al validar un FD");
		return 0;
	}

	// Recibo confirmacion
	if(recv(idFS,rev,tamARecibir,MSG_WAITALL) <= 0){
		log_error(logger,"Ocurrio un problema al validar un FD");
		return 0;
	}
	if(rev[0]== 'Y')
		return 1;
	return 0;
}

bool crearArchivo(char* path,uint32_t tamPath){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);
	send(idFS,"C",1,0);
	if(sendall(idFS,path, &tamPath) < 0){
		log_error(logger,"Ocurrio un problema al crear un FD");
		return 0;
	}

	// Recibo confirmacion
	if(recv(idFS,rev,tamARecibir,MSG_WAITALL) <= 0){
		log_error(logger,"Ocurrio un problema al crear un FD");
		return 0;
	}

	if(rev[0]== 'Y')
		return 1;

	return 0;
}



////////////////// FIN Funciones que interactuan con el FS

void imprimirPorConsola(uint32_t socket,char*data,uint32_t tamanio){
	uint32_t uno = 1;
	if(sendall(socket,"I",&uno) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
	if(sendall(socket,data,&tamanio) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
}

uint32_t abrirFD(uint32_t i,t_programa* unPrograma){
	uint32_t tamARecibir=0;
	char * rev = NULL;
	char * permisosRev = NULL;

	// Recibo largo del path
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		log_error(logger,"Ocurrio un problema al abrir un FD");
	if(send(i,"Y",1,0) < 0)
		log_error(logger,"Ocurrio un problema al abrir un FD");
	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);
	// Recibo el nombre del semaforo
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		log_error(logger,"Ocurrio un problema al abrir un FD");
	send(i,"Y",1,0);

	tamARecibir=0;
	// Recibo longitud permisos
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		log_error(logger,"Ocurrio un problema al abrir un FD");
	if(send(i,"Y",1,0) < 0)
		log_error(logger,"Ocurrio un problema al abrir un FD");
	rev=realloc(permisosRev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);
	// Recibo permisos
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		log_error(logger,"Ocurrio un problema al abrir un FD");
	send(i,"Y",1,0);

	if(archivoValido(rev,tamARecibir) && tienePermisos('c',permisosRev)){
		crearArchivo(rev,tamARecibir);
		send(i,"Y",1,0);
		log_trace(logger,"Se creo un archivo con exito");
	} else {
		send(i,"N",1,0);
		log_trace(logger,"No se pudo crear un archivo");
	}

	unPrograma->FDCounter++;
	t_entradaTAP * nuevaEntradaTAP = malloc(sizeof(nuevaEntradaTAP));
	nuevaEntradaTAP->flags = permisosRev;

	t_entradaTGA * nuevaEntradaTGA = buscarFDPorPath(rev);
	if(nuevaEntradaTGA != NULL){
		nuevaEntradaTGA->abierto++;
		nuevaEntradaTGA->archivo = rev;
	} else {
		if(tienePermisos('c',permisosRev)){
			nuevaEntradaTGA = malloc(sizeof(nuevaEntradaTGA));
			nuevaEntradaTGA->archivo = rev;
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
