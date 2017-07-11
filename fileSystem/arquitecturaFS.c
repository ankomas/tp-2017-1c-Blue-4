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
#include <commons/collections/dictionary.h>
#include <blue4-lib.h>
#include "arquitecturaFS.h"
#include "conexiones.h"

t_config *config;

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
	char* ruta,*string_configuracion;
	ruta= rutaAbsolutaDe("config.cfg");
	config=config_create(ruta);
	string_configuracion=obtenerConfiguracionString(config,"PUNTO_MONTAJE");
	configFS.puntoMontaje = rutaAbsolutaDe(string_configuracion);
	free(string_configuracion);
	char* config_string=obtenerConfiguracionString(config,"PUERTO");
	char* concat_string = concat(2,"Puerto: ",config_string);
	anuncio(concat_string);
	free(concat_string);
	concat_string=concat(2,"Punto de Montaje: ", configFS.puntoMontaje);
	anuncio( concat_string);
	free(ruta);
	free(concat_string);
}

void leerMetadata(){
	char* ruta = rutaEnPuntoMontaje("/Metadata","/Metadata.bin");
	t_config* elConfig = config_create(ruta);
	configFS.tamBloque = obtenerConfiguracion(elConfig,"TAMANIO_BLOQUES");
	configFS.bloques = obtenerConfiguracion(elConfig,"CANTIDAD_BLOQUES");
	free(ruta);
	printf("Tamaño de bloque: %i\n", configFS.tamBloque);
	printf("Cantidad de bloques: %i\n", configFS.bloques);
	config_destroy(elConfig);
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


int cantidad_bloquesLibres(){

	int bloques_libres = 0;
	int bloque_libre;
	int bit = 0;
	int tamMaximo=tamanioBitMap();
	while(bit < tamMaximo)
	{
		bloque_libre = bitarray_test_bit(bitMap,bit);
		//printf("%i",bloque_libre);
		if(bloque_libre == 0)bloques_libres ++;
		bit++;
	}
	return bloques_libres;
}



void ocuparBloque(int bloque)
{
	bitarray_set_bit(bitMap,bloque);
}

void liberarBloque(int bloque)
{
	bitarray_clean_bit(bitMap,bloque);
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


void generarFormatoArchivo(char* ruta)
{
	t_config elConfig;
	t_dictionary* diccionario = dictionary_create();
	dictionary_put(diccionario,"TAMANIO",0);
	dictionary_put(diccionario,"BLOQUES",0);
	elConfig.properties=diccionario;
	elConfig.path=ruta;
	config_save_in_file(&elConfig,ruta);
	dictionary_destroy(diccionario);
}


void cambiarFormatoDeArchivo(char* ruta,int tamanio,char* bloques)
{
	t_config* elConfig = config_create(ruta);
	char* tamanio_string=string_itoa(tamanio);
	config_set_value(elConfig,"TAMANIO",tamanio_string);
	config_set_value(elConfig,"BLOQUES",bloques);
	config_save(elConfig);
	free(tamanio_string);
	config_destroy(elConfig);
}

//todo liberar el char** del t_infoArchivo cuando se deje de usar!!!
t_infoArchivo obtenerInfoArchivo(char* ruta)
{
	t_infoArchivo info;
	t_config* elConfig = config_create(ruta);
	int tamanio = config_get_int_value(elConfig,"TAMANIO");
	char** bloques = config_get_array_value(elConfig,"BLOQUES");
	info.bloques=bloques;
	info.tamanio=tamanio;
	config_destroy(elConfig);
	return info;
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
