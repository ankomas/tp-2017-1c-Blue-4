/*
 * capaKernel.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */
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
#include "primitivasKernel.h"
#include "capaMemoria.h"
#include "capaKernel.h"
#include "test.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

char* const lineaEnPrograma(t_pcb2* pcb,t_puntero_instruccion inicioDeLaInstruccion, t_size tamanio) {
	char* aRetornar;
	//aRetornar=pedirLineaAMemoria(pcb,inicioDeLaInstruccion,tamanio);
	aRetornar=pedirLineaAMemoria(pcb,inicioDeLaInstruccion,tamanio);
	return aRetornar;
}

void ejecutarPCB(t_pcb2 *pcb, int socket){
	//char* programa=pedirProgramaAMemoria(pcb,socket);
	if(pcb->pc>pcb->indiceCodigoSize){
		printf(RED"ERROR: se esta tratando de ejecutar PC:%i cuando la ultima instruccion es: %i\n"RESET,pcb->pc,pcb->indiceCodigoSize);
		setExitCode(&pcb_global,"error desconocido",20);
		finPrograma_global='F';
		return;
	}
	char* const linea = lineaEnPrograma(pcb,
			pcb->indiceCodigo[pcb->pc].start,
			pcb->indiceCodigo[pcb->pc].offset);
	if(linea==-1){
		setExitCode(&pcb_global,"fallo al leer en memoria",11);
		return;
	}
	printf(GRN"PID: %i PC:%i SP:%i Ultima Pos Usada: (%i,%i,%i)\n"RESET,pcb->pid,pcb->pc,pcb->sp,pcb->ultimaPosUsada.pag,pcb->ultimaPosUsada.off,pcb->ultimaPosUsada.size);
	printf("\t Evaluando -> " BLU "%s" RESET"\n", linea);
	pcb->pc++;
	analizadorLinea(linea, &funciones, &kernel_functions);
	//test_asignadoCorrecto();
	free(linea);

}

void recibirPCB(int socket){
	uint32_t tam;
	char* buffer=NULL;
	package_t paquete;

	buffer=malloc(sizeof(uint32_t));
	if(recv(socket,buffer,4,0)==-1){
		perror("Error al recibir tamanio en recibirPCB");
		return;
	}

	memcpy(&tam,buffer,4);
	printf("RECIBIENDO PCB, TAMANIO DEL PAQUETE: %i\n",tam);
	buffer=realloc(buffer,tam);
	if(recv(socket,buffer,tam,MSG_WAITALL) > 0 ){
		send(socket,"Y",1,0);
		pcb_global=deserializarPCB(buffer);
		free(buffer);
		ejecutarPCB(&pcb_global,socket);
	} else {
		send(socket,"N",1,0);
	}

}

void enviarPCB(int socket){
	package_t paquete;
	uint32_t enviado=0;

	paquete=serializarPCB(pcb_global);
	printf("ENVIAR PCB, TAMANIO DEL PAQUETE: %i\n",paquete.data_size);
	printf("ENVIAR FIN DE PROGRAMA: %c\n",finPrograma_global);
	send(socket,&finPrograma_global,1,0);
	send(socket,&paquete.data_size,sizeof(uint32_t),0);
	enviado=send(socket,paquete.data,paquete.data_size,0);
	free(paquete.data);
	liberarPCB(pcb_global);
	/*while(enviado<paquete.data_size){
		enviado=send(socket,paquete.data,paquete.data_size-enviado,0);
	}*/
}

int32_t obtenerVarGlobal(t_nombre_compartida nombre,int32_t *compartida){
	uint32_t nombreTam=strlen(nombre);
	int32_t res;
	char aux;

	printf("	TAM NOMBRE VAR: %i,NOMBRE VAR: %s\n",nombreTam,nombre);
	send(kernel,"O",1,0);
	send(kernel,&nombreTam,sizeof(uint32_t),0);

	recv(kernel,&aux,1,0);//recibo una Y
	send(kernel,nombre,nombreTam,0);

	recv(kernel,&aux,1,0);

	switch(aux){
	case 'Y':
		recv(kernel,&res,sizeof(int32_t),0);
		*compartida=res;
		return 1;
	case 'N':
		printf(RED"ERROR: no se encontro la variable global en memoria\n"RESET);
		return -1;
	default:
		printf(RED"ERROR: respuesta invalida en obtener var global\n"RESET);
		return -1;
	}
}

int asignarVarGlobal(t_nombre_compartida nombre,t_valor_variable valor){
	uint32_t nombreTam=strlen(nombre);
	char aux;

	send(kernel,"A",1,0);
	send(kernel,&nombreTam,sizeof(uint32_t),0);

	recv(kernel,&aux,1,0);//recibo una Y
	send(kernel,nombre,nombreTam,0);

	recv(kernel,&aux,1,0);//recibo una Y
	send(kernel,&valor,sizeof(int32_t),0);

	recv(kernel,&aux,1,0);//recibo una Y
	switch(aux){
	case 'Y':
		return 1;
	case 'N':
		printf(RED"ERROR: no se encontro la variable global en memoria\n"RESET);
		return -1;
	default:
		printf(RED"ERROR: respuesta invalida en obtener var global\n"RESET);
		return -1;
	}
}

char semWait(t_nombre_semaforo sem){
	uint32_t nombreTam=strlen(sem);
	char aux;

	send(kernel,"W",1,0);

	send(kernel,&nombreTam,sizeof(uint32_t),0);
	recv(kernel,&aux,1,0); //recibo una y

	send(kernel,sem,nombreTam,0);
	recv(kernel,&aux,1,0); //recibo una y

	recv(kernel,&aux,1,0); //recibo una B o una Y o una N

	return aux;
}

char semSignal(t_nombre_semaforo sem){
	uint32_t nombreTam=strlen(sem);
	char aux;

	send(kernel,"S",1,0);

	send(kernel,&nombreTam,sizeof(uint32_t),0);
	recv(kernel,&aux,1,0); //recibo una y

	send(kernel,sem,nombreTam,0);
	recv(kernel,&aux,1,0); //recibo una y

	recv(kernel,&aux,1,0); //recibo una Y o N

	return aux;
}

char alloc(t_valor_variable valor,t_puntero *puntero){
	char res,*temp;

	temp=malloc(valor);
	memset(temp,0,valor);

	send(kernel,"G",1,0);
	send(kernel,&valor,sizeof(t_valor_variable),0);

	recv(kernel,&res,1,0); //recibo una Y
	send(kernel,temp,valor,0);

	free(temp);

	recv(kernel,&res,1,0); //recibo una Y

	recv(kernel,&res,1,0); //recibo una Y o una N

	switch(res){
	case 'Y':
		recv(kernel,puntero,sizeof(t_valor_variable),0);
		return res;
	case 'N':
		printf("ERROR: no se pudo alocar\n");
		return res;
	default:
		printf("ERROR: respuesta incorrecta en ALLOC\n");
		return 'N';
	}
}
