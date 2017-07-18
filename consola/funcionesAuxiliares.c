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




dataProceso_t* buscaInfoProcesoPorPid(int pidBuscado)
{
	dataProceso_t *info;
	//pthread_mutex_lock(&mutexAListas);
	pthread_mutex_lock(&mutexDataDeProcesos);
	int esIgualA(dataProceso_t* unaInfo)
	{
		//printf("en la funcion que filtro el pid es : %d \n",(*dataHilo).pidHilo);
		//printf("en la funcion que filtro el socket es : %d \n",(*dataHilo).socketKernel);
		return (*unaInfo).PID == pidBuscado;
	}

    info = list_find(dataDeProcesos,(void*) esIgualA);
	pthread_mutex_unlock(&mutexDataDeProcesos);
	//pthread_mutex_unlock(&mutexAListas);
	return info;
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
		//printf("en la funcion que filtro el pid es : %d \n",(*data).pidHilo);
		//printf("en la funcion que filtro el socket es : %d \n",(*data).socketKernel);
		return (*data).pidHilo==pid;
	}
	//printf("El pid que tengo que eliminar es : %d \n",pid);
	//pthread_mutex_lock(&mutexDataDeHilos);
	//printf("hiloRemovido->pid : %d\n",(*hiloRemovido).pidHilo);
	//printf("hiloRemovido->socket : %d \n",(*hiloRemovido).socketKernel);
    list_remove_by_condition(dataDeHilos,(void*)esIgual);
	//printf("tam de lista en funcion eliminarHilo es : %d \n",list_size(dataDeHilos));

	pthread_mutex_unlock(&mutexDataDeHilos);

	return hiloRemovido;
	//return list_remove_by_condition(dataDeHilos,(void*)esIgual);
}






dataProceso_t* eliminarProcesoDeListaPorPid(int pid)
{
	dataProceso_t* procesoRemovido;
	procesoRemovido=buscaInfoProcesoPorPid(pid);
	pthread_mutex_lock(&mutexDataDeProcesos);
	int esIgual(dataProceso_t* data)
	{
		//printf("en la funcion que filtro el pid es : %d \n",(*data).pidHilo);
		//printf("en la funcion que filtro el socket es : %d \n",(*data).socketKernel);
		return (*data).PID==pid;
	}
	//printf("El pid que tengo que eliminar es : %d \n",pid);
	//pthread_mutex_lock(&mutexDataDeHilos);
	//printf("hiloRemovido->pid : %d\n",(*hiloRemovido).pidHilo);
	//printf("hiloRemovido->socket : %d \n",(*hiloRemovido).socketKernel);
    list_remove_by_condition(dataDeProcesos,(void*)esIgual);
	//printf("tam de lista en funcion eliminarHilo es : %d \n",list_size(dataDeHilos));

	pthread_mutex_unlock(&mutexDataDeProcesos);

	return procesoRemovido;
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
	//printf("pid en agregarDataHilo es: %d \n",(dataHilo)->pidHilo);
	pthread_mutex_unlock(&mutexDataDeHilos);

}


void agregarDataDeProceso(dataProceso_t* dataProceso)
{
	pthread_mutex_lock(&mutexDataDeProcesos);
		list_add(dataDeProcesos,dataProceso);
	pthread_mutex_unlock(&mutexDataDeProcesos);
}

/**
 * Funcion que cierra un socket(file descriptor) y
 *
 *elimina la info del hilo de la lista de dataHilos
 *
 * @param socket
 */


//TODO hacer que se pude usar para reutilizar codigo en gestionarPrograma
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

		pthread_cancel(hiloAEliminar->hilo);
		eliminarRecursos(hiloAEliminar);
		printf("el tamaño de la lista al terminar el hilo es : %d \n",list_size(dataDeHilos));

}


