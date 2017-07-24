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
	char * rev = malloc(1);
	log_trace(logger,"Llamada a MANDAR OPERACION FS");
	log_trace(logger,opcode);
	send(idFS,opcode,1,0);
	uint32_t tamuint=sizeof(uint32_t);

	char* tamPathStream = intToStream(tamPath);
	if(sendall(idFS,tamPathStream, &tamuint) < 0){
		printf("11\n");
		log_error(logger,error);
		return 0;
	}
	free(tamPathStream);

	if(sendall(idFS,path, &tamPath) < 0){
		printf("2\n");
		log_error(logger,error);
		return 0;
	}
	// Recibo confirmacion
	if(recv(idFS,rev,1,MSG_WAITALL) <= 0){
		log_error(logger,error);
		return 0;
	}

	if(rev[0]== 'Y'){
		return 1;
	}
	return 0;
}

char* continuacionPeticionLectura(uint32_t cpu,uint32_t offset,uint32_t size,char*error){
	uint32_t tamARecibir=0;
	char * rev = malloc(1);

	package_t pk=serializar(4,
			sizeof(uint32_t),&offset,
			sizeof(uint32_t),&size
			);

	if(sendall(idFS,pk.data, &pk.data_size) < 0){
		log_error(logger,error);
		return 0;
	}

	// Recibo confirmacion
	if(recv(idFS,rev,1,0) <= 0){
		log_error(logger,error);
		return NULL;
	}
	//test("Entro a continuacion1");
	tamARecibir=size;
	if(rev[0] == 'Y'){
		rev=realloc(rev,tamARecibir);
		memset(rev,'\0',tamARecibir);

		// Recibo el data
		if(recv(idFS,rev,tamARecibir,MSG_WAITALL) <= 0){
			log_error(logger,error);
			return NULL;
		}

		return rev;
	}
	printf("Respuesta del FS: %s\n",rev);

	return NULL;
}

bool continuacionPeticionEscritura(uint32_t cpu,uint32_t offset,uint32_t size,char*buffer,char*error){
		//test("Continuacion peticion escritura");
		char * rev = NULL;
		uint32_t tamAMandar=sizeof(uint32_t);

		printf("OFFSET: %i SIZE: %i\n",offset,size);
		package_t pk=serializar(4,
				sizeof(uint32_t),&offset,
				sizeof(uint32_t),&size
				);

		if(sendall(idFS,pk.data, &pk.data_size) < 0){
			log_error(logger,error);
			return 0;
		}

		// Mando largo del buffer
		char* tamBufferStream = intToStream(size);
		if(sendall(idFS,tamBufferStream,&tamAMandar) < 0){
			log_error(logger,error);
			//test("ASD");
			return 0;
		}
		free(tamBufferStream);

		// Mando buffer
		if(sendall(idFS,buffer,&size) < 0){
			log_error(logger,error);
			//test("ASDD");
			return 0;
		}

		// Recibo confirmacion
		rev = malloc(1);
		if(recv(idFS,rev,1,0) < 0){
			log_error(logger,error);
			//test("ASDDD");
			return 0;
		} else {
			//printf("Respuesta del FS: %s\n",rev);
		}

		if(rev[0] == 'Y')
			return 1;
		else
			return 0;
}

bool borrarArchivo(char* path,uint32_t tamPath){
	return mandarOperacionFS("B",path,tamPath,"Ocurrio un error al borrar un archivo");
}

bool crearArchivo(char* path,uint32_t tamPath){
	return mandarOperacionFS("C",path,tamPath,"Ocurrio un error al crear un archivo");
}

char* leerArchivo(char*path,uint32_t tamPath, uint32_t i,uint32_t offset,uint32_t size){
	//test("Entro a leer Archivo");
	if(mandarOperacionFS("L",path,tamPath,"Ocurrio un error al leer un archivo") == 0){
		log_error(logger,"No se pudo leer archivo.");
		return NULL;
	}
	return continuacionPeticionLectura(i,offset,size,"Ocurrio un error al leer un archivo");
}

bool escribirArchivo(char* path,uint32_t tamPath,uint32_t i,uint32_t offset,uint32_t size,char*data){
	if(mandarOperacionFS("E",path,tamPath,"Ocurrio un error al escribir un archivo") == 0){
		log_error(logger,"No se pudo escribir un archivo.");
		return 0;
	}
	if(continuacionPeticionEscritura(i,offset,size,data,"Ocurrio un error al escribir un archivo")){
		log_trace(logger,"Se pudo escribir correctamente el archivo");
		return 1;
	}else{
		log_error(logger,"No se pudo escribir un archivo.");
		return 0;
	}
}

