/*
 * operacionesDeConsola.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include <pthread.h>
#include "conexiones.h"
#include "hilos.h"
#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>


#define id_kernel 2



void gestionarProgramaAnsisop(char* pathPrograma)
{
	// TODO serializar!!!
	//int tamanio_programa = strlen(pathPrograma);
	// pido el PID al Kernel
	uint32_t tamanio=1;
	send(socket_kernel,"A",1,NULL);
	//sendall(socket_kernel,pathPrograma,tamanio_programa);

	// recibir el PID del Kernel
	char* pid_programa = malloc(4);
	memset(pid_programa,'\0',4);

	recv(socket_kernel,pid_programa,4,MSG_WAITALL);
	test(pid_programa);

	//printf("el pid recibido es : %s \n",pid_programa);


	// recibir un mensaje del kernel y validar que sea el PID correspondiente al programa enviado por este hilo
/*
	while(1)
	{

	}

*/

}



void iniciarProgramaAnsisop(char* pathProgramaAsisop)
{
	pthread_t hiloPrograma;
	crearHiloPrograma(pathProgramaAsisop);
	//sendall();
}



void finalizarPrograma(int id)
{

}




void desconectarConsola()
{

}



void limpiarMensajes()
{
	system("clear");
}



void mostrarDatosDelConfig()
{
	datosConfig_t datos = obtenerEstructurasDelConfig();
	printf("La IP usada es : %s \n",datos.ip);
	printf("El PUERTO usado es: %d \n\n",datos.puerto_kernel);
}


int crearMenuDeConexion()
{
	int opcion,socket_cliente;

	do{
	printf("BIENVENIDO A LA CONSOLA \n");
	printf("PUEDE ELEGIR DOS OPCIONES \n\n");
	printf("1-PARA CONECTARSE A UN NUCLEO \n");
	printf("2-MOSTRAR DATOS USADOS EN EL CONFIG \n");
	printf("3-PARA SALIR \n\n");
	printf("OPCION A ELEGIR:");
	scanf("%d",&opcion);

	switch(opcion)
	{
	case 1:
		printf("Conectando al Kernel..... \n\n");
		socket_cliente=conectarseAlKernel(id_kernel);
		if(socket_cliente<0)
			return -2;
		system("clear");
		getchar();
		return socket_cliente;
	case 2:
		mostrarDatosDelConfig();
		getchar();
		break;
	case 3:
		return -1;
	default:
		system("clear");
		getchar();
		break;
	}
	}while(opcion!=3 && opcion!=1);

	return -2;
}







void crearMenuPrincipal()
{
	int opcion,id;
	char* rutaPrograma=NULL;

	do{

		printf("BIENVENIDO AL MENU PRINCIPAL DE LA CONSOLA \n");
		printf("ELIJA UNA OPCION \n\n");
		printf("1-INICIAR PROGRAMA \n");
		printf("2-FINALIZAR PROGRAMA \n");
		printf("3-DESCONECTAR CONSOLA \n");
		printf("4-LIMPIAR MENSAJES \n\n");

		printf("OPCION A ELEGIR:");
		scanf("%d",&opcion);

		switch(opcion)
			{
			case 1:
				printf("Ingrese programa ANSISOP ");
				scanf("%s",rutaPrograma);
				printf("Iniciando programa ANSISOP..... \n\n");
				//crearHiloPrograma(rutaPrograma);
				iniciarProgramaAnsisop(rutaPrograma);
				getchar();
				break;
			case 2:
				printf("ingrese el id del programa que quiere finalizar ");
				scanf("%d",&id);
				finalizarPrograma(id);
				getchar();
				break;
			case 3:
				printf("Desconectando Consola..... \n\n");
				desconectarConsola();
				getchar();
				break;
			case 4:
				limpiarMensajes();
				getchar();
				break;
			default:
				printf("Opcion ingresada invalida \n\n");
				getchar();
				break;
			}
	}while(opcion!=3);
}


