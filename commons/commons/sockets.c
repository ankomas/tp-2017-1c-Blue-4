/*
 * sockets.c
 *
 *  Created on: 31/8/2016
 *      Author: utnso
 */
#include "sockets.h"

int getSocket(void) {
	int yes = 1;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		close(sockfd);
		return -1;
	}

	return sockfd;
}

/**
 * @NAME: bindSocket
 * @DESC: Bindea el socket a la dirección ip y puerto pasados por parámetro.
 * Retorna -1 en caso de error.
 */
int bindSocket(int sockfd, char *addr, char *port) {
	struct sockaddr_in my_addr;

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(atoi(port));
	my_addr.sin_addr.s_addr = inet_addr(addr);
	memset(&(my_addr.sin_zero), '\0', 8);

	return bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr));
}

/**
 * @NAME: listenSocket
 * @DESC: Pone a la escucha el socket pasado por parámetro con el backlog pasado por parámetro.
 * Retorna -1 en caso de error.
 */
int listenSocket(int sockfd, int backlog) {
	return listen(sockfd, backlog);
}

/**
 * @NAME: acceptSocket
 * @DESC: Acepta una conexion al socket enviado por parámetro y retorna el nuevo descriptor de socket.
 * Retorna -1 en caso de error.
 */
int acceptSocket(int sockfd) {
	struct sockaddr_in their_addr;
	int sin_size = sizeof(struct sockaddr_in);

	return accept(sockfd, (struct sockaddr *) &their_addr,
			(socklen_t *) &sin_size);
}

/**
 * @NAME: connectSocket
 * @DESC: Conecta el socket a la dirección y puerto pasados por parámetro.
 * Retorna -1 en caso de error.
 */
int connectSocket(int sockfd, char *addr, char *port) {
	struct sockaddr_in their_addr;

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(atoi(port));
	their_addr.sin_addr.s_addr = inet_addr(addr);
	memset(&(their_addr.sin_zero), '\0', 8);

	return connect(sockfd, (struct sockaddr*) &their_addr,
			sizeof(struct sockaddr));
}
/**
 * @NAME: createServer
 * @DESC: Crea y devuelve un socket para ser utilizado como servidor.
 * En caso de error retorna -1.
 */