bool validarArchivo(char* path,uint32_t tamPath){
	return mandarOperacionFS("V",path,tamPath,"Ocurrio un error al validar un FD");
}


////////////////// FIN Funciones que interactuan con el FS

void imprimirPorConsola(uint32_t socket,char*data,uint32_t tamanio){
	uint32_t uno = 1;
	uint32_t tam=4;
	if(sendall(socket,"P",&uno) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
	if(sendall(socket,(char*)&tamanio,&tam) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
	if(sendall(socket,data,&tamanio) < 0)
		log_trace(logger,"Ocurrio en error al tratar de imprimir data por la consola");
}

uint32_t abrirFD(uint32_t i,t_programa* unPrograma){
	log_trace(logger,"Llamada a ABRIR FD");
	char* path = recibirPath(i);
	char* permisos = recibirPermisos(i);

	if(path == NULL || permisos == NULL){
		send(i,"N",1,0);
		printf("path: %p, permisos: %p\n",path,permisos);
		log_error(logger,"Path NULL o permisos NULL");
		return 9999;
	}
	if(!validarArchivo(path,strlen(path)) && !tienePermisos('c',permisos)){
		send(i,"N",1,0);
		unPrograma->pcb->exitCode = -2;
		log_error(logger,"El archivo no existe y no hay permisos para crear un nuevo archivo");
		return 9999;
	}
	unPrograma->FDCounter++;
	t_entradaTAP * nuevaEntradaTAP = malloc(sizeof(t_entradaTAP));
	nuevaEntradaTAP->flags = permisos;
	nuevaEntradaTAP->cursor = 0;

	t_entradaTGA * nuevaEntradaTGA = buscarFDPorPath(path);
	if(nuevaEntradaTGA != NULL){
		nuevaEntradaTGA->abierto++;
		nuevaEntradaTGA->archivo = path;
	} else {
		if(!validarArchivo(path,strlen(path))){
			if(crearArchivo(path,strlen(path))){

			} else {
				send(i,"N",1,0);
				log_error(logger,"No se pudo crear el nuevo archivo");
				return 9999;
			}
		}

		nuevaEntradaTGA = malloc(sizeof(t_entradaTGA));
		nuevaEntradaTGA->archivo = path;
		nuevaEntradaTGA->abierto = 1;
		nuevaEntradaTGA->indice = GlobalFDCounter;
		list_add(tablaGlobalArchivos,nuevaEntradaTGA);

	}

	nuevaEntradaTAP->globalFD = nuevaEntradaTGA->indice;
	nuevaEntradaTAP->indice = unPrograma->FDCounter;
	list_add(unPrograma->tablaArchivosPrograma,nuevaEntradaTAP);

	GlobalFDCounter++;
	log_trace(logger,"Nuevo FD creado");

	send(i,"Y",1,0);
	send(i,&nuevaEntradaTAP->indice,sizeof(uint32_t),0);
	log_trace(logger,"Se creo un archivo con exito");
	return nuevaEntradaTAP->indice;
}

bool borrarFD(uint32_t i,t_programa* unPrograma){
	uint32_t fd = 0;
	recv(i,&fd,sizeof(uint32_t),MSG_WAITALL);
	//send(i,"Y",1,0);
/*
	char* path = recibirPath(i);
	if(path == NULL){
		return 0;
	}

	if(validarArchivo(path,strlen(path)) == 0){
		send(i,"N",1,0);
		log_error(logger,"No existe el archivo que se quiere borrar");
		return 0;
	}
*/
	uint32_t auxFDTAP = buscarFDArchivoPorId(fd,unPrograma);
	t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,auxFDTAP);
	uint32_t indiceGlobalFD = entradaFD->globalFD;
	printf("FD: %i, indiceGlobalFD: %i\n",fd,indiceGlobalFD);
	if(indiceGlobalFD != 9999){
		bool _condicion3(t_entradaTGA* self){
			return self->indice==indiceGlobalFD;
		}
		void _destroyer3(t_entradaTGA* self){
			free(self);
		}
		//TODO cond
		t_entradaTGA * aux = list_find(tablaGlobalArchivos,(void*)_condicion3);
		if(aux->abierto == 1){
			if(borrarArchivo(aux->archivo,strlen(aux->archivo))){
				/*bool _condicion(t_entradaTAP* self){
					return self->indice==fd;
				}
				void _destroyer(t_entradaTAP* self){
					free(self);
				}
				list_remove_and_destroy_by_condition(unPrograma->tablaArchivosPrograma,(void*)_condicion,(void*)_destroyer);
				//list_remove(unPrograma->tablaArchivosPrograma,fd);

				bool _condicion2(t_entradaTGA* self){
					return self->indice==indiceGlobalFD;
				}
				void _destroyer2(t_entradaTGA* self){
					free(self);
				}
				list_remove_and_destroy_by_condition(tablaGlobalArchivos,(void*)_condicion2,(void*)_destroyer2);
				//list_remove(tablaGlobalArchivos,indiceGlobalFD);
				//ist_remove(unPrograma->tablaArchivosPrograma,buscarFDArchivoPorId(fd,unPrograma));
				*/
				log_trace(logger,"Se borro el FD correctamente");
				send(i,"Y",1,0);
				return 1;
			} else {
				// mandar proceso a exit recibiendo pcb de cpu
				log_trace(logger,"No se pudo borrar el archivo correctamente");
				send(i,"N",1,0);
				return 0;
			}
		}else{
			log_trace(logger,"No se pudo borrar el archivo, otro lo tiene abierto");
			send(i,"N",1,0);
		}
	}
	log_trace(logger,"No se pudo borrar el archivo correctamente.");
	send(i,"N",1,0);
	return 0;
}

bool cerrarFD(uint32_t i, t_programa* unPrograma){
	uint32_t fd = 0;
	recv(i,&fd,sizeof(fd),MSG_WAITALL);
	uint32_t auxFDTAP = buscarFDArchivoPorId(fd,unPrograma);
	t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,auxFDTAP);
	uint32_t indiceGlobalFD = entradaFD->globalFD;
	if(indiceGlobalFD != 9999){
		list_remove(unPrograma->tablaArchivosPrograma,auxFDTAP);//ojo este fd es un puntero que no es absoluto
		bool _condicion3(t_entradaTGA* self){
			return self->indice==indiceGlobalFD;
		}
		//TODO cond
		t_entradaTGA * aux = list_find(tablaGlobalArchivos,(void*)_condicion3);

		if(aux->abierto == 1){
			list_remove_by_condition(tablaGlobalArchivos,(void*)_condicion3);
		} else {
			aux->abierto--;
		}
		list_remove(unPrograma->tablaArchivosPrograma,buscarFDArchivoPorId(fd,unPrograma));
		log_trace(logger,"FD cerrado");
		send(i,"Y",1,0);
		return 1;
	}else{
		log_trace(logger,"Se intento cerrar un FD que no existia");
		send(i,"N",1,0);
		return 0;
	}
}