/**
 * Funcion que sirve para leer el contenido de un archivo al cual,
 *
 * su ruta sea especificada por parametro.
 *
 * Nota: liberar el string que retorna la funcion!!!
 *
 * @param ruta_del_archivo
 * @return contenido_leido
 */
char* leerProgramaAnsisop(char* ruta,int* tamanio)
{

	FILE* programa;
	char* contenido;/*malloc(200);*/
	int size;

	//printf("ruta en leerPrograma : %s \n",ruta);

	programa = fopen(ruta,"r");
	while(programa==NULL)
	{
		textoEnColor("Ruta invalida del programa ",0,0);
		programa = fopen(ruta,"r");
		scanf("%s",ruta);
	}

	fseek(programa, 0L, SEEK_END);
	size = ftell(programa);
	rewind(programa);

	*tamanio=size;
	contenido=calloc(1,size);

	while(!feof(programa))
	{
		fread(contenido,size,1,programa);
	}

	//printf("el contenido es : \n\n%s \n",contenido);
	//printf("el tamaño del contenido es : \n\n %d \n",strlen(contenido));
	return contenido;
}




/**
 * Gestiona el manejo de las conexiones con el kernel cada ves que se invoca y
 *
 * queda a la escucha de mensajes del kernel
 *
 * @param dataHilo
 */


int enviarMensajeConCodigoDeOperacion(char codOp[1],int socket,char* contenido,uint32_t tamContenido)
{
	uint32_t tamanio=1;
		int envio=sendall(socket,codOp,&tamanio);
		if(envio<0){
			textoEnColor("fallo el envio del mensaje ",0,0);
			return -1;
		}

		tamanio= sizeof(uint32_t);
		char*stream = intToStream(tamContenido);
		envio=sendall(socket,stream,&tamanio);
		free(stream);
		if(envio<0){
			printf("fallo el envio del mensaje \n");
			return -1;
		}

		tamanio=tamContenido;
		envio=sendall(socket,contenido,&tamanio);
		if(envio<0){
			printf("fallo el envio del mensaje \n");
			return -1;
		}

		return 0;
}



void listarFechaYhora(time_t tiempo,char* mensaje)
{
	struct tm *tlocal = localtime(&(tiempo));
	char inicio[128];
	strftime(inicio,128,"%d/%m/%y %H:%M:%S",tlocal);
	textoAmarillo(mensaje);
	textoAmarillo(inicio);
}

void listarInfoArchivo(dataProceso_t* infoProceso,int pid)
{

	textoAmarillo("\n INFORMACION DEL PROCESO: ");
	char* pid_string=string_itoa(pid);
	textoAmarillo(pid_string);
	free(pid_string);
	listarFechaYhora(infoProceso->fecha_Y_hora_DeInicio,"\n La fecha y hora de inicio es :");

	infoProceso->fecha_Y_hora_DeFin=time(0);
	listarFechaYhora(infoProceso->fecha_Y_hora_DeFin," \n La fecha y hora de fin es :");

	textoAmarillo(" \n Cantidad de impresiones por pantala: ");
	char* num_string=string_itoa(infoProceso->impresionesPorPantalla);
	textoAmarillo(num_string);
	free(num_string);

	infoProceso->tiempoTotalDeEjecucion=infoProceso->fecha_Y_hora_DeFin - infoProceso->fecha_Y_hora_DeInicio;
	textoAmarillo("\n Tiempo total de ejecucion: ");
	printf("%s %2f %s \n",KYEL,(double)infoProceso->tiempoTotalDeEjecucion,KNRM);

}


void imprimirFinalizacionDeProceso(dataHilos_t* data)
{
	dataHilos_t* dataAEliminar = eliminarHiloDeListaPorPid(data->pidHilo);
	dataProceso_t* infoProceso=eliminarProcesoDeListaPorPid(data->pidHilo);

	pthread_mutex_lock(&mutexDataDeProcesos);
	listarInfoArchivo(infoProceso,data->pidHilo);
	free(infoProceso);
	free(dataAEliminar->path);
	close(dataAEliminar->socketKernel);
	free(dataAEliminar);
	pthread_mutex_unlock(&mutexDataDeProcesos);
	pthread_exit(NULL);

}





