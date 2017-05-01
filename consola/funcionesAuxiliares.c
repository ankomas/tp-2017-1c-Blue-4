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
 * Funcion que cierra un socket(file descriptor) y
 *
 * devuelve este socket cerrado.
 *
 * @param socket
 * @return socketCerrado (-1)
 */

int cerrarSocket(int socket)
{
	close(socket);
	socket = -1;
	return socket;
}




/**
 * Gestiona el manejo de las conexiones con el kernel cada ves que se invoca y
 *
 * queda a la escucha de mensajes del kernel
 *
 * @param dataHilo
 */

void gestionarProgramaAnsisop(dataHilos_t dataHilo)
{
	// TODO serializar!!!
	//int tamanio_programa = strlen(pathPrograma);
	// pido el PID al Kernel
	//int socket_kernel=dataHilo.socket;
	char* pathPrpgrama=dataHilo.path;
	uint32_t tamanio=1;
	int envio=sendall(socket_kernel,"A",&tamanio);
	if(envio<0)
	{
		printf("fallo el envio del mensaje \n");
		return;
	}

	// recibir el PID del Kernel
	char* pid_programa = malloc(4);
	memset(pid_programa,'\0',4);

	recv(socket_kernel,pid_programa,4,MSG_WAITALL);

	if(strcmp(pid_programa,"N")==0)
	{
		textoEnColor("El programa NO puede ser iniciado \n",0,0);
		// TODO averiguar si esta es la forma correcta de eliminar los recursos de este hilo si no pudo ser creado!!!
		free(pid_programa);
		close(socket_kernel);
		pthread_exit(NULL);
		return;
	}
	printf("%s PROGRAMA CREADO CON PID DE : %s %s \n",KCYN,pid_programa,KNRM);

	//sleep(100);
	// recibir un mensaje del kernel y validar que sea el PID correspondiente al programa enviado por este hilo

	while(1)
	{
		if(socket_kernel<0)
		{
			pthread_exit(NULL);
		}
	}


}

