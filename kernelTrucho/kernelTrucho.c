/*
 * kernelTrucho.c
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>
#include <blue4-lib.h>
#define IPDEFECTO "127.0.0.1"
#define PACKAGESIZE 1024	// Define cual va a ser el size maximo del paquete a enviar

int crearSocket(struct addrinfo *serverInfo)
{
	return socket(serverInfo->ai_family, serverInfo->ai_socktype,
			serverInfo->ai_protocol);
}

struct addrinfo* crearServerInfo(char* puerto, char* ip)
{
	struct addrinfo *serverInfo;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(ip, puerto, &hints, &serverInfo);
	return serverInfo;
}

void enviarMensajesDelTeclado(int socket)
{
	int enviar = 1;
	char message[PACKAGESIZE];
	printf(
			"Ya puede enviar mensajes. Escriba 'exit' para salir\n");
	while (enviar) {
		fgets(message, PACKAGESIZE, stdin);
		if (!strcmp(message, "exit\n"))
		{
			enviar = 0;
			send(socket, "Me desconecto\n", strlen("Me desconecto\n") + 1 , 0);
		}

		if (enviar)
			send(socket, message, strlen(message) + 1, 0);
	}
}


int conectarseA(char* puerto, char* ip)
{
	int bool;
	struct addrinfo* serverInfo = crearServerInfo(puerto, ip);
	int serverSocket = crearSocket(serverInfo);
	bool = connect(serverSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	if(bool>=0)
		printf("Conectado Exitosamente\n");
	else
		printf("No me conecte :c");
	printf("bool = %i\n", bool);
	freeaddrinfo(serverInfo);
	return serverSocket;
}

void flocasend(uint32_t i) {
	package_t paquete;
	uint32_t data, bytes=-1, contador=1;

	data = 9999;

	paquete = serializar(2, sizeof(uint32_t), &data);

	bytes = send(i, "a", 1, 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);
	bytes = send(i, &paquete.data_size, sizeof(uint32_t), 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);
	bytes = send(i, paquete.data, paquete.data_size, 0);
	printf("envie %i bytes en el %i send\n", bytes, contador++);

	free(paquete.data);
}

int handshake(int socketCliente) {
	char *charsito;
	int res;
	if(send(socketCliente, "2", 1, 0) < 1)
		printf("Fallo el envio al momento de realizar un handshake");
	charsito = malloc(1);
	if(recv(socketCliente, charsito, 1, 0) < 1)
		printf("Fallo el recibir al momento de realizar un handshake");

	//printf("Lo que me llego: %s\n", charsito);
	res=charsito[0]-'0';
	free(charsito);

	return res;
}

int main()
{
	uint32_t socket = conectarseA("8085", IPDEFECTO);
	handshake(socket);
	printf("socket = %i\n", socket);
	flocasend(socket);
	enviarMensajesDelTeclado(socket);

	close(socket);
	return 0;
}
