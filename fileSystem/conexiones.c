/*
 * conexiones.c
 *
 *  Created on: 5/4/2017
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

#include <blue4-lib.h>
#include "operacionesFS.h"

#include <pthread.h>
//#include <commons/config>
#include "conexiones.h"
#define KERNEL_ID '2'
#define FS_ID '4'






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
	uint32_t tamHandshake = 1;
	if(recvall(i, bufHandshake, sizeof bufHandshake) == 0){
		if(bufHandshake[0] == KERNEL_ID){
			reconozcoCliente = 1;
		}
	} else {
		return -1;
	}

	if(reconozcoCliente < 0){
		return -1;
	} else {
		if(sendall(i,charToString(FS_ID),&tamHandshake) == 0)
			printf("Handshake exitoso");
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




uint32_t recibirUint32_t(int socket)
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
	test(concat(2,data,"\0"));
	printf("sali del recvall \n");
	return data;
}


char* recibirPath(int socket)
{
	uint32_t tamanioTotalBuffer;
	int resultado;
	char *path,*buffer;
	tamanioTotalBuffer=recibirUint32_t(socket);
	printf("el tamanio del buffer es: %d \n",tamanioTotalBuffer);
	if(tamanioTotalBuffer==-1)return NULL;
	buffer=recibirPaquete(socket,tamanioTotalBuffer,&resultado);
	if(validarRecv(socket,resultado)<0)
	{
		free(buffer);
		return NULL;
	}
	path=calloc(1,tamanioTotalBuffer+1);
	memcpy(path,buffer,tamanioTotalBuffer);
	free(buffer);
	printf("el path es: %s\n ",path);
	return path;

}

void validar(int socket)
{
	int resultado;
	char* path=recibirPath(socket);
	if(path)
	{
		//send(socket,"Y",1,0);
		resultado=validarArchivo(path);
		if(resultado<0)
		{
			send(socket,"N",1,0);
			free(path);
			return;
		}
		send(socket,"Y",1,0);
		free(path);
		return;
	}
	send(socket,"N",1,0);
}


void crear(int socket)
{
	int resultado;
	char* path=recibirPath(socket);

	if(path)
	{
		//send(socket,"Y",1,0);
		resultado=crearArchivo(path);
		if(resultado<0)
		{
			send(socket,"N",1,0);
			free(path);
			return;
		}
		send(socket,"Y",1,0);
		free(path);
		return;
	}
	send(socket,"N",1,0);
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


/*
void recibirParametrosDeLectua(int socket,uint32_t* offset,uint32_t* tamanio)
{
	uint32_t tamanioTotalBuffer=(sizeof(uint32_t))*2;
	int resultado;
	char* buffer;
    uint32_t *puntero=0;
	buffer=recibirPaquete(socket,tamanioTotalBuffer,&resultado);
		if(validarRecv(socket,resultado)<0)
		{
			free(buffer);
			return ;
		}
		*offset=obtenerNumeroSerializado(puntero,buffer);
		*tamanio=obtenerNumeroSerializado(puntero,buffer);
		free(buffer);
}
*/


void recibir_offset_tamanio(int socket,uint32_t* offset,uint32_t* tamanio)
{
	uint32_t tamanioTotalBuffer=(sizeof(uint32_t))*4;
	int resultado;
	char* buffer;
	uint32_t *puntero=malloc(sizeof(uint32_t));
	*puntero=0;
	buffer=recibirPaquete(socket,tamanioTotalBuffer,&resultado);
	if(validarRecv(socket,resultado)<0)
	{
		free(buffer);
		return ;
	}
	*offset=obtenerNumeroSerializado(puntero,buffer);
	printf("el offset es: %d \n ",*offset);
	*tamanio=obtenerNumeroSerializado(puntero,buffer);
	printf("el tamanio es: %d \n ",*tamanio);
	free(buffer);
}