void imprimirSentenciaAnsisop(dataHilos_t* data)
{
	char* sentencia=recibirSentenciaAnsisop(data->socketKernel);
	dataProceso_t* infoProceso=eliminarProcesoDeListaPorPid(data->pidHilo);

	if(infoProceso==NULL){
		textoAzul("SOY NULL \n");
		return;
	}
	if(sentencia)
	{
		pthread_mutex_lock(&mutexDataDeProcesos);
		textoAmarillo("\n EL PROCESO:");
		char* num_string=string_itoa(infoProceso->PID);
		textoAmarillo(num_string);
		textoAmarillo("\n IMPRIME:");
		textoAmarillo(sentencia);
		free(num_string);
		free(sentencia);
		infoProceso->impresionesPorPantalla=infoProceso->impresionesPorPantalla+1;
		pthread_mutex_unlock(&mutexDataDeProcesos);
		agregarDataDeProceso(infoProceso);
		return;

	}
	printf("no se pudo recibir la sentencia \n");

}

void manejadorDeCodigosDeOperacion(char cop,dataHilos_t* data)
{
	if(cop=='F')
	{
		imprimirFinalizacionDeProceso(data);
	}
	else
	{
		imprimirSentenciaAnsisop(data);
	}
}


void gestionarProgramaAnsisop(dataHilos_t* dataHilo)
{
	char* pathPrograma=dataHilo->path;
	//printf("dataHilo.path: %s \n",dataHilo->path);
	int tamanio;
	char* lecturaDeProgramaAnsisop = leerProgramaAnsisop(pathPrograma,&tamanio);
	if(lecturaDeProgramaAnsisop==NULL)
	{
		free(dataHilo->path);
		close(dataHilo->socketKernel);
		free(dataHilo);
		pthread_exit(NULL);
		return;
	}
	printf("el tamanio es: %d \n",tamanio);
	int respuesta=enviarMensajeConCodigoDeOperacion("A",dataHilo->socketKernel,lecturaDeProgramaAnsisop,tamanio);
	if(respuesta<0)
	{
		free(dataHilo->path);
		close(dataHilo->socketKernel);
		free(dataHilo);
		pthread_exit(NULL);
		return;
	}
	char* pid_programa;


	pid_programa=recibirPid(dataHilo->socketKernel);
	printf("pid_programa: %s \n",pid_programa);
	if(pid_programa==NULL)
	{
		printf(" No se pudo recibir un pid \n");
		free(dataHilo->path);
		close(dataHilo->socketKernel);
		free(dataHilo);
		pthread_exit(NULL);
		return;
	}
	dataHilo->pidHilo= atoi(pid_programa);
    textoEnColor("PROGRAMA CREADO CON PID ",dataHilo->pidHilo,1);

	free(pid_programa);
	agregarDataHilo(dataHilo);
	//TODO liberar la estructura cuando ya no se necesite mas!!!
	dataProceso_t* dataProceso=malloc(sizeof(dataProceso_t));
	dataProceso->PID=dataHilo->pidHilo;
	dataProceso->fecha_Y_hora_DeInicio=time(0);
	dataProceso->impresionesPorPantalla=0;
	printf("impresionesPorPantalla inicial: %d \n",dataProceso->impresionesPorPantalla);
	agregarDataDeProceso(dataProceso);

	//dataHilos_t* hiloBuscado;


	char cop;

	while(1)
	{

		if(recv(dataHilo->socketKernel,&cop,1,0)==0){
			printf("se desconecto el kernel \n ");
			break;
		}
		manejadorDeCodigosDeOperacion(cop,dataHilo);

	}


}


