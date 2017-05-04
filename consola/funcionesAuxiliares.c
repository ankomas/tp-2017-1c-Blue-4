/*
 * funcionesAuxiliares.c
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#include "funcionesAuxiliares.h"
#include "operacionesDeConsola.h"
#include <blue4-lib.h>
#include "conexiones.h"
#include <stdlib.h>


/**
 * Puede imprimir un texto y 1 valor si el tercer parametro que le pasamos es un criterio,(numero), mayor a 0,
 * de lo contrario mostrara solo un string!!!.
 *
 * @param texto
 * @param variable
 * @param criterio
 **/
void textoEnColor(char* texto,int variable,int criterio)
{
	if(criterio<=0)
	{
	printf("%s  %s %s \n",KCYN,texto,KNRM);
	}
	else
	{
		printf("%s  %s: %d %s \n",KCYN,texto,variable,KNRM);

	}
}




/**
 *
 * Busca en una lista,(de hilos), un hilo segun el pid que venga por parametro
 *
 * @param pidBuscado
 * @return puntero a un dataHilos_t
 */

dataHilos_t* buscaHiloPorPid(int pidBuscado)
{
	dataHilos_t* hilo;
	//pthread_mutex_lock(&mutexAListas);
	pthread_mutex_lock(&mutexDataDeHilos);
	int esIgualA(dataHilos_t* dataHilo)
	{
		//printf("en la funcion que filtro el pid es : %d \n",(*dataHilo).pidHilo);
		//printf("en la funcion que filtro el socket es : %d \n",(*dataHilo).socketKernel);
		return (*dataHilo).pidHilo == pidBuscado;
	}

    hilo = list_find(dataDeHilos,(void*) esIgualA);
	pthread_mutex_unlock(&mutexDataDeHilos);
	//pthread_mutex_unlock(&mutexAListas);
	return hilo;
}


void destruirElemento(dataHilos_t* data)
{

}




dataHilos_t* eliminarHiloDeListaPorPid(int pid)
{
	dataHilos_t* hiloRemovido;
	hiloRemovido=buscaHiloPorPid(pid);
	pthread_mutex_lock(&mutexDataDeHilos);
	int esIgual(dataHilos_t* data)
	{
		printf("en la funcion que filtro el pid es : %d \n",(*data).pidHilo);
		printf("en la funcion que filtro el socket es : %d \n",(*data).socketKernel);
		return (*data).pidHilo==pid;
	}
	printf("El pid que tengo que eliminar es : %d \n",pid);
	//pthread_mutex_lock(&mutexDataDeHilos);
	//printf("hiloRemovido->pid : %d\n",(*hiloRemovido).pidHilo);
	//printf("hiloRemovido->socket : %d \n",(*hiloRemovido).socketKernel);
    list_remove_by_condition(dataDeHilos,(void*)esIgual);
	printf("tam de lista en funcion eliminarHilo es : %d \n",list_size(dataDeHilos));

	pthread_mutex_unlock(&mutexDataDeHilos);

	return hiloRemovido;
	//return list_remove_by_condition(dataDeHilos,(void*)esIgual);
}





/**
 * Agrega un dataHilo_t a una lista que sirve para poder administrar los datos de los hilos
 *
 * @param dataHilo
 */

void agregarDataHilo(dataHilos_t* dataHilo)
{
	pthread_mutex_lock(&mutexDataDeHilos);
	list_add(dataDeHilos,dataHilo);
	printf("pid en agregarDataHilo es: %d \n",(dataHilo)->pidHilo);
	pthread_mutex_unlock(&mutexDataDeHilos);

}




/**
 * Funcion que cierra un socket(file descriptor) y
 *
 *elimina la info del hilo de la lista de dataHilos
 *
 * @param socket
 */

void eliminarRecursos(dataHilos_t* datosDeHilo)
{
	close(datosDeHilo->socketKernel);
	//printf("size en cerrarSocket es : %d \n",list_size(dataDeHilos));

	dataHilos_t* hilo=eliminarHiloDeListaPorPid(datosDeHilo->pidHilo);
	free(hilo->path);
	free(hilo);
	//printf("size en cerrarSocket es : %d \n",list_size(dataDeHilos));
}



/**
 * Recibe el socket para recibir un pid que devuelve como char*
 *
 * sino lo puede recibir cerrara el hilo en ejecucion y su socket;
 *
 * @param socket
 * @return pid_en_string;
 */

char* recibirPid(int socket_kernel)
{
	// recibir el PID del Kernel
	char* pid_programa = malloc(4);
	memset(pid_programa,'\0',4);

	recv(socket_kernel,pid_programa,4,MSG_WAITALL);

	if(strcmp(pid_programa,"N")==0)
	{
		textoEnColor("El programa NO puede ser iniciado ",0,0);
		// TODO averiguar si esta es la forma correcta de eliminar los recursos de este hilo si no pudo ser creado!!!
		free(pid_programa);
		close(socket_kernel);
		pthread_exit(NULL);
		return NULL;
	}
	return pid_programa;
}









/**
 *
 * Elimina a un hilo y sus recursos,(por ejemplo un socket)
 *
 * @param hiloAEliminar
 */
