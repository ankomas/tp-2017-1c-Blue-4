/*
 * capaFs.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include "capaFs.h"
#include <sys/socket.h>

///////////////// Funciones que interactuan con el FS
// Usalas con sabiduria, porque requieren validaciones previas

bool mandarOperacionFS(char* opcode,char* path,uint32_t tamPath,char* error){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);
	send(idFS,opcode,1,0);

	char* tamPathStream = intToStream(tamPath);
	if(sendall(idFS,tamPathStream, &tamPath) < 0){
		log_error(logger,error);
		return 0;
	}
	free(tamPathStream);

	if(sendall(idFS,path, &tamPath) < 0){
		log_error(logger,error);
		return 0;
	}

	// Recibo confirmacion
	if(recv(idFS,rev,tamARecibir,MSG_WAITALL) <= 0){
		log_error(logger,error);
		return 0;
	}

	if(rev[0]== 'Y')
		return 1;

	return 0;
}

char* continuacionPeticionLectura(uint32_t i,uint32_t offset,uint32_t size,char*error){
	uint32_t tamARecibir=0;
	char * rev = NULL;
	uint32_t tamAMandar=sizeof(uint32_t);

	char* offsetStream = intToStream(offset);
	char* sizeStream = intToStream(size);
	if(sendall(idFS,offsetStream, &tamAMandar) < 0){
		log_error(logger,error);
		return 0;
	}
	free(offsetStream);

	if(sendall(idFS,sizeStream, &tamAMandar) < 0){
		log_error(logger,error);
		return 0;
	}
	free(sizeStream);

	// Recibo largo del data
	if(recv(idFS,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0){
		log_error(logger,error);
		send(i,"N",1,0);
		return NULL;
	}

	/*if(send(idFS,"Y",1,0) < 0)
		log_error(logger,error);*/

	// Recibo confirmacion
	if(recv(idFS,rev,1,0) <= 0){
		log_error(logger,error);
		return NULL;
	}

	if(rev[0] == 'Y'){
		rev=realloc(rev,tamARecibir);
		memset(rev,'\0',tamARecibir);

	// Recibo el data
	if(recv(idFS,rev,tamARecibir,MSG_WAITALL) <= 0){
		log_error(logger,error);
		return NULL;
	}

	send(i,"Y",1,0);
	sendall(i,rev,&tamARecibir);
	return rev;
}
}



bool continuacionPeticionEscritura(uint32_t offset,uint32_t size,char*buffer,uint32_t tamBuffer,char*error){
		char * rev = NULL;
		uint32_t tamAMandar=sizeof(uint32_t);

		char* offsetStream = intToStream(offset);
		char* sizeStream = intToStream(size);
		if(sendall(idFS,offsetStream, &tamAMandar) < 0){
			log_error(logger,error);
			return 0;
		}
		free(offsetStream);

		if(sendall(idFS,sizeStream, &tamAMandar) < 0){
			log_error(logger,error);
			return 0;
		}
		free(sizeStream);

		// Mando largo del buffer
		char* tamBufferStream = intToStream(tamBuffer);
		if(sendall(idFS,tamBufferStream,&tamBuffer) < 0){
			log_error(logger,error);
			return NULL;
		}
		free(tamBufferStream);

		// Mando buffer
		if(sendall(idFS,buffer,&tamBuffer) < 0){
			log_error(logger,error);
			return NULL;
		}

		// Recibo confirmacion
		if(recv(idFS,rev,1,0) <= 0){
			log_error(logger,error);
			return NULL;
		}

		if(rev[0] == 'Y')
			return 1;
		else
			return 0;
}

bool borrarArchivo(char* path,uint32_t tamPath){
	return mandarOperacionFS("B",path,tamPath,"Ocurrio un error al crear un archivo");
}

bool crearArchivo(char* path,uint32_t tamPath){
	return mandarOperacionFS("C",path,tamPath,"Ocurrio un error al crear un archivo");
}

char* leerArchivo(char*path,uint32_t tamPath, uint32_t i,uint32_t offset,uint32_t size){

	if(mandarOperacionFS("L",path,tamPath,"Ocurrio un error al leer un archivo") == 0){
		log_error(logger,"No se pudo leer archivo.");
		send(i,"N",1,0);
		return 0;
	}
	continuacionPeticionLectura(i,offset,size,"Ocurrio un error al leer un archivo");
	return 0;
}

