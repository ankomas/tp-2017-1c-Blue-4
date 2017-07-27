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

	crearHiloPrograma(pathProgramaAsisop);
}



void finalizarPrograma(int id,char* cop)
{

	dataHilos_t* data = buscaHiloPorPid(id);

	if(data==NULL)
	{
		textoRojo(" \n El ID ingresado no es valido");
		return;
	}
	textoEnColor("El id Buscado es ",(*data).pidHilo,1);


	dataProceso_t* infoProceso=eliminarProcesoDeListaPorPid(data->pidHilo);
	pthread_mutex_lock(&mutexDataDeProcesos);
	listarInfoArchivo(infoProceso,data->pidHilo);
	pthread_mutex_unlock(&mutexDataDeProcesos);
	send(data->socketKernel,cop,1,0);
	eliminarHiloYrecursos(data);
}




void desconectarConsola()
{
	pthread_mutex_lock(&mutexDataDeHilos);
	textoAzul("\n DESCONECTANDO CONSOLA.....");
	int cant_procesos=list_size(dataDeHilos);
	printf("la cantidad de procesos a finalizar es: %d \n ",cant_procesos);
	pthread_mutex_unlock(&mutexDataDeHilos);
	if(cant_procesos<=0)
	{
		textoAzul(" \n .....CONSOLA DESCONECTADA");
		return;
	}
	int i=0;
	while(i<cant_procesos)
	{
		//printf("el i es : %d \n",i);
		pthread_mutex_lock(&mutexDataDeHilos);
		dataHilos_t* data=list_get(dataDeHilos,0);
		pthread_mutex_unlock(&mutexDataDeHilos);

		if(data)
		{
			finalizarPrograma(data->pidHilo,"F");
		}
		i++;
	}
	textoAzul(" \n .....CONSOLA DESCONECTADA");
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
				textoAzul("Ingrese programa ANSISOP: ");
				scanf("%s",rutaPrograma);
				char *rutaConcatenada = concat(3,rutaAbsoluta(),"scripts/",rutaPrograma);
				FILE*abrirArchivo = fopen(rutaConcatenada,"r");
				if(abrirArchivo == NULL){
					textoRojo("Ruta de programa invalida, por favor verifique la ruta del mismo\n");
				}else {
					textoAzul("Iniciando programa ANSISOP..... \n\n");
					fclose(abrirArchivo);
					iniciarProgramaAnsisop(rutaConcatenada);
				}
				free(rutaConcatenada);
				getchar();
				break;
			case 2:
				textoAzul("Ingrese el id del programa que quiere finalizar ");
				scanf("%d",&id);
				finalizarPrograma(id,"F");
				getchar();
				break;
			case 3:
				desconectarConsola();
				getchar();
				break;
			case 4:
				limpiarMensajes();
				getchar();
				break;
			default:
				textoRojo("Opcion ingresada invalida \n\n");
				getchar();
				break;
			}
	}while(opcion!=3);
}


