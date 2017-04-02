/*
 * sockets.h
 *
 *  Created on: 31/8/2016
 *      Author: utnso
 */

#ifndef COMMONS_SOCKETS_H_
#define COMMONS_SOCKETS_H_

#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "collections/list.h"
#include <string.h>
#include <sys/time.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// ESTRUCTURAS
typedef struct{
	//char* nombre; //Se usar para la ruta del directorio
	int vidas;
	int x;
	int y;
	int pokemones_a_capturar;
	//char* pokemones;
	//char simbolo;
	int cant_reintentos;
} __attribute__((__packed__)) entrenador_t;

typedef struct {
	int8_t type;
	int16_t length;
}__attribute__((__packed__)) header_t;

typedef struct {
	char *addr;
	char *port;
}__attribute__((__packed__)) ip_info_t;

// DEFINICIONES
int getSocket(void);
int bindSocket(int, char *, char *);
int listenSocket(int, int);
int acceptSocket(int);
int sendSocket(int socket, header_t* cabecera, void* data);
int connectSocket(int, char *, char *);
int createServer(char *, char *, int);
int createClient(char *, char *);
header_t crear_cabecera(int codigo, int length);
int enviar_paquete_vacio(int codigo_operacion, int socket);
int recibir_paquete(int socket, void** paquete, int* tipo);
int enviarHandshake(int sockfd, int headerType);
void* entrenador_serializer(entrenador_t *self, int16_t *length);
entrenador_t* entrenador_deserializer(int socketfd);
int sendSocket(int sockfd, header_t *header, void *data);
int sendallSocket(int socket, void* paquete_a_enviar, int tamanio_paquete);

//PROTOCOLOS

enum enum_protocolo {// Si yo soy el mapa tengo que enviar handshake_entrenador.
	PEDIDO_INFO_CONEXION = 1,
	HANDSHAKE_MAPA= 2,
	HANDSHAKE_ENTRENADOR = 3,
	OK = 4
};
enum enum_protocolo_entrenador_a_mapa {// Si yo soy el mapa tengo que enviar handshake_entrenador.
	INICIAR_AVENTURA = 5,
	UBICACION_POKENEST= 6
};
enum enum_protocolo_mapa_a_entrenador{// Si yo soy el mapa tengo que enviar handshake_entrenador.
	UBICACION_POKENEST_MAPA= 10,
	STATUS_AVENTURA = 11,
	OBJETIVOS_FINALIZADOS = 12,
	BATALLA_POKEMON = 13,
	STATUS = 14,
	GAME_OVER=15
};
#endif /* COMMONS_SOCKETS_H_ */