bool escribirFD(uint32_t i,t_programa* unPrograma){
	log_trace(logger,"Dentro de la funcion escribirFD");
	uint32_t fd = 0;
	recv(i,&fd,sizeof(uint32_t),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t tamanio = 0;
	recv(i,&tamanio,sizeof(uint32_t),MSG_WAITALL);
	send(i,"Y",1,0);

	char* data = NULL;
	data=realloc(data,tamanio+1);
	memset(data,'\0',tamanio+1);

	recv(i,data,tamanio,0);
	send(i,"Y",1,0);

	int contador = 0;
	/*textoRojo("TESTEANDO");
	while(tamanio > contador){
		printf("%c\n",data[contador]);
		contador++;
	}
	textoRojo("FIN TESTEANDO");*/

	/*char* path = recibirPath(i);
	char* permisos = recibirPermisos(i);
	if(path == NULL || permisos == NULL){
		send(i,"N",1,0);
		return 0;
	}*/

	if(fd == 1){
		imprimirPorConsola(unPrograma->id,data,tamanio);
		send(i,"Y",1,0);
		return 0;
	}

	uint32_t auxFDTAP = buscarFDArchivoPorId(fd,unPrograma);
	t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,auxFDTAP);
	uint32_t indiceGlobalFD = entradaFD->globalFD;
	bool _condicion3(t_entradaTGA* self){
		return self->indice==indiceGlobalFD;
	}
	void _destroyer3(t_entradaTGA* self){
		free(self);
	}
	//TODO cond
	t_entradaTGA * aux = list_find(tablaGlobalArchivos,(void*)_condicion3);

	if(aux == NULL){
		unPrograma->pcb->exitCode = -2;
		log_error(logger,"Se trato de escribir un archivo que no ha sido abierto");
		return NULL;
	}

	/*if(validarArchivo(aux->archivo,strlen(aux->archivo)) == 0 || !tienePermisos('e',entradaFD->flags)){
		send(i,"N",1,0);
		log_error(logger,"No hay permisos para crear un nuevo archivo o el archivo no existe");
		return 0;
	}*/

	if(auxFDTAP != 9999 || fd == 1){

		if(tienePermisos('e',entradaFD->flags)){
				if(escribirArchivo(aux->archivo,strlen(aux->archivo),i,entradaFD->cursor,tamanio,data)){
					log_trace(logger,"Se escribio un archivo correctamente");
					log_trace(logger,aux->archivo);
					send(i,"Y",1,0);
					return 1;
				}else{
					log_error(logger,"No se pudo escribir un archivo correctamente");
					send(i,"N",1,0);
					return 0;
				}
		} else {
			log_error(logger,"No hay permisos para escribir en un archivo");
			unPrograma->pcb->exitCode = -4;
			send(i,"N",1,0);
			return 0;
		}
	}
	send(i,"N",1,0);
	return 0;
}

