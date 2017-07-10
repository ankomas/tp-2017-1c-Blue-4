/*
 * operacionesFS.c
 *
 *  Created on: 5/7/2017
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
#include <commons/string.h>
#include <blue4-lib.h>
#include "arquitecturaFS.h"
#include "operacionesFS.h"

//Algunas Funciones estan completas, ninguna esta probada!

char* rutaArchivo(char* path){
	return rutaEnPuntoMontaje("/Archivos", path);
}

char* rutaBloque(char* path){
	return rutaEnPuntoMontaje("/Bloques", path);
}



int archivoValido(char* path){
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "rb");
	int booleano = (archivo != NULL);
	fclose(archivo);
	return booleano;
}

int validarArchivo(char* path){ //Completa!
	if(!archivoValido(path)){
		perror("Archivo No Encontrado");
		return -1;
	} else
		return 0;
}

/*
int asignarBloque( char* path){
	int bloque = getBloqueLibre();
	if(bloque < 0)
		return -1;
	ocuparBloque(bloque);
	return bloque;
}
*/
/*
int crearArchivo(char* path){ //No sabia que hacer
	if(validarArchivo(path) == 0)
		return -1;
	char* bloques= "BLOQUES=";
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "wb");
	fwrite("TAMANIO=0\n" ,1 ,9, archivo);
	char* bloque = asignarBloque(path);
	string_append(&bloques, bloque);
	fwrite(bloques, 1, sizeof(*bloques), archivo);
	fclose(archivo);
	return 0;
}
*/
int borrar(char* path){ //No esta Completa! Falta borrar su metadata y cambiar los bits en el bitmap
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "rb");
	fclose(archivo);
	return remove(rutaArchivo(path))==0 ;
}

char* obtenerDatos(char* path, int offset, int tam){ //Completa! Requiere Free!
	validarArchivo(path);
	char* buffer = malloc(tam);
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "rb");
	fseek(archivo, offset, SEEK_SET);
	fread(buffer, 1, tam, archivo);
	fclose(archivo);
	return buffer;
}

void guardarDatos(char* path, int offset, int tam, char* buffer){ //Completa!
	validarArchivo(path);
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "wb");
	fseek(archivo, offset, SEEK_SET);
	fwrite(buffer, 1, tam, archivo);
	fclose(archivo);
}


