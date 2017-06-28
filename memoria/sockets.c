/*
 * sockets.c
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <pthread.h>
#include <blue4-lib.h>
#include <commons/string.h>
#include "memoria.h"
#include "operacionesMemoria.h"
#include "sockets.h"
#include "configuraciones.h"

const char CONSOLA_ID = '1';
const char KERNEL_ID = '2';
const char CPU_ID = '3';
const char FS_ID = '4';
const char UMC_ID = '5';

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}




int handshakeHandler(int i){

	int reconozcoCliente = -1;
	char bufHandshake[1];
	int tamHandshake = 1;
	if(recvall(i, bufHandshake, sizeof(bufHandshake)) == 0){
		if(bufHandshake[0] == KERNEL_ID || bufHandshake[0] == CPU_ID){
			reconozcoCliente = 1;
		}
	} else {
		return -1;
	}

	if(reconozcoCliente < 0){
		return -1;
	} else {
		if(sendall(i,charToString(UMC_ID),(uint32_t*)&tamHandshake) == 0)
			printf("Handshake exitoso\n");
		return 0;
	}
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



uint32_t obtenerNumeroSerializado(uint32_t* puntero,char* buffer)
{
	uint32_t pid;
	package_t paquete;
	paquete = deserializar(puntero, buffer);
	pid =*(uint32_t *)paquete.data;
	free(paquete.data);
	return pid;
}



//TODO liberar la data cuando no se necesite mas!!!
char* obtenerDataSerializada(uint32_t* puntero,char* buffer)
{
	uint32_t tamData;
	package_t paquete;
	char* data;
	paquete=deserializar(puntero,buffer);
	tamData=paquete.data_size;
	data=malloc(tamData);
	memcpy(data,paquete.data,tamData);
	free(paquete.data);
	return data;
}
//TODO liberar el string cuando no se necesite mas !!!
char* recibirTamanioBuffer(int socket,int* resultado)
{
	printf("entro a recibir tamanio buffer \n");
	int tambuffer=sizeof(uint32_t);
	char* buffer=malloc(tambuffer);
	char* buffer_string=malloc(tambuffer+1);
	memset(buffer,'\0',tambuffer);
	memset(buffer_string,'\0',tambuffer+1);
	*resultado=recvall(socket,buffer,tambuffer);
	memcpy(buffer_string,buffer,tambuffer);
	free(buffer);
	printf("salgo de recibir tamanio buffer \n");
	return buffer_string;
}



uint32_t recibirTamanioBuffer2(int socket)
{
	int resultado;
	uint32_t tamanioTotal;
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
	memcpy(&tamanioTotal,buffer,tambuffer);
	//tamanioTotal=*(uint32_t*)buffer;
	free(buffer);
	printf("salgo de recibir tamanio buffer \n");
	printf("tam_paquete: %u \n",tamanioTotal);
	return tamanioTotal;
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




uint32_t peticionMemoria2(uint32_t socket)
{
	uint32_t tamanio, bytes, puntero = 0;
	package_t paquete;
	char* buffer;
	if(recv(socket,&tamanio, 4,0) != 4){
		send(socket,"N",1,0);
		return -1;
	}

	buffer = malloc(tamanio);
	if(recv(socket,buffer, tamanio,0) != tamanio){
		send(socket,"N",1,0);
		return -1;
	}

	paquete = deserializar(&puntero, buffer);
	bytes =*(uint32_t *)paquete.data;

	send(socket,"Y",1,0);
	printf("Se van a reservar %i bytes de memoria para el socket %i\n", bytes, socket);

	//reservarMemoria(bytes);

	free(paquete.data);
	free(buffer);
	return 0;
}



char* recibir_PID_PAGINAS(int socket,uint32_t* pid,uint32_t* pagina,uint32_t* puntero)
{
	uint32_t tamanioTotalBuffer;
		*puntero=0;
		int resultado;
		char* buffer,*tambuffer_string;
/*
		tambuffer_string=recibirTamanioBuffer(socket,&resultado);
		printf("sali de recibir tamanio buffer \n");
		if(validarRecv(socket,resultado)<0)
		{
			free(tambuffer_string);
			return NULL;
		}
		printf("entro al atoi(tambuffer_string) \n");
		tamanioTotalBuffer=atoi(tambuffer_string);
		free(tambuffer_string);
		printf("sali del atoi(tambuffer_string) \n");
*/
		tamanioTotalBuffer=recibirTamanioBuffer2(socket);
		if(tamanioTotalBuffer<0)return NULL;

		printf("el tamaño del paquete es: %d \n",tamanioTotalBuffer);
		buffer=recibirPaquete(socket,tamanioTotalBuffer,&resultado);
		printf("entro al if del validar  recv \n");
		if(validarRecv(socket,resultado)<0)
		{
			printf("recv invalido \n");
			free(buffer);
			return NULL;
		}
		printf("entro al deserializar \n");
		*pid=obtenerNumeroSerializado(puntero,buffer);
		printf("el pid es: %d \n",*pid);
		*pagina=obtenerNumeroSerializado(puntero,buffer);
		printf("pagina/s : %d \n",*pagina);
		return buffer;
}






