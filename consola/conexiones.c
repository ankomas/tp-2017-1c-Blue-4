/*
 * conexiones.c
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdint.h>
#include "conexiones.h"
#include "hilos.h"
#include "funcionesAuxiliares.h"
#include <blue4-lib.h>



struct data_socket
{
	int socket;
	struct sockaddr_in direccionServidor;
};

struct data_socket crearSocket(int puerto, char *ip)
{
	int miSocket;
	struct data_socket datosDelSocket;
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);
	miSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (miSocket < 0)
	{
		perror("no se pudo crear el socket ");
		exit(1);
	}
	datosDelSocket.socket = miSocket;
	datosDelSocket.direccionServidor = direccionServidor;
	return datosDelSocket;
}



int enviarMensaje(int socket,char* mensaje,int tamanioMensaje)
{
	//char mensaje[1000];
	//scanf("%s", mensaje);
	//memset(mensaje,'\0',1000);
	//memcpy(mensaje,mensajeAEnviar,tamanioMensajeAEnviar);
	//tamanioMensaje = strlen(mensaje);
	int bytesEnviados = send(socket, mensaje, tamanioMensaje, 0);
	int bytesRestantes = tamanioMensaje - bytesEnviados;
	int enviadosEnEsteCiclo;
	if(bytesEnviados <= 0)
		perror("no se envio el mensaje");
	while(bytesRestantes > 0)
	{
		enviadosEnEsteCiclo = send(socket, mensaje+bytesEnviados, bytesRestantes, 0);
		if(enviadosEnEsteCiclo == -1)
			break;
		bytesEnviados += enviadosEnEsteCiclo;
		bytesRestantes -= enviadosEnEsteCiclo;
	}
	return (bytesEnviados == tamanioMensaje);

}


int handshake(int socketCliente) {
	char *mensaje;
	int respuesta;
	send(socketCliente, "1", 1, 0);
	mensaje = malloc(1);
	recv(socketCliente, mensaje, 1, 0);

	//printf("Lo que me llego: %s\n", mensaje);
	respuesta=mensaje[0]-'0';
	free(mensaje);

	if(id_kernel!=respuesta){
			printf("Error: el id del proceso al que se conecto (%i) no es el requerido (%i)\n",respuesta,id_kernel);
			return -1;
		}

	return 0;
}


int conectar(int puerto, char *ip,int id)
{
	int respuesta_handshake;

	struct data_socket datosDelSocket = crearSocket(puerto, ip);
	int socketCliente = datosDelSocket.socket;
	struct sockaddr_in direccionServidor = datosDelSocket.direccionServidor;
	if (connect(socketCliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0)
	{
		perror("No se pudo conectar");
		return -1;
	}
	respuesta_handshake= handshake(socketCliente);
	if(respuesta_handshake<0)
		return -1;

	return socketCliente;
}



datosConfig_t obtenerEstructurasDelConfig()
{
	datosConfig_t datos;
	datos.ip = obtenerConfiguracionString(config,"IP_KERNEL");

	datos.puerto_kernel = obtenerConfiguracion(config,"PUERTO_KERNEL");
	return datos;
}




int conectarseAlKernel()
{
	int socket_cliente;
	datosConfig_t datosDelConfig = obtenerEstructurasDelConfig();
	char* ip = datosDelConfig.ip;
	printf("el ip usado es: %s \n",ip);
	int puerto_kernel = datosDelConfig.puerto_kernel;
	printf("el puerto usado es: %d \n",puerto_kernel);
	socket_cliente=conectar(puerto_kernel, ip,id_kernel);
	if(socket_cliente>0)
	{
		printf("Conexion Exitosa!!! \n\n");
		return socket_cliente;
	}
	return -1;
}








int validarRecv(int socket,int resultadoRecv)
{
	if(resultadoRecv<0)
	{
		send(socket,"N",1,0);
		return -1;
	}
	return 0;
}




uint32_t recibirTamPaquete(int socket)
{
	int resultado;
	uint32_t uint;
	printf("entro a recibir tamanio buffer \n");
	uint32_t tambuffer=sizeof(uint32_t);
	char* buffer=malloc(tambuffer);
	memset(buffer,'\0',tambuffer);
	resultado=recvall(socket,buffer,tambuffer);
	if(validarRecv(socket,resultado)<0)
	{
		free(buffer);
		return -1;
	}
	memcpy(&uint,buffer,tambuffer);
	//tamanioTotal=*(uint32_t*)buffer;
	free(buffer);
	printf("se recibio: %u \n",uint);
	return uint;
}




//TODO liberar la data cuando no se necesite mas !!!
char* recibirPaquete(int socket,uint32_t tamanio,int* resultadoDelRecv)
{
	char* data=malloc(tamanio);
	printf("entro al recvall \n");
	printf("se va a recibir data de socket : %d ,tamanio: %d \n",socket,tamanio);
	*resultadoDelRecv=recvall(socket,data,tamanio);
	testi(*resultadoDelRecv);
	printf("sali del recvall \n");
	return data;
}





/**
 * Recibe el socket para recibir un pid que devuelve como char*
 *
 * sino lo puede recibir devuelve NULL;
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
	//printf("el pid que recibo del kernel es: %s \n",pid_programa);

	if(strcmp(pid_programa,"N")==0)
	{
		textoEnColor("El programa NO puede ser iniciado ",0,0);
		// TODO averiguar si esta es la forma correcta de eliminar los recursos de este hilo si no pudo ser creado!!!
		free(pid_programa);
		//close(socket_kernel);
		//pthread_exit(NULL);
		return NULL;
	}
	return pid_programa;
}




// TODO liberar lo que devuelve cuando no se use mas!!!
char* recibirSentencia(int socket)
{
	uint32_t tamBuffer=recibirTamPaquete(socket);
	int resultado;
	if(tamBuffer==-1)return NULL;
	char* sentencia=recibirPaquete(socket,tamBuffer,&resultado);
		if(validarRecv(socket,resultado)<0)
		{
			free(sentencia);
			return NULL;
		}
	char* sentencia_string=calloc(1,tamBuffer+1);
	memcpy(sentencia_string,sentencia,tamBuffer);
	free(sentencia);
	printf("la sentencia es: %s\n ",sentencia_string);
	return sentencia_string;
}
