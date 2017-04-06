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
#define idKernel 8080


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

int conectar(int puerto, char *ip)
{
	struct data_socket datosDelSocket = crearSocket(puerto, ip);
	int socketCliente = datosDelSocket.socket;
	struct sockaddr_in direccionServidor = datosDelSocket.direccionServidor;
	if (connect(socketCliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0)
	{
		perror("No se pudo conectar");
		return 1;
	}
	//while (1)
	//{
		char* mensaje = "hola soy un proceso Consola";
		enviarMensaje(socketCliente,mensaje,strlen(mensaje));
	//}
	return 0;
}


// esta funcion es provisoria
// opcion 1 : estblecer un id por cada proceso que este definido en un protocolo
// opcion 2 : manejarse por medio de mensajes con strings
// opcion 3 : combinar las dos opciones aneteriores
// duda : el hs se encarga de cortar las comunicaciones de una conexion no deseada o lo hace el propio proceso
int establecerHandshake(int socket,char* mensaje,int tamanioMensaje)
{
	char buf[4];

	enviarMensaje(socket,mensaje,tamanioMensaje);

	memset(buf,'\0',4);

	if(recv(socket,buf,sizeof(uint32_t),0) == -1 ){
		perror("recv");
		exit(1);
	}
	printf("Recibi del cliente : %s , de tamanio : %d \n",buf,strlen(buf));

	return 0;
}
