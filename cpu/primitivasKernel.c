/*
 * primitivasKernel.c
 *
 *  Created on: 13/7/2017
 *      Author: utnso
 */

#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "conexiones.h"
#include "primitivas.h"
#include "primitivasKernel.h"
#include "capaKernel.h"
#include "capaMemoria.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

//Semaforos

void dummy_wait(t_nombre_semaforo identificador_semaforo){
	if(finPrograma_global!='Y')
		return;
	char res;
	printf("Llamada a "YEL"WAIT"RESET" %s\n",identificador_semaforo);

	res=semWait(identificador_semaforo);

	switch(res){
	case 'Y':
		printf("\n");
		return;
	case 'N':
		setExitCode(&pcb_global,"el semaforo no existe",15);
		printf("\n");
		return;
	case 'B':
		printf("\n");
		finPrograma_global='B';
		return;
	}
}

void dummy_signal(t_nombre_semaforo identificador_semaforo){
	if(finPrograma_global!='Y')
		return;
	char res;

	printf("Llamada a "YEL"SIGNAL"RESET" %s\n",identificador_semaforo);

	res=semSignal(identificador_semaforo);

	switch(res){
	case 'Y':
		printf("\n");
		return;
	case 'N':
		setExitCode(&pcb_global,"el semaforo no existe",15);
		printf("\n");
		return;
	}
}

//Heap

t_puntero dummy_reservar(t_valor_variable espacio){
	if(finPrograma_global!='Y')
		return -1;
	char res;
	t_pos pos;
	t_puntero puntero;
	printf("Llamada a "YEL"RESERVAR"RESET": %i\n",espacio);
	res=alloc(espacio,&puntero);

	switch(res){
	case 'Y':
		pos=punteroAPos(puntero,tamPag_global);
		printf("Puntero: %i (%i,%i,%i)\n",puntero,pos.pag,pos.off,espacio);
		printf("\n");
		return puntero;
	case 'N':
		setExitCode(&pcb_global,"error al reservar en heap",16);
		printf("\n");
		return -1;
	default:
		setExitCode(&pcb_global,"error al reservar en heap",16);
		printf("\n");
		return -1;
	}
}

void dummy_liberar(t_puntero puntero){
	if(finPrograma_global!='Y')
		return;
	t_pos pos=punteroAPos(puntero,tamPag_global);
	char res;

	printf("Llamada a "YEL"LIBERAR"RESET": %i (%i,%i,%i)\n",puntero,pos.pag,pos.off,pos.size);
	res=liberar(puntero);

	switch(res){
	case 'Y':
		printf("\n");
		return;
	case 'N':
		setExitCode(&pcb_global,"error al liberar en heap",16);
		printf("\n");
		return;
	default:
		setExitCode(&pcb_global,"error al liberar en heap",16);
		printf("\n");
		return;
	}
}

//Filesystem

t_descriptor_archivo dummy_abrir(t_direccion_archivo direccion, t_banderas flags){
	if(finPrograma_global!='Y')
		return -1;
	int res;

	printf("Llamada a "YEL"ABRIR"RESET"\n");
	printf("Direccion: %s\n",direccion);
	printf("L: %i E: %i C: %i\n",flags.lectura,flags.escritura,flags.lectura);

	res=abrirArchivo(direccion,flags);

	if(res==-1){
		setExitCode(&pcb_global,"error al abrir archivo",2);
		printf("\n");
		return -1;
	}
	printf("File descriptor: %i\n",res);
	printf("\n");
	return res;
}

void dummy_borrar(t_descriptor_archivo descriptor_archivo){
	if(finPrograma_global!='Y')
		return;
	int res;
	printf("Llamada a "YEL"BORRAR"RESET" %i\n",descriptor_archivo);

	res=borrarArchivo(descriptor_archivo);

	if(res==-1){
		setExitCode(&pcb_global,"error al borrar archivo",17);
	}
	printf("\n");

}

void dummy_cerrar(t_descriptor_archivo descriptor_archivo){
	if(finPrograma_global!='Y')
		return;
	int res;
	printf("Llamada a "YEL"CERRAR"RESET" %i\n",descriptor_archivo);
	res=cerrarArchivo(descriptor_archivo);

	if(res==-1){
		setExitCode(&pcb_global,"error al cerrar archivo",18);
	}
	printf("\n");
}

void dummy_moverCursor(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion){
	if(finPrograma_global!='Y')
		return;
	int res;
	printf("Llamada a "YEL"MOVER CURSOR"RESET" %i\n",descriptor_archivo);
	res=moverCursor(descriptor_archivo,posicion);

	if(res==-1){
		setExitCode(&pcb_global,"error al cerrar archivo",18);
	}
	printf("\n");

}

void dummy_escribir(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio){
	if(finPrograma_global!='Y')
		return;
	int res,i;
	printf("Llamada a "YEL"ESCRIBIR"RESET" %i,%i\n",descriptor_archivo,tamanio);
	printf("Info a escribir: ");
	for(i=0;i<tamanio;i++)
		printf("%c",((char*)informacion)[i]);
	printf("\n");
	res=escribirArchivo(descriptor_archivo,informacion,tamanio);

	if(res==-1){
		setExitCode(&pcb_global,"error al escribir archivo",19);
	}
	printf("\n");
}

void dummy_leer(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio){
	if(finPrograma_global!='Y')
		return;
	char *data=NULL;
	t_pos pos=punteroAPos(informacion,tamPag_global);
	printf("Llamada a "YEL"LEER"RESET" %i,%i (%i,%i),%i\n",descriptor_archivo,informacion,pos.pag,pos.off,tamanio);
	int res,i;

	res=leerArchivo(descriptor_archivo, tamanio, &data);

	if(res==-1){
		setExitCode(&pcb_global,"error al leer archivo",21);
		return;
	}

	printf("Info leida: ");
	for(i=0;i<tamanio;i++)
		printf("%c",data[i]);
	printf("\n");

	res=guardarEnMemoria(memoria,pcb_global.pid,pos.pag,pos.off,tamanio,data);
	free(data);
	if(res==-1){
		setExitCode(&pcb_global,"fallo al escribir en memoria",12);
		printf("\n");
	}
}

t_descriptor_archivo (*AnSISOP_abrir)(t_direccion_archivo direccion, t_banderas flags);
void (*AnSISOP_borrar)(t_descriptor_archivo descriptor_archivo);
void (*AnSISOP_cerrar)(t_descriptor_archivo descriptor_archivo);
void (*AnSISOP_moverCursor)(t_descriptor_archivo descriptor_archivo, t_valor_variable posicion);
void (*AnSISOP_escribir)(t_descriptor_archivo descriptor_archivo, void* informacion, t_valor_variable tamanio);
void (*AnSISOP_leer)(t_descriptor_archivo descriptor_archivo, t_puntero informacion, t_valor_variable tamanio);

