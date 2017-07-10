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

//#include <parser/metadata_program.h>
#include <pthread.h>
#include <blue4-lib.h>

#include "main.h"

uint32_t pidActual = 100;

/* CLIENTE */
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
		if(send(socketCliente, "2", 1, 0) < 1)
			log_error(logger,"Fallo el envio al momento de realizar un handshake");
		charsito = malloc(1);
		if(recv(socketCliente, charsito, 1, 0) < 1)
			log_error(logger,"Fallo el recibir al momento de realizar un handshake");

		//printf("Lo que me llego: %s\n", charsito);
		res=charsito[0]-'0';
		free(charsito);

		return res;
	}

	int conectar(char *puerto, char *ip,int id) {
		struct addrinfo *servinfo = crear_addrinfo_socket(ip, puerto);
		int res;

		int socketCliente = crear_socket(servinfo);

		//printf("Socket creado: %i\n", socketCliente);
		if (connect(socketCliente, servinfo->ai_addr, servinfo->ai_addrlen) != 0) {
			perror("No se pudo conectar");
			return -1;
		}

		res=handshake(socketCliente);

		if(id!=res){
			printf("Error: el id del proceso al que se conecto (%i) no es el requerido (%i)\n",res,id);
			return -1;
		}
		//printf("Conexion existosa.\n");

		return socketCliente;
	}
/* FIN CLIENTE */

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void enviarPID(uint32_t i) {
	log_trace(logger,"Un nuevo proceso ha sido creado");
	char* mensaje = malloc(4);
	mensaje = string_itoa(pidActual);
	uint32_t tamMensaje = 4;
	if(sendall(i,mensaje,&tamMensaje) < 0){
		log_error(logger,"No se pudo crear un proceso");
	}
}

int handshakeHandler(int i){

	int reconozcoCliente = -1;
	char bufHandshake[1];
	uint32_t tamHandshake = 1;
	if(recvall(i, bufHandshake, sizeof(bufHandshake)) == 0){

		if(bufHandshake[0] == PROGRAMA_ID){
			if(sendall(i,"2",&tamHandshake) == 0)
				log_trace(logger, concat(2,"Se realizo el Handshake con exito con ",string_itoa(i)) );
			if(recvall(i, bufHandshake, sizeof(bufHandshake)) == 0){
				if(bufHandshake[0] == 'A'){
				pthread_t thread;
				t_programa * nuevoPrograma = inicializarPrograma(i,pidActual);
				pthread_create(&thread,NULL,(void*)programa,nuevoPrograma);
				pidActual++;
				}
			}
			reconozcoCliente = 1;
		}else if(bufHandshake[0] == CPU_ID){
			t_cpu * nuevaCPU = malloc(sizeof(t_cpu));
			nuevaCPU->id = i;
			nuevaCPU->programaEnEjecucion = NULL;
			nuevaCPU->disponible = true;
			pthread_t thread;
			if(sendall(i,"2",&tamHandshake) == 0)
				log_trace(logger, concat(2,"Se realizo el Handshake con exito con ",string_itoa(i)) );
			pthread_create(&thread, NULL, cpu,nuevaCPU);
			list_add(CPUs,nuevaCPU);
			reconozcoCliente = 1;
		}

	} else {
		return -1;
	}

	if(reconozcoCliente < 0){
		return -1;
	} else {
		return 0;
	}
}

/*void eliminarSiHayPROGRAMAs(int i) {
	if(!list_is_empty(PROGRAMAs)){
		int aux = 0;
		t_programa *programaAux = list_get(PROGRAMAs,aux);
		while (programaAux->pcb->pid != i && aux < list_size(PROGRAMAs)){
			programaAux = list_get(PROGRAMAs,aux);
			aux++;
		}

		if(programaAux->pcb->pid == i){
			free(programaAux->pcb);
			list_remove(PROGRAMAs, aux);
			log_trace(logger,"Programa eliminado");
		}
	}
}*/

void eliminarSiHayCPU(int i) {
	if(!list_is_empty(CPUs)){
		uint32_t aux = 0;
		t_cpu *cpuAux = list_get(CPUs,aux);
		while (cpuAux->id != i && aux < list_size(CPUs)){

			cpuAux = list_get(CPUs,aux);
			aux++;
		}

		if(cpuAux->id == i){
			//pthread_exit(&cpuAux->hilo);
			list_remove(CPUs, aux);
			log_trace(logger,"CPU eliminada");
		}
	}

}

void liberarRecursos(int i,int codigoError){
	if(i == idFS){
		idFS = 0;
	} else if(i == idUMC){
		idUMC = 0;
	} else{
		//Busco a ver si es una cpu o consola
		eliminarSiHayCPU(i);
		//eliminarSiHayPROGRAMAs(i);
	}
}