char* leerFD(uint32_t i,t_programa* unPrograma){
	log_trace(logger,"Dentro de la funcion leerFD");
	uint32_t fd = 0;
	recv(i,&fd,sizeof(uint32_t),MSG_WAITALL);
	send(i,"Y",1,0);

	uint32_t tamanio = 0;
	recv(i,&tamanio,sizeof(uint32_t),MSG_WAITALL);
	send(i,"Y",1,0);

	/*char* path = recibirPath(i);
	char* permisos = recibirPermisos(i);
	if(path == NULL || permisos == NULL){
		send(i,"N",1,0);
		return 0;
	}*/

	uint32_t auxFDTAP = buscarFDArchivoPorId(fd,unPrograma);
	t_entradaTAP* entradaFD = list_get(unPrograma->tablaArchivosPrograma,auxFDTAP);

	uint32_t indiceGlobalFD = entradaFD->globalFD;
	bool _condicion3(t_entradaTGA* self){
		return self->indice==indiceGlobalFD;
	}
	void _destroyer3(t_entradaTGA* self){
		free(self);
	}
	//TODO cond
	t_entradaTGA * aux = list_find(tablaGlobalArchivos,(void*)_condicion3);

	if(aux == NULL){
		unPrograma->pcb->exitCode = -2;
		log_error(logger,"Se trato de leer un archivo que no ha sido abierto");
		return NULL;
	}

	//printf("Flags: %s\n",entradaFD->flags);
	//printf("Validar archivo %i|| !tienePermisos %i\n",validarArchivo(aux->archivo,strlen(aux->archivo))==0,!tienePermisos('l',entradaFD->flags));
	if(validarArchivo(aux->archivo,strlen(aux->archivo)) == 0 || !tienePermisos('l',entradaFD->flags)){
		send(i,"N",1,0);
		unPrograma->pcb->exitCode = -3;
		log_error(logger,"No hay permisos para crear un nuevo archivo o el archivo no existe");
		return NULL;
	}

	if(auxFDTAP != 9999){
		if(tienePermisos('l',entradaFD->flags)){
			char* contenido = leerArchivo(aux->archivo,strlen(aux->archivo),i,entradaFD->cursor,tamanio);
			if(contenido != NULL){
				log_trace(logger,"Se leyo un archivo correctamente");
				log_trace(logger,aux->archivo);
				send(i,"Y",1,0);
				return contenido;
			}else{
				log_error(logger,"No se pudo leer un archivo correctamente");
				send(i,"N",1,0);
				return NULL;
			}
		} else {
			log_error(logger,"No hay permisos para leer un archivo");
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
		if(strcmp(aux->archivo,path) == 0){
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
		printf("Comparando %i con ID %i\n",aux->indice,id);
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
		//printf("  							1\n");
		if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
			log_error(logger,"Ocurrio un problema al abrir un FD");
		//printf("  							1\n");
		if(send(i,"Y",1,0) < 0)
			log_error(logger,"Ocurrio un problema al abrir un FD");
		rev=realloc(rev,tamARecibir+1);
		memset(rev,'\0',tamARecibir+1);
		// Recibo el nombre del semaforo
		if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
			log_error(logger,"Ocurrio un problema al abrir un FD");
		send(i,"Y",1,0);
		return rev;
}

char* recibirPermisos(uint32_t i){
	uint32_t tamARecibir=0;
	char * rev = NULL;
	//char * permisosRev = NULL;

	// Recibo longitud permisos
	if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
		log_error(logger,"No se pudieron recibir los permisos");
	if(send(i,"Y",1,0) < 0)
		log_error(logger,"No se pudieron recibir los permisos");

	rev=realloc(rev,tamARecibir+1);
	memset(rev,'\0',tamARecibir+1);

	// Recibo permisos
	if(recv(i,rev,tamARecibir,MSG_WAITALL) <= 0)
		log_error(logger,"No se pudieron recibir los permisos");
	send(i,"Y",1,0);

	return rev;

}
