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
#include <signal.h>

#include <pthread.h>
#include <blue4-lib.h>
#include <commons/string.h>
#include "memoria.h"
#include "operacionesMemoria.h"
#include "sockets.h"
#include "configuraciones.h"
#include "estructurasAdministrativas.h"

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
	//printf("entro a recibir tamanio buffer \n");
	int tambuffer=sizeof(uint32_t);
	char* buffer=malloc(tambuffer);
	char* buffer_string=malloc(tambuffer+1);
	memset(buffer,'\0',tambuffer);
	memset(buffer_string,'\0',tambuffer+1);
	*resultado=recvall(socket,buffer,tambuffer);
	memcpy(buffer_string,buffer,tambuffer);
	free(buffer);
	//printf("salgo de recibir tamanio buffer \n");
	return buffer_string;
}



uint32_t recibirUint32_t(int socket)
{
	int resultado;
	uint32_t uint;
	//printf("entro a recibir tamanio buffer \n");
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
	//printf("se recibio: %u \n",uint);
	return uint;
}

//TODO liberar la data cuando no se necesite mas !!!
char* recibirPaquete(int socket,uint32_t tamanio,int* resultadoDelRecv)
{
	char* data=malloc(tamanio);
	//printf("entro al recvall \n");
	//printf("se va a recibir data de socket : %d ,tamanio: %d \n",socket,tamanio);
	*resultadoDelRecv=recvall(socket,data,tamanio);
	//testi(*resultadoDelRecv);
	//printf("sali del recvall \n");
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
	//printf("Se van a reservar %i bytes de memoria para el socket %i\n", bytes, socket);
	char* num_str = string_itoa(bytes);
	char* numero_str = string_itoa(socket);
	char* mensaje_log = concat(2,"Se van a reservar en la memoria los siguientes bytes: ",num_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
	mensaje_log = concat(2,"del socket: ",numero_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(numero_str);
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
		char* buffer;
		tamanioTotalBuffer=recibirUint32_t(socket);
		if(tamanioTotalBuffer<0)return NULL;

		//printf("el tamaño del paquete es: %d \n",tamanioTotalBuffer);
		//usleep(10000);
		buffer=recibirPaquete(socket,tamanioTotalBuffer,&resultado);
		//printf("entro al if del validar  recv \n");
		if(validarRecv(socket,resultado)<0)
		{
			printf("recv invalido \n");
			free(buffer);
			return NULL;
		}
		//printf("entro al deserializar \n");
		*pid=obtenerNumeroSerializado(puntero,buffer);
		//printf("el pid es: %d \n",*pid);
		*pagina=obtenerNumeroSerializado(puntero,buffer);
		//printf("pagina/s : %d \n",*pagina);
		char* num_str = string_itoa(*pagina);
	char* numero_str = string_itoa(*pid);
	char* mensaje_log = concat(2,"Para el pid: ",numero_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(numero_str);
	mensaje_log = concat(2,"Se reservan pagina/s: ",num_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
		return buffer;
}






void peticionMemoria(uint32_t socket)
{
	uint32_t pid,paginasRequeridas, puntero;
	char *buffer;
	buffer=recibir_PID_PAGINAS(socket,&pid,&paginasRequeridas,&puntero);
	if(buffer==NULL)
	{
		send(socket,"N",1,0);
		return ;
	}
	free(buffer);
	char* num_str = string_itoa(pid);
	if(!(tieneMarcosSuficientes(paginasRequeridas)))
	{
		char* mensaje_log = concat(2,"No Se pudo  reservar  memoria para el proceso: ",num_str);
		log_error(logMemoria,mensaje_log);
		free(mensaje_log);
		free(num_str);
		send(socket,"N",1,0);
		return ;
	}
	send(socket,"Y",1,0);
	//printf("Se van a reservar bytes de memoria para el proceso %i\n", pid);
	char* mensaje_log = concat(2,"Se va a reservar  memoria para el proceso: ",num_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
	inicializarPrograma(pid,paginasRequeridas);
	//printf("salgo del inicializar \n");
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
	//printf("no se pudo recibir: pid,pagina,offset,tamanio  de : %d\n",socket);
	char* numero_str = string_itoa(socket);
	char* mensaje_log = concat(2,"No se pudo recibir data para el socket: ",numero_str);
	log_error(logMemoria,mensaje_log);
	free(mensaje_log);
	free(numero_str);
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
		//printf("no se pudo recibir: pid,pagina,offset,tamanio,data  de : %d\n",socket);
		char* numero_str = string_itoa(socket);
		char* mensaje_log = concat(2,"No se pudo recibir data para el socket: ",numero_str);
		log_error(logMemoria,mensaje_log);
		free(mensaje_log);
		free(numero_str);
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
	if(!validarPIDPAG(pid,pagina)){
		send(socket,"N",1,0);
		return;
	}
	char* num_str = string_itoa(pid);
	data=leer(pid,pagina,offset,tamanio);
/*
	int wtf;
	char *wtf2=(char*)data;
	printf("\n");
	for(wtf=0;wtf<tamanio;wtf++)
		printf("%c",wtf2[wtf]);
	printf("\n");
*/
	if(data){
		char* mensaje_log = concat(2,"Se leyo correctamente en el proceso : ",num_str);
				log_trace(logMemoria,mensaje_log);
				free(mensaje_log);
				free(num_str);
		send(socket,"Y",1,0);
		sendall(socket,data,&tamanio);
		free(data);
	}else{
		char* mensaje_log = concat(2,"NO se pudo leer correctamente en el proceso : ",num_str);
		log_error(logMemoria,mensaje_log);
		free(mensaje_log);
		free(num_str);
		send(socket,"N",1,0);
	}
}


void almacenarBytes(int socket)
{
	uint32_t pid,pagina,offset,tamanio,resultado;
	void* data;
	data=recibir_PID_PAGINA_OFFSET_TAMANIO_DATA(socket,&pid,&pagina,&offset,&tamanio);
	if(!validarPIDPAG(pid,pagina)){
		send(socket,"N",1,0);
		return;
	}
	if(data)
	{
/*
		int wtf;
		char *wtf2=(char*)data;
		printf("\n");
		for(wtf=0;wtf<256;wtf++)
			printf("%c",wtf2[wtf]);
		printf("\n");
*/
		char* num_str = string_itoa(pid);
		resultado=escribir(pid,pagina,offset,tamanio,data);
		if(resultado<0)
		{
			char* mensaje_log = concat(2,"NO se escribio correctamente en el proceso : ",num_str);
			log_error(logMemoria,mensaje_log);
			free(mensaje_log);
			free(num_str);
			send(socket,"N",1,0);
			free(data);
			return;
		}
		char* mensaje_log = concat(2,"Se escribio correctamente en el proceso : ",num_str);
		log_trace(logMemoria,mensaje_log);
		free(mensaje_log);
		free(num_str);
		send(socket,"Y",1,0);
		free(data);
		return;
	}
	// log_error()
}


void agregarPaginasA(int socket)
{
	log_trace(logMemoria,"Iniciando agregar paginas a un proceso");
	uint32_t pid,paginasRequeridas, puntero;
	int resultado;
		char *buffer;
		buffer=recibir_PID_PAGINAS(socket,&pid,&paginasRequeridas,&puntero);
		if(buffer==NULL)
		{
			send(socket,"N",1,0);
			return ;
		}
		free(buffer);
		resultado=asignarPaginasAUnProceso(pid,paginasRequeridas);
		if(resultado<0)
		{
			//texto_en_color_de_error(" LA MEMORIA NO POSEE MAS PAGINAS DISPONIBLES \n");
	
	log_error(logMemoria,"LA MEMORIA NO POSEE MAS MARCOS DISPONIBLES ");
			send(socket,"N",1,0);
			return;
		}
		send(socket,"Y",1,0);
		//printf("Se agregaron paginas para el proceso %i\n", pid);
	char* num_str = string_itoa(pid);
	char* mensaje_log = concat(2,"SE AGREGARON PAGINAS CORRECTAMENTE PARA EL PROCESO : ",num_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
}


void darFinA(int socket)
{
	log_trace(logMemoria,"Iniciando finalizar programa");
	uint32_t pid;
	int resultado;
	pid=recibirUint32_t(socket);
	if(pid<0)return;
	char* num_str = string_itoa(pid);
	resultado=finalizarPrograma(pid);
	if(resultado<0)
	{
		char* mensaje_log = concat(2,"No pudo finalizarse correctamente el proceso: ",num_str);
		log_error(logMemoria,mensaje_log);
		free(mensaje_log);
		free(num_str);
		send(socket,"N",1,0);
		return;
	}
		send(socket,"Y",1,0);
	//printf("Se finalizo el proceso %i\n", pid);
	char* mensaje_log = concat(2,"Se finalizo correctamente el proceso: ",num_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
}


void eliminarPaginaDe(int socket)
{
	log_trace(logMemoria,"Iniciando eliminar pagina");
	uint32_t pid,paginaRequeridas, puntero;
	int resultado;
	char *buffer;
	buffer=recibir_PID_PAGINAS(socket,&pid,&paginaRequeridas,&puntero);
	if(buffer==NULL)
	{
		send(socket,"N",1,0);
		return ;
	}
	free(buffer);
	char* num_str = string_itoa(pid);
	resultado=eliminarPaginaDeUnProceso(pid,paginaRequeridas);
	if(resultado<0)
	{
		char* mensaje_log = concat(2,"No se pudo eliminar una pagina del proceso : ",num_str);
		log_error(logMemoria,mensaje_log);
		free(mensaje_log);
		free(num_str);
		send(socket,"N",1,0);
		return;
	}
	send(socket,"Y",1,0);
	//printf("Se elimino una pagina del proceso: %i\n", pid);
	char* mensaje_log = concat(2,"Se elimino correctamente una pagina del proceso : ",num_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
}

void manejarProgramaCaido(){

}
void operacionesMemoria(int socket)
{
	//pthread_mutex_lock(&mutex_operacion);
	//int socket=dataHilo->socket;
	char cop;//=dataHilo->codOp;
	while(1)
	{
	if(recv(socket,&cop,1,MSG_WAITALL)==0)
		{
			//printf("se desconecto el cliente : %d",socket);
		char* num_str = string_itoa(socket);
	char* mensaje_log = concat(2,"Se desconecto el cliente: ",num_str);
	log_error(logMemoria,mensaje_log);
	free(mensaje_log);
	free(num_str);
			return;
		}
	signal(SIGPIPE, manejarProgramaCaido);
	//printf(" \n %s LLEGUE A OPERACIONES MEMORIA con %c %s \n",KBLU, cop,KNRM);
        char* cop_str = malloc(2);
		memset(cop_str,'\0',2);
		memcpy(cop_str,&cop,1);
	char* mensaje_log = concat(2,"EL CODIGO DE OPERACION DE MEMORIA ES : ",cop_str);
	log_trace(logMemoria,mensaje_log);
	free(mensaje_log);
	free(cop_str);	
	switch(cop)
	{
		case 'A': agregarPaginasA(socket);break;
		case 'E': eliminarPaginaDe(socket);break;
		case 'F': darFinA(socket);break;
		case 'I': peticionMemoria(socket);break;
		case 'P': enviarTamPagina(socket);break;
		case 'R': solicitarBytes(socket);break;
		case 'W': almacenarBytes(socket);break;
		default: break;
	}
  }
	//pthread_mutex_lock(&mutex_operacion);
	//printf("el socket es : %d \n",socket);
	//ingresarSocketALista(socket);
	//pthread_mutex_unlock(&mutex_operacion);

	//pthread_mutex_unlock(&mutex_operacion);
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
		if(sendall(i,charToString(UMC_ID),(uint32_t*)&tamHandshake) == 0){
			log_trace(logMemoria,"Handshake exitoso");

			pthread_attr_t hiloDetachable;
			pthread_t hilo;
			pthread_attr_init(&hiloDetachable);
			pthread_attr_setdetachstate(&hiloDetachable,PTHREAD_CREATE_DETACHED);
			pthread_mutex_lock(&mutex_operacion);
			//FD_CLR(i,&master);
			//printf("entro al hilo \n");
			pthread_mutex_unlock(&mutex_operacion);
			pthread_create(&hilo,&hiloDetachable,(void*)operacionesMemoria,(void*)i);

		return 0;
		}
	}
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

    char buf;    // buffer for client data
    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];

    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

	struct addrinfo hints, *ai, *p;

	pthread_mutex_lock(&mutex_operacion);
    FD_ZERO(&master);    // clear the master and temp sets
    pthread_mutex_unlock(&mutex_operacion);
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
    pthread_mutex_lock(&mutex_operacion);
    FD_SET(listener, &master);
    pthread_mutex_unlock(&mutex_operacion);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one

    // main loop
	while (1) {
		pthread_mutex_lock(&mutex_operacion);
		read_fds = master; // copy it
		pthread_mutex_unlock(&mutex_operacion);
		if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
			perror("select");
			exit(4);
		}

		//printf("hasta aca llegue \n");
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
						pthread_mutex_lock(&mutex_operacion);
						FD_SET(newfd, &master); // add to master set
						pthread_mutex_unlock(&mutex_operacion);
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
							pthread_mutex_lock(&mutex_operacion);
							FD_CLR(newfd, &master); // lo elimino de la lista maestra de FD
							pthread_mutex_unlock(&mutex_operacion);
						}
						FD_CLR(newfd,&master);

					}

				} else {
					// handle data from a client
					//printf("entro al recv del cop \n");
					if ((nbytes = recv(i, &buf, 1, 0)) <= 0) {

						//queue_push(procesosNEW, 2);
						// got error or connection closed by client
						if (nbytes == 0) {
							// connection closed
							//printf("selectserver: se desconecto el socket %d \n", i);
							char* numero_str = string_itoa(i);
							char* mensaje_log = concat(2,"Se Desconecto el socket : ",numero_str);
							log_error(logMemoria,mensaje_log);
							free(mensaje_log);
							free(numero_str);
						} else {
							perror("recv");
						}
						close(i); // bye!
						pthread_mutex_lock(&mutex_operacion);
						FD_CLR(i, &master); // remove from master set
						pthread_mutex_unlock(&mutex_operacion);
					} else {
						// la i es el socket_cliente y el buff es el codigo de operacion!!!.
						//pthread_mutex_lock(&mutex_operacion);
						//char * opCode = malloc(1);
						//opCode[0] = buf;
						//dataHilo_t *dataHilo = malloc(sizeof(dataHilo_t));
						//dataHilo->codOp=opCode[0];
						//dataHilo->socket=i;

						//dataHilo_t dataHilo;
						/*
						pthread_attr_t hiloDetachable;
						pthread_t hilo;
						pthread_attr_init(&hiloDetachable);
						pthread_attr_setdetachstate(&hiloDetachable,PTHREAD_CREATE_DETACHED);
						pthread_mutex_lock(&mutex_operacion);
						FD_CLR(i,&master);
						//printf("entro al hilo \n");
						pthread_mutex_unlock(&mutex_operacion);
						pthread_create(&hilo,&hiloDetachable,(void*)operacionesMemoria,(void*)dataHilo);
						*/
						//pthread_mutex_unlock(&mutex_operacion);
						//operacionesMemoria(dataHilo);
						// we got some data from a client
						/*for (j = 0; j <= fdmax; j++) {
							// send to everyone!
							if (FD_ISSET(j, &master)) {
								// except the listener and ourselves
								if (j != listener && j != i) {
									if (sendall(j, &buf, (uint32_t*) nbytes) == -1) {
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
