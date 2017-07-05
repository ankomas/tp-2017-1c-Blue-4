/*
 * conexiones.c
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */
#define ID_KERNEL 2
#define ID_MEMORIA 5

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/types.h>
#include <netdb.h>
#include <blue4-lib.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>

#include "conexiones.h"
#include "primitivas.h"
#include "capaMemoria.h"
#include "capaKernel.h"

struct addrinfo *crear_addrinfo_socket(char* ip, char* puerto) {
	int status;
	struct addrinfo hints;
	struct addrinfo *servinfo;

	//Llenar info

	memset(&hints, 0, sizeof hints); //Lleno hints de 0
	hints.ai_family = AF_INET; //ip4v
	hints.ai_socktype = SOCK_STREAM;

	status = getaddrinfo(ip, puerto, &hints, &servinfo);

	if (status != 0) {
		printf("\a");
		system("tput setaf 1");
		fprintf(stderr, "Error de getaddrinfo: %s\n", gai_strerror(status));
		system("tput setaf 9");
		exit(1);
	}

	return servinfo;
}

int crear_socket(struct addrinfo *servinfo) {
	int sockfd;

	sockfd = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol); //El protocolo esta en 0 por el memset

	if (sockfd == -1)
		perror("socekt");

	return sockfd;
}

int handshake(int socketCliente) {
	char *charsito;
	int res;
	send(socketCliente, "3", 1, 0);
	charsito = malloc(1);
	recv(socketCliente, charsito, 1, 0);

	//printf("Lo que me llego: %s\n", charsito);
	res=charsito[0]-'0';
	free(charsito);

	return res;
}

int conectar(char *puerto, char *ip,int id) {
	struct addrinfo *servinfo = crear_addrinfo_socket(ip, puerto);
	int res;

	int socketCliente = crear_socket(servinfo);

	printf("Socket creado: %i\n", socketCliente);
	printf("Comensando conexion a ");
	printf("IP: %s, PUERTO: %s, ",ip,puerto);
	if(id==ID_KERNEL)
		printf("PROCESO: KERNEL\n");
	if(id==ID_MEMORIA)
		printf("PROCESO: MEMORIA\n");

	if (connect(socketCliente, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
		perror("No se pudo conectar");
		return -1;
	}

	res=handshake(socketCliente);

	if(id!=res){
		printf("Error: el id del proceso al que se conecto (%i) no es el requerido (%i)\n",res,id);
		return -1;
	}
	printf("Conexion existosa.\n");

	return socketCliente;
}

void cerrarConexion(int socket){
	close(socket);
}

void elegirFuncion(char op,int socket){
	switch(op){
	case '0':
		recibirPCB(socket);
		break;
	default:
		printf("Error: COP invalido.\n");
		break;
	}
}

int pedirAMemoria(t_pos pos){
	return -1;
}

int asignarAMemoria(t_pos pos,int variable){
	return -1;
}

void standby(int socket){
	char op;
	while(1){
	if(send(socket,"0",1,0)<1){
		perror("No se pudo enviar");
		return;
	}
	printf("A la espera del kernel...\n");
	if(recv(socket,&op,1,MSG_WAITALL)<1){
		perror("No se pudo recibir");
		return;
	}

	elegirFuncion(op,socket);
	}
}