int createServer(char *addr, char *port, int backlog) {

	struct addrinfo hints;
	struct addrinfo *serverInfo;

	memset(&hints,0,sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(NULL,port,&hints,&serverInfo);

	int sockfd = getSocket();

	if (bindSocket(sockfd, addr, port) == -1) {
		perror("bind");
		close(sockfd);
		return -1;
	}

	if (listenSocket(sockfd, backlog) == -1) {
		perror("listen");
		close(sockfd);
		return -1;
	}

	return sockfd;
}
/**
 * @NAME: createClient
 * @DESC: Crea y devuelve un socket para ser utilizado como cliente.
 * En caso de error retorna -1.
 */
int createClient(char *addr, char *port) {
	int sockfd = getSocket();

	if (connectSocket(sockfd, addr, port) == -1) {
		perror("connect");
		close(sockfd);
		return -1;
	}

	return sockfd;
}

int enviarHandshake(int sockfd, int headerType) {
	header_t header;
	header.type = headerType;
	header.length = 0;

	return sendSocket(sockfd, &header, '\0');
}


header_t crear_cabecera(int codigo, int length){

	header_t cabecera;

	cabecera.type = codigo;
	cabecera.length = length;

	return cabecera;

}

//Para los Handshake
int enviar_paquete_vacio(int codigo_operacion, int socket){

	header_t cabecera = crear_cabecera(codigo_operacion, 1);
	int bytes_enviados = sendSocket(socket, &cabecera, "0");

	return bytes_enviados;
}

int recibir_paquete(int socket, void** paquete, int* tipo){

	int bytes_recibidos;
	//void* buffer;
	header_t cabecera;

	bytes_recibidos = recv(socket, &cabecera, sizeof(header_t),MSG_WAITALL);

	if(bytes_recibidos == 0) return 0;
	if(bytes_recibidos == -1) return -1;

	*tipo = cabecera.type;
	//buffer = malloc(cabecera.length);

	//bytes_recibidos = recv(socket, buffer, cabecera.length, MSG_WAITALL);

	//if(bytes_recibidos == 0) return 0;
	//if(bytes_recibidos == -1) return -1;

	//*paquete = buffer;

	return bytes_recibidos;

}

void* entrenador_serializer(entrenador_t* self, int16_t *length){

	void *serialized = malloc(sizeof(entrenador_t));
	int offset = 0, tmp_size = 0;

	//memcpy(serialized, &self->nombre, tmp_size = sizeof(self->nombre));
	//offset += tmp_size;

	memcpy(serialized + offset, &self->cant_reintentos, tmp_size = sizeof(self->cant_reintentos));
	offset += tmp_size;

	//memcpy(serialized+ offset, &self->simbolo, tmp_size = sizeof(self->simbolo));
	//offset += tmp_size;

	memcpy(serialized+ offset, &self->vidas, tmp_size = sizeof(self->vidas));
	offset += tmp_size;

	memcpy(serialized+ offset, &self->x, tmp_size = sizeof(self->x));
	offset += tmp_size;

	memcpy(serialized+ offset, &self->y, tmp_size = sizeof(self->y));
	offset += tmp_size;

	memcpy(serialized+ offset, &self->pokemones_a_capturar, tmp_size = sizeof(self->pokemones_a_capturar));
	offset += tmp_size;

	*length = offset;

	return serialized;
}

entrenador_t* entrenador_deserializer(int socketfd) {
	entrenador_t* self = malloc(sizeof(entrenador_t));
	//recv(socketfd, &self->nombre, sizeof(self->nombre), MSG_WAITALL);
	recv(socketfd, &self->cant_reintentos, sizeof(self->cant_reintentos), MSG_WAITALL);
	//recv(socketfd, &self->simbolo, sizeof(self->simbolo), MSG_WAITALL);
	recv(socketfd, &self->vidas, sizeof(self->vidas), MSG_WAITALL);
	recv(socketfd, &self->x, sizeof(self->x), MSG_WAITALL);
	recv(socketfd, &self->y, sizeof(self->y), MSG_WAITALL);
	return self;
}

/**
 * @NAME: sendSocket
 * @DESC: Serializa el header y el string de datos y lo envía al scoket pasado por parámetro.
 * Retorna la cantidad de bytes enviados o -1 en caso de error.
 */
int sendSocket(int sockfd, header_t *header, void *data) {
	int bytesEnviados, offset = 0, tmp_len = 0;
	void *packet = malloc(sizeof(header_t) + header->length);

	memcpy(packet, &header->type, tmp_len = sizeof(int8_t));
	offset = tmp_len;

	memcpy(packet + offset, &header->length, tmp_len = sizeof(header->length));
	offset += tmp_len;

	memcpy(packet + offset, data, tmp_len = header->length);
	offset += tmp_len;

	bytesEnviados = sendallSocket(sockfd, packet, offset);//El offset representa la cantidad total de bytes del paquete a enviar
	free(packet);

	return bytesEnviados;
}
/**
 * @NAME: sendallSocket
 * @DESC: procura enviar todo el paquete entero. Devuelve la cantidad de bytes enviados o -1 en caso de error.
 *
 */
int sendallSocket(int socket, void* paquete_a_enviar, int tamanio_paquete) {

	int total = 0;
	int bytesleft = tamanio_paquete;
	int n;

	while(total < tamanio_paquete) {
		n = send(socket, paquete_a_enviar+total, bytesleft, 0);
		if(n == -1) {break; }
		total += n;
		bytesleft -= n;
	}

	return n==-1?-1:total;

}
