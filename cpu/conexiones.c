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
	//t_intructions intructions;
	t_metadata_program *metadata;
	t_pos pos;
	t_list *args,*vars;
	char *programa=strdup(PROGRAMA);
	metadata=metadata_desde_literal(programa);

	pcb.pid=1;
	pcb.pc=metadata->instruccion_inicio;
	pcb.sp=0;
	pcb.cantPagCod=2;

	pcb.indiceCodigoSize=metadata->instrucciones_size;
	pcb.indiceCodigo=metadata->instrucciones_serializado;

	pcb.indiceStack=list_create();
	args=list_create();
	vars=list_create();

	pos.pag=0;
	pos.size=4;
	pos.off=4;

	list_add(args,var_create('a',pos));
	list_add(vars,var_create('b',pos));
	list_add(pcb.indiceStack,stack_create(args,vars,5,pos));

	pcb.indiceEtiquetasSize=metadata->etiquetas_size;
	pcb.indiceEtiquetas=metadata->etiquetas;

	pcb.exitCode=5000;

	package_t paquete;
	t_pcb2 pcb2;

	paquete=serializarPCB(pcb);
	printf("PAQUETE SIZE: %i\n",paquete.data_size);
	pcb2=deserializarPCB(paquete.data);

	list_destroy_and_destroy_elements(pcb.indiceStack,(void*)stack_destroy);

	free(pcb.indiceCodigo);
	free(pcb.indiceEtiquetas);

	printf("Instruccion inicio: %i\n",pcb2.pc);

	printf("Instrucciones size: %i\n",pcb2.indiceCodigoSize);

	for(i=0;i<pcb.indiceCodigoSize;i++)
		printf("Start: %i Offset: %i\n",pcb2.indiceCodigo[i].start,pcb2.indiceCodigo[i].offset);


	printf("Indice etiquetas size: %i\n",pcb2.indiceEtiquetasSize);

	printf("Posicion de doble: %i\n",metadata_buscar_etiqueta("doble",pcb2.indiceEtiquetas,pcb2.indiceEtiquetasSize));

	t_stack *aux;
	aux=list_get(pcb2.indiceStack,0);
	args=aux->args;
	vars=aux->vars;

	list_destroy_and_destroy_elements(pcb2.indiceStack,(void*)stack_destroy);

	free(pcb2.indiceCodigo);
	free(pcb2.indiceEtiquetas);

	free(paquete.data);

	free(metadata);

	free(programa);

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
