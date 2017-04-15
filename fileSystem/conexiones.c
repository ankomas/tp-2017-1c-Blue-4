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

#include <pthread.h>
//#include <commons/config>
#include "conexiones.h"
#define KERNEL_ID '2'
#define FS_ID '1'



typedef struct{
	char* ip;
	int puerto_kernel;
}datosConfig_t;


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



datosConfig_t obtenerEstructurasDelConfig()
{
	datosConfig_t datos;
	datos.ip = obtenerConfiguracionString(rutaAbsolutaDe("configConsola.cfg"),"IP_KERNEL");

	printf("el ip: %s \n",datos.ip);
	datos.puerto_kernel = obtenerConfiguracion(rutaAbsolutaDe("configConsola.cfg"),"PUERTO_KERNEL");
	return datos;
}




int conectarseAlKernel()
{
	int resultado;
	datosConfig_t datosDelConfig = obtenerEstructurasDelConfig();
	char* ip = datosDelConfig.ip;
	printf("el ip usado es: %s \n",ip);
	int puerto_kernel = datosDelConfig.puerto_kernel;
	printf("el puerto usado es: %d \n",puerto_kernel);
	resultado=conectar(puerto_kernel, ip);
	if(resultado==0)
	{
		printf("Conexion Exitosa!!! \n\n");
		return 0;
	}
	return 1;
}

/*
void servidor(int puerto) {

struct sockaddr_in direccionServidor;

	direccionServidor.sin_family = AF_INET;

	direccionServidor.sin_addr.s_addr = INADDR_ANY;

	direccionServidor.sin_port = htons(puerto);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {

	perror("Falló el bind");

	}

	printf("Estoy escuchando\n");

	listen(servidor, 100);

	//------------------------------Establecer HandShake

	struct sockaddr_in direccionCliente;

	unsigned int tamanioDireccion;

	int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	printf("Recibí una conexión en %d!!\n", cliente);

	send(cliente, "Hola NetCat!", 13, 0);

	send(cliente, ":)\n", 4, 0);

	//------------------------------

	char* buffer = malloc(1000);

	while (1) {

	int bytesRecibidos = recv(cliente, buffer, 1000, 0);

	if (bytesRecibidos <= 0) {

	perror("El Kernel se desconectó.\n");

	}
	buffer[bytesRecibidos] = '\0';

	printf("Me llegaron %d bytes con %s\n", bytesRecibidos, buffer);
	}

	free(buffer);
}

*/



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



int servidor()
{
	char* puerto = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO");

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
                    if ((nbytes = recv(i, buf, sizeof buf,0)) <= 0) {

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
                        // we got some data from a client
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
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
