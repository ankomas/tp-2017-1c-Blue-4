/*
 * operacionesDeConsola.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */
#include "conexiones.h"
#include "hilos.h"
#include  "operacionesDeConsola.h"
#include "funcionesAuxiliares.h"
#include "unistd.h"

/**
 * Es el que inicia una conexion ,envio de programas ANSISOP y queda a la espera de mensajes del proceso KERNEL
 *
 * @param pathProgramaAnsisop (ruta del programa a utillizar)
 */
void iniciarProgramaAnsisop(char* pathProgramaAsisop)
{
	//pthread_t hiloPrograma;
	//conectarseAlKernel();
	printf("la ruta es : %s \n",pathProgramaAsisop);
	//leerProgramaAnsisop(pathProgramaAsisop);
	crearHiloPrograma(pathProgramaAsisop);
	//sendall();
}



void finalizarPrograma(int id)
{
	//void * estado;

	//printf("el size de la lista es : %d \n",list_size(dataDeHilos));
	//pthread_mutex_lock(&mutexDataDeHilos);// QUIZA NO VALLAN ESTOS MUTEX!!!
	dataHilos_t* data = buscaHiloPorPid(id);
	//pthread_mutex_unlock(&mutexDataDeHilos);

	if(data==NULL)
	{
		textoEnColor("el ID ingresado no es valido",0,0);
		return;
	}
	textoEnColor("El id Buscado es ",(*data).pidHilo,1);
	//pthread_mutex_lock(&mutexAListas);

	eliminarHiloYrecursos(data);
	//pthread_mutex_unlock(&mutexAListas);
	//eliminarHiloDeListaPorPid(data->pidHilo);
	//printf("nuevo tam antes de cerrar 2: %d \n",list_size(dataDeHilos));

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


void menuDeControl()
{
	int opcion;

	do{
	printf("BIENVENIDO A LA CONSOLA \n");
	printf("PUEDE ELEGIR UNA DE ESTAS OPCIONES \n\n");
	printf("1-IR AL MENU PRINCIPAL DE LA CONSOLA \n");
	printf("2-MOSTRAR DATOS USADOS EN EL CONFIG \n");
	printf("3-PARA SALIR \n\n");
	printf("OPCION A ELEGIR: \n");
	scanf("%d",&opcion);

	switch(opcion)
	{
	case 1:
		/*
		printf("Conectando al Kernel..... \n\n");
		socket_cliente=conectarseAlKernel();
		if(socket_cliente<0)
			return -2;
		system("clear");
		getchar();
		return socket_cliente;
		*/
		getchar();
		break;
	case 2:
		mostrarDatosDelConfig();
		getchar();
		break;
	case 3:
		break;
	default:
		system("clear");
		getchar();
		break;
	}
	}while(opcion!=3 && opcion!=1);

}







void crearMenuPrincipal()
{
	int opcion,id;
	char rutaPrograma[100];

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