void peticionMemoria(uint32_t socket)
{
	uint32_t pid,paginasRequeridas, puntero;
	char *buffer;
	printf("entro a recibir paginas \n");
	buffer=recibir_PID_PAGINAS(socket,&pid,&paginasRequeridas,&puntero);
	printf("sali de aca papurri \n");
	if(buffer==NULL)
	{
		send(socket,"N",1,0);
		free(buffer);
		return ;
	}
	free(buffer);
	if(!(tieneMarcosSuficientes(paginasRequeridas)))
	{
		send(socket,"N",1,0);
		return ;
	}
	send(socket,"Y",1,0);
	// TODO arreglar inicializarPrograma
	//inicializarPrograma(pid,paginasRequeridas);
	printf("Se van a reservar bytes de memoria para el proceso %i\n", pid);
}



char* recibir_PID_PAGINA_OFFSET(int socket,uint32_t* pid,uint32_t* pagina,uint32_t* offset,uint32_t* puntero)
{
		char* buffer;
		buffer=recibir_PID_PAGINAS(socket,pid,pagina,puntero);
		if(buffer)
			{
				*offset=obtenerNumeroSerializado(puntero,buffer);
				return buffer;
			}
		free(buffer);
		return NULL;
}


int recibir_PID_PAGINA_OFFSET_TAMANIO(int socket,uint32_t* pid,uint32_t* pagina,uint32_t* offset,uint32_t* tamanio)
{
	uint32_t puntero;
	char* buffer=recibir_PID_PAGINA_OFFSET(socket,pid,pagina,offset,&puntero);
	if(buffer)
	{
		*tamanio=obtenerNumeroSerializado(&puntero,buffer);
		free(buffer);
		return 0;
	}
	printf("no se pudo recibir: pid,pagina,offset,tamanio  de : %d\n",socket);
	free(buffer);
	return -1;
}


char* recibir_PID_PAGINA_OFFSET_TAMANIO_DATA(int socket,uint32_t* pid,uint32_t* pagina,uint32_t* offset,uint32_t* tamanio)
{
	uint32_t puntero;
	void* data;
		char* buffer=recibir_PID_PAGINA_OFFSET(socket,pid,pagina,offset,&puntero);
		if(buffer)
		{
			*tamanio=obtenerNumeroSerializado(&puntero,buffer);
			data=obtenerDataSerializada(&puntero,buffer);
			free(buffer);
			return data;
		}
		printf("no se pudo recibir: pid,pagina,offset,tamanio,data  de : %d\n",socket);
		free(buffer);
		return NULL;
}



void enviarTamPagina(int socket)
{
	char* data;
	data=string_itoa(configDeMemoria.tamMarco);
	uint32_t tamanio= sizeof(uint32_t);

	sendall(socket,data,&tamanio);
	free(data);
}



void solicitarBytes(int socket)
{
	uint32_t pid,pagina,offset,tamanio;
	int resultado;
	void* data;
	resultado=recibir_PID_PAGINA_OFFSET_TAMANIO(socket,&pid,&pagina,&offset,&tamanio);
	if(resultado<0)return;
	data=leerMemoria(pid,pagina,offset,tamanio);
	if(data){
		send(socket,"Y",1,0);
		sendall(socket,data,&tamanio);
		free(data);
	}else{
		send(socket,"N",1,0);
		free(data);
	}
}


