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

//Algunas Funciones estan completas, ninguna esta probada!

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

void crearArchivo(char* path){ //No sabia que hacer

}

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
	return buffer;
}

int guardarDatos(char* path, int offset, int tam, char* buffer){ //Completa!
	validarArchivo(path);
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "wb");
	fseek(archivo, offset, SEEK_SET);
	fwrite(buffer, 1, tam, archivo);
	return 0;
}