bool escribirArchivo(char* path,uint32_t tamPath){
	if(mandarOperacionFS("E",path,tamPath,"Ocurrio un error al escribir un FD") == 0){
		return 0;
	}
		// mandar resto de cosas
	return 0;
}

bool validarArchivo(char* path,uint32_t tamPath){
	return mandarOperacionFS("V",path,tamPath,"Ocurrio un error al validar un FD");
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
	char* path = recibirPath(i);
	char* permisos = recibirPermisos(i);
	if(path == NULL || permisos == NULL){
		return 9999;
	}

	if(validarArchivo(path,strlen(path)) == 1 || !tienePermisos('c',permisos)){
		send(i,"N",1,0);
		log_error(logger,"No hay permisos para crear un nuevo archivo o el archivo ya esta abierto");
		return 9999;
	}

	unPrograma->FDCounter++;
	t_entradaTAP * nuevaEntradaTAP = malloc(sizeof(nuevaEntradaTAP));
	nuevaEntradaTAP->flags = permisos;

	t_entradaTGA * nuevaEntradaTGA = buscarFDPorPath(path);
	if(nuevaEntradaTGA != NULL){
		nuevaEntradaTGA->abierto++;
		nuevaEntradaTGA->archivo = path;
	} else {
		nuevaEntradaTGA = malloc(sizeof(nuevaEntradaTGA));
		nuevaEntradaTGA->archivo = path;
		nuevaEntradaTGA->abierto = 1;
		nuevaEntradaTGA->indice = GlobalFDCounter;
		list_add(tablaGlobalArchivos,nuevaEntradaTGA);
		GlobalFDCounter++;
		log_trace(logger,"Nuevo FD creado");
	}

	nuevaEntradaTAP->globalFD = nuevaEntradaTGA->indice;
	nuevaEntradaTAP->indice = unPrograma->FDCounter;
	list_add(unPrograma->tablaArchivosPrograma,nuevaEntradaTAP);

	send(i,"Y",1,0);
	log_trace(logger,"Se creo un archivo con exito");

	return nuevaEntradaTAP->indice;
}

bool borrarFD(uint32_t i,t_programa* unPrograma){
	uint32_t fd = 0;
	recv(i,&fd,sizeof(fd),MSG_WAITALL);
	send(i,"Y",1,0);

	char* path = recibirPath(i);
	if(path == NULL){
		return 0;
	}

	if(validarArchivo(path,strlen(path)) == 0){
		send(i,"N",1,0);
		log_error(logger,"No existe el archivo que se quiere borrar");
		return 0;
	}

	uint32_t indiceGlobalFD = buscarFDPorId(fd);
	if(indiceGlobalFD != 9999){
		t_entradaTGA * aux = list_get(tablaGlobalArchivos,indiceGlobalFD);
		if(aux->abierto == 1){
			if(borrarArchivo(aux->archivo,strlen(aux->archivo))){
				list_remove(unPrograma->tablaArchivosPrograma,fd);
				list_remove(tablaGlobalArchivos,indiceGlobalFD);
				list_remove(unPrograma->tablaArchivosPrograma,buscarFDArchivoPorId(fd,unPrograma));
				return 1;
				log_trace(logger,"Se borro el FD correctamente");
				send(i,"Y",1,0);
			} else {
				return 0;
				// mandar proceso a exit recibiendo pcb de cpu
				log_trace(logger,"No se pudo borrar el FD correctamente");
				send(i,"N",1,0);
			}
		}
	}
	return 0;
}

