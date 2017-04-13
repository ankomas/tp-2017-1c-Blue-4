/*
 * consola.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

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

int enviarMensaje(int socket)
{
	char mensaje[1000];
	scanf("%s", mensaje);
	int tamanioMensaje = strlen(mensaje);
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
	while (1)
	{
		enviarMensaje(socketCliente);
	}
	return 0;
}

int main()
{
	return conectar(8086, "127.0.0.1");
}
