/*
 * arquitecturaFS.c
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

char* rutaEnPuntoMontaje(char* path){
	char* ruta = string_new();
	string_append(&ruta,configFS.puntoMontaje);
	string_append(&ruta,path);
	return ruta;
}

void leerConfig(){
	configFS.puntoMontaje = malloc(100);
	configFS.puntoMontaje = obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUNTO_MONTAJE");
	anuncio(concat(2,"Puerto: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO")));
	anuncio(concat(2,"Punto de Montaje: ", configFS.puntoMontaje ));
}

void leerMetadata(){
	char* ruta = rutaEnPuntoMontaje("/Metadata/Metadata.bin");
	configFS.tamBloque = obtenerConfiguracion(ruta,"TAMANIO_BLOQUES");
	configFS.bloques = obtenerConfiguracion(ruta,"CANTIDAD_BLOQUES");
	free(ruta);
	printf("Tamaño de bloque: %i\n", configFS.tamBloque);
	printf("Cantidad de bloques: %i\n", configFS.bloques);
}

void inicializarFS(){
	leerConfig();
	leerMetadata();
}

FILE* abrirBitmap(char* modo){
	FILE* bitmap;
	bitmap = fopen(rutaEnPuntoMontaje("/Metadata/Bitmap.bin"), modo);
	return bitmap;
}

int bloqueLibre(int bloque){
	FILE* bitmap = abrirBitmap("rb");
	fseek(bitmap, bloque, SEEK_SET);
	int bit = fgetc(bitmap) - '0';
	fclose(bitmap);
	return bit;
}

void ocuparBitmap(int bloque){
	FILE* bitmap = abrirBitmap("wb");
	fseek(bitmap, bloque, SEEK_SET);
	fwrite('1',1,1,bitmap);
	fclose(bitmap);
}

void liberarBitmap(int bloque){
	FILE* bitmap = abrirBitmap("wb");
	fseek(bitmap, bloque, SEEK_SET);
	fwrite('0',1,1,bitmap);
	fclose(bitmap);
}



