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
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <commons/string.h>
#include <commons/bitarray.h>
#include <blue4-lib.h>
#include "arquitecturaFS.h"

char* rutaEnPuntoMontaje(char* carpeta, char* path){
	char* ruta = string_new();
	if(path){
	string_append(&ruta,configFS.puntoMontaje);
	string_append(&ruta,carpeta);
	string_append(&ruta,path);
	return ruta;
	}

	string_append(&ruta,configFS.puntoMontaje);
	string_append(&ruta,carpeta);
	return ruta;
}

void leerConfig(){
	//configFS.puntoMontaje = malloc(100);
	configFS.puntoMontaje = rutaAbsolutaDe(obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUNTO_MONTAJE"));
	char* config_string=obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO");
	char* concat_string = concat(2,"Puerto: ",config_string);
	anuncio(concat_string);
	free(concat_string);
	concat_string=concat(2,"Punto de Montaje: ", configFS.puntoMontaje);
	anuncio( concat_string);
	free(concat_string);
}

void leerMetadata(){
	char* ruta = rutaEnPuntoMontaje("/Metadata","/Metadata.bin");
	configFS.tamBloque = obtenerConfiguracion(ruta,"TAMANIO_BLOQUES");
	configFS.bloques = obtenerConfiguracion(ruta,"CANTIDAD_BLOQUES");
	free(ruta);
	printf("Tamaño de bloque: %i\n", configFS.tamBloque);
	printf("Cantidad de bloques: %i\n", configFS.bloques);
}


int crearCarpeta(char* nombreCarpeta)
{
	char* ruta=rutaEnPuntoMontaje(nombreCarpeta,NULL);
	int resultado=mkdir(ruta,S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
	printf("resultado: %d \n",resultado);
	free(ruta);
	return resultado;
}


void crearBitmap(char* bitArray){
	int tamanioBitMap = configFS.bloques/8;
	//if(tamanioBitMap%8)tamanioBitMap++;
	bitMap = bitarray_create_with_mode(bitArray,tamanioBitMap,MSB_FIRST);
}


void guardarBitmap()
{
	FILE* archivo=NULL;
	char *bitmap,*ruta=rutaEnPuntoMontaje("/Metadata/","Bitmap.bin");
	archivo=fopen(ruta,"rb");
	if(archivo)
	{
		// EN ESTE CASO  YA EXISTE EL ARCHIVO Bitmap.bin!!!
		printf("archivo ya creado\n");
		free(ruta);
		return;
	}
	archivo=fopen(ruta,"wb+"); //"ab+",si quisiera escribir siempre al final del archivo
	bitmap=calloc(1,configFS.bloques);
	fwrite(bitmap,1,configFS.bloques,archivo);
	free(ruta);
	free(bitmap);
	fclose(archivo);
}




void cargarBitmap()
{
	char *bitmap,*ruta=rutaEnPuntoMontaje("/Metadata/","Bitmap.bin");
	int descriptor_bitmap = open(ruta,O_RDWR);
	struct stat mystat;

	if (fstat(descriptor_bitmap, &mystat) < 0) {
	    printf("Error al establecer fstat\n");
	    close(descriptor_bitmap);
	    free(ruta);
	    return ;
	}
	bitmap=(char*)mmap(NULL,mystat.st_size,PROT_READ|PROT_WRITE,MAP_SHARED,descriptor_bitmap,0);
	crearBitmap(bitmap);
	free(ruta);
}



void generarBloques()
{
	int numeroDeBloque=0;
	char* numero_string;
	char* ruta;
	FILE* archivo;
	while(numeroDeBloque<configFS.bloques)
	{
		numero_string=string_itoa(numeroDeBloque);
		string_append(&numero_string,".bin");
		ruta=rutaEnPuntoMontaje("/Bloques/",numero_string);
		archivo=fopen(ruta,"wb");
		free(ruta);
		free(numero_string);
		fclose(archivo);
		numeroDeBloque++;
	}
}


void crearEstructuraDeBloques()
{
	int resultado=crearCarpeta("/Bloques");
	if(resultado>=0)
	{
		generarBloques();
	}
}

void crearEstruturasDelFS()
{
	crearCarpeta("/Archivos");
	crearEstructuraDeBloques();
	guardarBitmap();
	cargarBitmap();

}


void inicializarFS(){
	leerConfig();
	leerMetadata();
	crearEstruturasDelFS();
}


int tamanioBitMap()
{
	int tamBitMap=bitarray_get_max_bit(bitMap);
	return tamBitMap;
}

int getBloqueLibre(int bloque)
{

	int bits_recorridos = 0;
	int bit_libre ;
	int tamBitMap=bitarray_get_max_bit(bitMap);

	while(bits_recorridos < tamBitMap)
	{
		bit_libre = (int) bitarray_test_bit(bitMap, bits_recorridos);
		if( bit_libre == 0)return bits_recorridos;
		bits_recorridos++ ;

	}
	return -1;
}
/*
int getBloqueLibre(){
	FILE* bitmap = abrirBitmap("rb");
	int bloque = 0;
	while(fgetc(bitmap) != '0' && bloque < configFS.bloques)
		bloque++;
	fclose(bitmap);
	if(bloque == configFS.bloques)
		return -1;
	else
		return bloque;
}
*/
void ocuparBloque(int bloque)
{
	bitarray_set_bit(bitMap,bloque);
}

void liberarBloque(int bloque)
{
	bitarray_clean_bit(bitMap,bloque);
}