void eliminarHiloYrecursos(dataHilos_t* hiloAEliminar)
{
	//pthread_mutex_lock(&mutexAListas);
		int resultado=pthread_attr_destroy(&hiloAEliminar->hiloPrograma);
		if(resultado)
		{
			textoEnColor("El programa NO finalizo exitosamente",0,0);
			return;
		}

		textoEnColor("El programa finalizo exitosamente",0,0);
		//eliminarHiloDeListaPorPid(hiloAEliminar.pidHilo);
		//printf("nuevo tam de la lista : %d \n",list_size(dataDeHilos));
		//cerrarSocket(hiloAEliminar);
		printf("el pid antes de cerrar en eliminarRecursos es: %d \n",hiloAEliminar->pidHilo);

		pthread_cancel(hiloAEliminar->hilo);
		eliminarRecursos(hiloAEliminar);
		printf("el tamaño de la lista al terminar el hilo es : %d \n",list_size(dataDeHilos));

		//sem_wait(&semaforo);

		//eliminarHiloDeListaPorPid(hiloAEliminar.pidHilo);
		//printf("tam de la lista antes de cerrar el hilo es: %d \n",list_size(dataDeHilos));
		//pthread_mutex_unlock(&mutexAListas);
		/*
		pthread_mutex_lock(&mutexAListas);
		eliminarHiloDeListaPorPid(hiloAEliminar.pidHilo);

		printf("nuevo tam antes de cerrar 2: %d \n",list_size(dataDeHilos));
		pthread_mutex_unlock(&mutexAListas);
		*/
		//socket_kernel=cerrarSocket(hiloAEliminar);

}


/**
 * Funcion que sirve para leer el contenido de un archivo al cual,
 *
 * su ruta sea especificada por parametro.
 *
 * @param ruta_del_archivo
 * @return contenido_leido
 */
char* leerProgramaAnsisop(char* ruta)
{

	FILE* programa;
	char* contenido=malloc(200);
	memset(contenido,'\0',200);
	printf("ruta en leerPrograma : %s \n",ruta);

	programa = fopen(ruta,"r");
	if(programa==NULL)
	{
		textoEnColor("Ruta invalida del programa ",0,0);
		return NULL;
	}

	while(!feof(programa))
	{
		fread(contenido,200,1,programa);
	}

	//printf("el contenido es : \n\n%s \n",contenido);
	//printf("el tamaño del contenido es : \n\n %d \n",strlen(contenido));
	return contenido;
}

/*
char* serializarMensaje(char* data,char* codigoDeOperacion)
{
	char* msg=string_new();
	int tamData;
	tamData= strlen(data);

}
char* serializar_msj(char* path,char codigoOp[1])
{
	char* msg;
	int offset=0;
	int tamPath;
	//char stringTam;

	tamPath=strlen(path);
	printf("el codigoDeOp es : %c\n",codigoOp);
	printf("la data es: %s \n",path);
	printf("el tam de la data es: %d \n",tamPath);

	msg=malloc(sizeof(int)+tamPath+sizeof(char));
	memset(msg,'\0',sizeof(int)+tamPath+sizeof(char));
	memcpy(msg,&codigoOp,sizeof(char));
	printf(" el mensaje serializado es: %s \n",msg+offset);
	offset=sizeof(char);
	printf(" offset es: %d \n",offset);
	memcpy(msg+offset,&tamPath,sizeof(int));
	printf(" el mensaje serializado es: %s \n",msg+offset);
	offset=offset+sizeof(int);
	memcpy(msg+offset,path,tamPath);
	printf(" el mensaje serializado es: %s \n",msg+offset);

	printf(" el mensaje serializado es: %s \n",msg+5);
	return msg;
}

*/






/**
 * Gestiona el manejo de las conexiones con el kernel cada ves que se invoca y
 *
 * queda a la escucha de mensajes del kernel
 *
 * @param dataHilo
 */

void gestionarProgramaAnsisop(dataHilos_t* dataHilo)
{
	// TODO serializar!!!
	//int tamanio_programa = strlen(pathPrograma);
	// pido el PID al Kernel
	//int socket_kernel=dataHilo.socket;
	char* pathPrograma=dataHilo->path;
	printf("dataHilo.path: %s \n",dataHilo->path);
	package_t mensaje;
	uint32_t tamPath=strlen(pathPrograma);
	printf("tam de path: %d \n",tamPath);
	mensaje=serializar(2,11,"ahora funca");
	printf("el mensaje enpaquetado es: %s \n",mensaje.data);
	printf("el tamaño del mensaje enpaquetado es: %d \n",mensaje.data_size);
	//char* lecturaDeProgramaAnsisop = leerProgramaAnsisop(pathPrograma);
	//printf("el paquete es: \n\n %s \n",mensaje);
	char* pid_programa;
	uint32_t tamanio=1;
	int envio=sendall(dataHilo->socketKernel,"A",&tamanio);
	if(envio<0)
	{
		printf("fallo el envio del mensaje \n");
		return;
	}

	pid_programa=recibirPid(dataHilo->socketKernel);
	dataHilo->pidHilo= atoi(pid_programa);

	textoEnColor("PROGRAMA CREADO CON PID ",dataHilo->pidHilo,1);

	free(pid_programa);
	agregarDataHilo(dataHilo);
	// recibir un mensaje del kernel y validar que sea el PID correspondiente al programa enviado por este hilo

	//dataHilos_t* hiloBuscado;



	while(1)
	{

		sleep(50);

		printf("NO FUNCIONA EL PTHREAD_CLOSE() \n");

	}


}