int servidor(void)
{
	char* puerto = obtenerConfiguracionString(cfg,"PUERTO_PROG");

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
    int i, rv;

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

						if(encontrarCPU(newfd) != NULL){
							FD_CLR(newfd, &master); // Si es una CPU lo saco del select
						} else if(encontrarPrograma(newfd) != NULL){
							FD_CLR(newfd, &master); // Si es una consola lo saco del select
						}
					}

                } else {
                    if ((nbytes = recv(i, buf, sizeof(buf),MSG_WAITALL)) <= 0) {
                        // got error or connection closed by client
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                            liberarRecursos(i,-20);
                        } else {
                            perror("recv");
                            liberarRecursos(i,-20);
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    } else {

                    	// FUNCIONES MANEJADORAS

							// INICIALIZO PROGRAMA
							if(buf[0] == 'A'){



							} else if(buf[0] == 'B'){
								// Envio el valor de una variable global

								uint32_t tamARecibir=0;
								char * rev = malloc(1);

								// Recibo largo del nombre de la variable
								if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al recibir un valor de variable global");
								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");
								rev=realloc(rev,tamARecibir+1);
								memset(rev,'\0',tamARecibir+1);

								// Recibo el nombre de la variable
								if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al recibir un valor de variable global");
								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");

								/*char* res = signedIntToStream((int32_t)dictionary_get(variablesCompartidas,rev));

								if(sendall(i, res, &tamInt) < 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");
								free(rev);
								free(res);*/

							} else if(buf[0] == 'C'){
								// Guardo el valor de una variable global

								uint32_t tamInt=sizeof(int32_t);
								uint32_t tamARecibir=0;
								int32_t nuevoValorVar=0;
								char * rev = malloc(1);

								// Recibo largo del nombre de la variable
								if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al recibir un valor de variable global");
								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");
								rev=realloc(rev,tamARecibir+1);
								memset(rev,'\0',tamARecibir+1);

								// Recibo el nombre de la variable
								if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al recibir un valor de variable global");

								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");

								// Recibo el valor a asignar
								if(recv(i,&nuevoValorVar,tamInt,MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");

								dictionary_put(variablesCompartidas,rev,&nuevoValorVar);

								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al enviar un valor de variable global");

								free(rev);
							} else if(buf[0] == 'W'){
								// Wait semaforo

								uint32_t tamARecibir=0;
								char * rev = malloc(1);

								// Recibo largo del nombre del semaforo
								if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al hacer un Wait");
								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al hacer un Wait");
								rev=realloc(rev,tamARecibir+1);
								memset(rev,'\0',tamARecibir+1);

								// Recibo el nombre del semaforo
								if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al hacer un Wait");
								send(i,"Y",1,0);

								t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
								if(semaforoObtenido->valor > 0){
									semaforoObtenido->valor--;
									if(send(i,"Y",1,0))
										anuncio("Ocurrio un problema al hacer un Wait");
								}else {
									t_cpu * cpuEncontrada = encontrarCPU(i);
									uint32_t pid = cpuEncontrada->programaEnEjecucion->pid;
									queue_push(semaforoObtenido->colaEspera,&pid);
								}

								free(rev);


							} else if(buf[0] == 'S'){
								uint32_t tamARecibir=0;
								char * rev = malloc(1);

								// Recibo largo del nombre del semaforo
								if(recv(i,&tamARecibir,sizeof(uint32_t),MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al hacer un Wait");
								if(send(i,"Y",1,0) < 0)
									anuncio("Ocurrio un problema al hacer un Wait");
								rev=realloc(rev,tamARecibir+1);
								memset(rev,'\0',tamARecibir+1);

								// Recibo el nombre del semaforo
								if(recv(i,&rev,tamARecibir,MSG_WAITALL) <= 0)
									anuncio("Ocurrio un problema al hacer un Wait");
								send(i,"Y",1,0);

								t_semaforo * semaforoObtenido =(t_semaforo *)dictionary_get(semaforos,rev);
								if(queue_size(semaforoObtenido->colaEspera)>0){
									uint32_t *proximoPID = queue_pop(semaforoObtenido->colaEspera);
									t_cpu * cpuEncontrada = encontrarCPUporPID(*proximoPID);

									if(send(i,"Y",cpuEncontrada->id,0))
										anuncio("Ocurrio un problema al hacer un Wait");
								}else {
									semaforoObtenido->valor++;
								}

								free(rev);
							}


                    	// FIN FUNCIONES MANEJADORAS

                        // we got some data from a client
                        /*for(j = 0; j <= fdmax; j++) {
                            // send to everyone!
                            if (FD_ISSET(j, &master)) {
                                // except the listener and ourselves
                                if (j != listener && j != i) {
                                    if (sendall(j, buf, (uint32_t*)&nbytes) == -1) {
                                        perror("send");
                                    }
                                }
                            }
                        }*/
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!

    return 0;
}