bool cerrarFD(uint32_t i, t_programa* unPrograma){
	uint32_t fd = 0;
	recv(i,&fd,sizeof(fd),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t indiceGlobalFD = buscarFDPorId(fd);
	if(indiceGlobalFD != 9999){
		list_remove(unPrograma->tablaArchivosPrograma,fd);//ojo este fd es un puntero que no es absoluto
		t_entradaTGA * aux = list_get(tablaGlobalArchivos,indiceGlobalFD);
		if(aux->abierto == 1){
			list_remove(tablaGlobalArchivos,indiceGlobalFD);
		} else {
			aux->abierto--;
		}
		list_remove(unPrograma->tablaArchivosPrograma,buscarFDArchivoPorId(fd,unPrograma));
		log_trace(logger,"Archivo cerrado");
		send(i,"Y",1,0);
		return 1;
	}else{
		log_trace(logger,"Se intento cerrar un archivo que no existia");
		send(i,"N",1,0);
		return 0;
	}
}

bool escribirFD(uint32_t i,t_programa* unPrograma){
	uint32_t fd = 0;
	recv(i,&fd,sizeof(fd),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t offset = 0;
	recv(i,&offset,sizeof(offset),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t tamanio = 0;
	recv(i,&tamanio,sizeof(offset),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t tamRecibirData = 0;
	recv(i,&tamRecibirData,sizeof(tamRecibirData),MSG_WAITALL);
	send(i,"Y",1,0);

	char* data = NULL;
	data=realloc(data,tamRecibirData+1);
	memset(data,'\0',tamRecibirData+1);
	recv(i,&tamanio,sizeof(offset),MSG_WAITALL);
	send(i,"Y",1,0);

	char* path = recibirPath(i);
	char* permisos = recibirPermisos(i);
	if(path == NULL || permisos == NULL){
		send(i,"N",1,0);
		return 0;
	}

	if(validarArchivo(path,strlen(path)) == 1 || !tienePermisos('e',permisos)){
		send(i,"N",1,0);
		log_error(logger,"No hay permisos para crear un nuevo archivo o el archivo no existe");
		return 0;
	}

	uint32_t indiceFD = buscarFDArchivoPorId(fd,unPrograma);
	if(indiceFD != 9999 || fd == 1){
		t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,indiceFD);
		if(tienePermisos('e',entradaFD->flags)){
			if(fd == 1){
				imprimirPorConsola(unPrograma->id,data,tamanio);
			} else {
				if(escribirArchivo(path,strlen(path))){
					log_trace(logger,"Se creo un archivo correctamente");
					send(i,"Y",1,0);
					return 1;
				}else{
					log_error(logger,"No se pudo escribir un archivo correctamente");
					send(i,"N",1,0);
					return 0;
				}
			}
		} else {
			log_error(logger,"No hay permisos para escribir en un archivo");
			return 0;
		}
	}
	return 0;
}

char* leerFD(uint32_t i,t_programa* unPrograma){
	char* path = recibirPath(i);
	char* permisos = recibirPermisos(i);
	if(path == NULL || permisos == NULL){
		send(i,"N",1,0);
		return 0;
	}

	uint32_t fd = 0;
	recv(i,&fd,sizeof(fd),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t offset = 0;
	recv(i,&offset,sizeof(offset),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t tamanio = 0;
	recv(i,&tamanio,sizeof(offset),MSG_WAITALL);
	send(i,"Y",1,0);

	if(validarArchivo(path,strlen(path)) == 1 || !tienePermisos('l',permisos)){
		send(i,"N",1,0);
		log_error(logger,"No hay permisos para crear un nuevo archivo");
		return 0;
	}

	uint32_t indiceFD = buscarFDArchivoPorId(fd,unPrograma);
	if(indiceFD != 9999){
		t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,indiceFD);
		if(tienePermisos('l',entradaFD->flags)){
			leerArchivo(path,strlen(path),i,offset,tamanio);
			send(i,"Y",1,0);
		} else {
			log_error(logger,"No hay permisos para escribir un nuevo archivo");
			send(i,"N",1,0);
			return NULL;
		}
	}
	send(i,"N",1,0);
	return NULL;
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
	while(contador < strlen(permisos)){
		if(acceso == permisos[contador])
			return true;
		contador++;
	}

	return false;
}

char* recibirPath(uint32_t i){
	uint32_t tamARecibir=0;
		char * rev = NULL;

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
	return NULL;
}

char* recibirPermisos(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = NULL;
	char * permisosRev = NULL;

	// Recibo longitud permisos
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		log_error(logger,"No se pudieron recibir los permisos");
	if(send(i,"Y",1,0) < 0)
		log_error(logger,"No se pudieron recibir los permisos");
	rev=realloc(permisosRev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);
	// Recibo permisos
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		log_error(logger,"No se pudieron recibir los permisos");
	send(i,"Y",1,0);

	return NULL;

}
