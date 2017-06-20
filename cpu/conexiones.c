/*
 * conexiones.c
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */
#define ID_KERNEL 2
#define ID_MEMORIA 5

#include "conexiones.h"

static const char* PROGRAMA =
		"begin\n"
		"variables a, b\n"
		"a = 3\n"
		"b = 5\n"
		"a = b + 12\n"
		"b <- doble a\n"
		"b = 5\n"
		"end\n"
		"\n"
		"function doble\n"
		"variables f\n"
		"f = $0 + $0\n"
		"return f\n"
		"end\n"
		"\n";

t_pcb2 testPCB(){
	int i;
	t_pcb2 pcb;
	t_intructions intructions;
	t_metadata_program *metadata;
	char *programa=strdup(PROGRAMA);
	metadata=metadata_desde_literal(programa);

	pcb.pid=1;
	pcb.pc=metadata->instruccion_inicio;
	printf("Instruccion inicio: %i\n",pcb.pc);
	pcb.sp=0;
	pcb.cantPagCod=2;

	pcb.indiceCodigoSize=metadata->instrucciones_size;
	printf("Instrucciones size: %i\n",pcb.pc);
	pcb.indiceCodigo=metadata->instrucciones_serializado;
	for(i=0;i<pcb.indiceCodigoSize;i++)
		printf("Start: %i Offset: %i\n",pcb.indiceCodigo[i].start,pcb.indiceCodigo[i].offset);

	pcb.indiceEtiquetasSize=metadata->etiquetas_size;
	printf("Indice etiquetas size: %i\n",pcb.indiceEtiquetasSize);
	pcb.indiceEtiquetas=metadata->etiquetas;
	printf("Posicion de doble: %i\n",metadata_buscar_etiqueta("doble",pcb.indiceEtiquetas,pcb.indiceEtiquetasSize));

	pcb.indiceStackSize=0;
	pcb.indiceStack=NULL;

	pcb.exitCode=0;

	return pcb;

}

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

void recibirPCB(int socket){
	uint32_t tam,puntero;
	char* buffer;
	package_t paquete;

	if(recv(socket,buffer,4,0)==-1){
		perror("Error al recibir tamanio en recibirPCB");
		return;
	}

	puntero=0;

	paquete=deserializar(buffer,&puntero);


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