char* recibir_offset_tamanio_data(int socket,uint32_t* offset,uint32_t* tamanio)
{
	/*
	uint32_t puntero;
	char *data,*buffer;
	buffer=recibirPath_offset_tamanio(socket,offset,tamanio,&puntero);
	if(buffer)
	{
		data=obtenerDataSerializada(&puntero,buffer);
		free(buffer);
		return data;
	}
	printf("no se pudo recibir: offset,tamanio,data  de : %d \n",socket);
	return NULL;
	*/
	int resultado;
	recibir_offset_tamanio(socket,offset,tamanio);
	uint32_t tamanioTotalBuffer=recibirUint32_t(socket);
	if(tamanioTotalBuffer==-1)return NULL;
	printf("el tamaño del paquete es: %d \n",tamanioTotalBuffer);
	char *data=recibirPaquete(socket,tamanioTotalBuffer,&resultado);
	int contador = 0;
	textoRojo("TESTEANDO");
	while(tamanioTotalBuffer > contador){
		printf("%c\n",data[contador]);
		contador++;
	}
	textoRojo("FIN TESTEANDO");
	if(validarRecv(socket,resultado)<0)
		{
			printf("recv invalido \n");
			free(data);
			return NULL;
		}

	return data;

}

void leer(int socket)
{
	char* resultado;
	uint32_t puntero,tamanio,offset;
	char *ruta;
	char* path=recibirPath(socket);
	send(socket,"Y",1,0);
	recibir_offset_tamanio(socket,&offset,&tamanio);

	//if(buffer)
	//{
		//free(buffer);
	ruta=rutaEnPuntoMontaje("/Archivos",path);
	resultado=obtenerDatos(ruta,offset,tamanio);
	if(resultado)
	{
		send(socket,"Y",1,0);
		sendall(socket,resultado,&tamanio);
		free(ruta);
		free(resultado);
		return;
	}else{
		send(socket,"N",1,0);
		free(ruta);
		return;
	}

}


void escribir(int socket)
{

	int resultado;
	uint32_t puntero,tamanio,offset;
	char* buffer,*data,*ruta;
	char* path=recibirPath(socket);
	send(socket,"Y",1,0);
	data=recibir_offset_tamanio_data(socket,&offset,&tamanio);

	//if(data)
	//{
		ruta=rutaEnPuntoMontaje("/Archivos",path);
		printf("Escribir en: %s\n",ruta);
		resultado=guardarDatos(ruta,offset,tamanio,data);
		testi(resultado);
		if(resultado<0)
		{
			send(socket,"N",1,0);
			free(ruta);
			return;
		}
		textoAmarillo("\n wachin te envio una Y \n");
		send(socket,"Y",1,0);
		free(ruta);
		return;
	//}

}


void borrar(int socket)
{
	char* path=recibirPath(socket);
	if(path)
	{
		send(socket,"Y",1,0);
		borrarArchivo(path);
		free(path);
		return;
	}
	send(socket,"N",1,0);
}


void operacionesFS(int socket,char cop)
{
	printf(" \n llego al fs con el cod. de operacion: %c \n",cop);
	switch(cop)
	{
	case 'V': validar(socket);break;
	case 'C': crear(socket);break;
	case 'L': leer(socket);break;
	case 'E': escribir(socket);break;
	case 'B': borrar(socket);break;
	default : break;
	}
}



int servidor()
{
	char* puerto = obtenerConfiguracionString(config,"PUERTO");

    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf;    // buffer for client data
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
	if ((rv = getaddrinfo(NULL, puerto, &hints, &ai)) != 0) {
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
    for(;;) {
        read_fds = master; // copy it
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        // run through the existing connections looking for data to read
        for(i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
						perror("accept");
					} else {
							FD_SET(newfd, &master); // add to master set
							if (newfd > fdmax) {    // keep track of the max
								fdmax = newfd;
							}
							printf("selectserver: new connection from %s on "
								"socket %d\n",
								inet_ntop(remoteaddr.ss_family,
									get_in_addr((struct sockaddr*)&remoteaddr),
									remoteIP, INET6_ADDRSTRLEN),
								newfd);
						if(handshakeHandler(newfd) == -1){
							send(newfd,"0",1,0);
							//close(i);
						}

					}


                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, &buf, 1,0)) <= 0) {

                    	//queue_push(procesosNEW, 2);

                    	// umc =

                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {

                    	operacionesFS(i,buf);

                        // we got some data from a client
                    	/*
                        for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (sendall(j, buf, (int*)nbytes) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }
                        */
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