void almacenarBytes(int socket)
{
	uint32_t pid,pagina,offset,tamanio,resultado;
	void* data;
	data=recibir_PID_PAGINA_OFFSET_TAMANIO_DATA(socket,&pid,&pagina,&offset,&tamanio);
	if(data)
	{
		resultado=escribirMemoria(pid,pagina,offset,tamanio,data);
		if(resultado<0)
		{
			send(socket,"N",1,0);
			free(data);
			return;
		}
		send(socket,"Y",1,0);
		free(data);
		return;
	}
	free(data);
}


void operacionesMemoria(dataHilo_t* dataHilo)
{
	//pthread_mutex_lock(&mutex_operacion);
	int socket=dataHilo->socket;
	char cop=dataHilo->codOp;

	printf("LLEGUE A OPERACIONES MEMORIA con %c\n", cop);
	switch(cop)
	{
	case 'A': peticionMemoria2(socket);break;
	case 'I': peticionMemoria(socket);break;
	case 'P': enviarTamPagina(socket);break;
	case 'R': solicitarBytes(socket);break;
	case 'W': almacenarBytes(socket);break;
	default: break;
	}
	//pthread_mutex_unlock(&mutex_operacion);
}





int servidor()
{
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[1];    // buffer for client data
    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

	struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, configDeMemoria.puerto, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
			continue;
		}

		// lose the pesky "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}

	freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
	while (1) {
		read_fds = master; // copy it
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		// run through the existing connections looking for data to read
		for (i = 0; i <= fdmax; i++) {
			if (FD_ISSET(i, &read_fds)) { // we got one!!
				if (i == listener) {
					// handle new connections
					addrlen = sizeof remoteaddr;
					newfd = accept(listener, (struct sockaddr *) &remoteaddr,&addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
						FD_SET(newfd, &master); // add to master set
						if (newfd > fdmax) {    // keep track of the max
							fdmax = newfd;
						}
						printf("selectserver: nueva conexion de  %s en "
								"socket %d\n",
								inet_ntop(remoteaddr.ss_family,
										get_in_addr((struct sockaddr*) &remoteaddr),
										remoteIP, INET6_ADDRSTRLEN), newfd);
						if (handshakeHandler(newfd) == -1) {
							send(newfd, "0", 1, 0);
							close(newfd);  // Le cierro la conexion al intruso
							FD_CLR(newfd, &master); // lo elimino de la lista maestra de FD
						}

					}

				} else {
					// handle data from a client
					if ((nbytes = recv(i, buf, 1, 0)) <= 0) {

						//queue_push(procesosNEW, 2);
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							printf("selectserver: se desconecto el socket %d \n", i);
						} else {
							perror("recv");
						}
						close(i); // bye!
						FD_CLR(i, &master); // remove from master set
					} else {
						// la i es el socket_cliente y el buff es el codigo de operacion!!!.
						pthread_mutex_lock(&mutex_operacion);
						char * opCode = malloc(1);
						opCode[0] = buf[0];
						dataHilo_t dataHilo;
						dataHilo.codOp=opCode[0];
						dataHilo.socket=i;

						//dataHilo_t dataHilo;
						pthread_attr_t hiloDetachable;
						pthread_t hilo;
						pthread_attr_init(&hiloDetachable);
						pthread_attr_setdetachstate(&hiloDetachable,PTHREAD_CREATE_DETACHED);
						pthread_create(&hilo,&hiloDetachable,(void*)operacionesMemoria,(void*)&dataHilo);
						pthread_mutex_unlock(&mutex_operacion);
						//operacionesMemoria(&dataHilo);
						// we got some data from a client
						for (j = 0; j <= fdmax; j++) {
							// send to everyone!
							if (FD_ISSET(j, &master)) {
								// except the listener and ourselves
								if (j != listener && j != i) {
									if (sendall(j, buf, (uint32_t*) nbytes) == -1) {
										perror("send");
									}
								}
							}
						}
					}
				} // END handle data from client
			} // END got new incoming connection
		} // END looping through file descriptors
	} // END for(;;)--and you thought it would never end!

    return 0;
}
