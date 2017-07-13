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


char* rutaArchivo(char* path){
	return rutaEnPuntoMontaje("/Archivos", path);
}

char* rutaBloque(char* path){
	return rutaEnPuntoMontaje("/Bloques", path);
}



int archivoValido(char* path){
	FILE* archivo;
	char* ruta=rutaArchivo(path);
	archivo = fopen(ruta, "rb");
	int booleano = (archivo != NULL);
	fclose(archivo);
	free(ruta);
	return booleano;
}

int validarArchivo(char* path){ //Completa!
	if(!archivoValido(path)){
		perror("Archivo No Encontrado");
		return -1;
	} else
		return 0;
}




char* obtenerIdentificadorDirectorio(char * ruta,uint16_t posicion){
	  //char str[1024];
	char* str=calloc(1,string_length(ruta)+1);
	//char* str=string_new();
	  uint16_t contador = 0;
	  //strcpy(str,ruta);
	  memcpy(str,ruta,string_length(ruta));
	  char * pch;
	  pch = strtok (str,"/");
	  //free(str);
	  while (pch != NULL && contador != posicion)
	  {
	    pch = strtok (NULL, "/");
	    contador++;
	  }
	  //free(str);
	  return pch;
}



void crearDirectorios(char* path)
{
	int i=0,posicion=0;
	char* directorio=obtenerIdentificadorDirectorio(path,posicion);
	char* directorioSiguiente=obtenerIdentificadorDirectorio(path,posicion+1);
	char* ruta_directorio;
	char* ruta_Archivo=string_new();
	if(directorioSiguiente==NULL)
	{
		printf("no hay directorios \n");
		//free(directorio);
		free(ruta_Archivo);
		return;
	}
	while(directorioSiguiente)
	{
		ruta_directorio=string_new();
		//printf("directorio: %s \n",directorio);
		//printf("directorioSiguiente: %s \n",directorioSiguiente);
		//free(directorioSiguiente);
		string_append(&ruta_Archivo,"/");
		string_append(&ruta_Archivo,directorio);
		string_append(&ruta_directorio,"/Archivos");
		string_append(&ruta_directorio,ruta_Archivo);
		printf("ruta_directorio: %s \n",ruta_directorio);
		crearCarpeta(ruta_directorio);
		posicion++;
		free(ruta_directorio);
		//free(directorio);
		directorio=obtenerIdentificadorDirectorio(path,posicion);
		directorioSiguiente=obtenerIdentificadorDirectorio(path,posicion+1);
		if(directorioSiguiente==NULL)
		{
			//free(directorio);
			free(ruta_Archivo);
			return;
		}
	}
	free(ruta_Archivo);
}


int crearArchivo(char* ruta)
{
	FILE* archivo;
	char *numero_string,*bloques;
	int bloqueLibre=getBloqueLibre();
	if(bloqueLibre<0)return -1;
	//crear Directorios si los hay!!
	//crearDirectorios();
	ocuparBloque(bloqueLibre);
	char* ruta_Archivo=rutaArchivo(ruta);
	archivo=fopen(ruta_Archivo,"wb");
	generarFormatoArchivo(ruta_Archivo);
	numero_string=string_itoa(bloqueLibre);
	bloques=obtenerFormatoDeBloques(NULL,numero_string);
	bloques=obtenerFormatoDeBloques(bloques,NULL);
	cambiarFormatoDeArchivo(ruta_Archivo,0,bloques);
	fclose(archivo);
	free(bloques);
	free(numero_string);
	free(ruta_Archivo);
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
void borrarArchivo(char* path){
	FILE* archivo;
	t_infoArchivo info;
	int num_bloque,posicion=0;
	char*ruta=rutaArchivo(path);
	//archivo = fopen(ruta, "rb");
	//fclose(archivo);
	info=obtenerInfoArchivo(ruta);
	while(info.bloques[posicion])
	{
		num_bloque=atoi(info.bloques[posicion]);
		liberarBloque(num_bloque);
		posicion++;
	}
	remove(ruta);
	free(ruta);
	free(info.bloques);
}



char* obtenerArchivoSegunBloque(char* numeroDeBloque)
{
	char* archivo=string_new();
	string_append(&archivo,"/");
	string_append(&archivo,numeroDeBloque);
	string_append(&archivo,".bin");
	return archivo;
}


int calcularBloque( int offset, int* nuevoOffset)
{
	int i=0;
	while(offset >= configFS.tamBloque)
	{
		offset -= configFS.tamBloque;
		i++;
	}
  *nuevoOffset = offset;
  return i;
}

int cuantosBloquesLeo(int tam, int* tamSobra){
	int i=0;
  while(tam >= configFS.tamBloque){
  tam -= configFS.tamBloque;
  i++;
  }
  *tamSobra = tam;
  if( tam >0) return i+1;
  else return i;
}


char* obtenerRutaSegunBLoque(char* num_bloque)
{
	char* ruta_bloque;
	char*ruta;
	ruta_bloque=obtenerArchivoSegunBloque(num_bloque);
	ruta=rutaEnPuntoMontaje("/Bloques",ruta_bloque);
	free(ruta_bloque);
	return ruta;
}

void* lecturaSegunTamanio(int offset, int tam,char** bloques){
	int i, offsetInicial, offsetFinal, offsetBloques, cantBloquesLeo,tamALeer, tamLeido=0;
	char* buffer = malloc(configFS.tamBloque);
	void* cadena = malloc(tam);
	char* ruta;
	offsetBloques = calcularBloque(offset, &offsetInicial);
	cantBloquesLeo= cuantosBloquesLeo(tam, &offsetFinal);
	for(i=0; i < cantBloquesLeo; i++){
	ruta=obtenerRutaSegunBLoque(bloques[i]);
  	FILE *archivo = fopen(ruta, "rb");
    tamALeer = configFS.tamBloque;
		if(offsetInicial > 0)	{
    	fseek(archivo, offsetInicial, SEEK_SET);
      tamALeer -= offsetInicial;
      offsetInicial=0;
    	}
    if(i+1 == cantBloquesLeo) tamALeer = offsetFinal;
    fread(buffer, 1, tamALeer, archivo);
    memcpy(cadena+ tamLeido, buffer, tamALeer);
    tamLeido += tamALeer;
    fclose(archivo);
    free(ruta);
  }
  free(buffer);
  return cadena;
}

void escribirEnBloques(int offset, int tam, char** bloques, char* cadena) {
	int i, offsetInicial, offsetFinal, offsetBloques, cantBloquesLeo, tamAEscribir,
			tamEscrito = 0;
	char* buffer = malloc(configFS.tamBloque);
	char* ruta;
	offsetBloques = calcularBloque(offset, &offsetInicial);
	cantBloquesLeo = cuantosBloquesLeo(tam, &offsetFinal);
	for (i = 0; i < cantBloquesLeo; i++) {
		ruta = obtenerRutaSegunBLoque(bloques[i]);
		FILE *archivo = fopen(ruta, "wb");
		tamAEscribir = configFS.tamBloque;
		if (offsetInicial > 0) {
			fseek(archivo, offsetInicial, SEEK_SET);
			tamAEscribir -= offsetInicial;
			offsetInicial = 0;
		}
		if (i + 1 == cantBloquesLeo)
			tamAEscribir = offsetFinal;
		memcpy(buffer, cadena + tamEscrito, tamAEscribir);
		fwrite(buffer, 1, tamAEscribir, archivo);
		tamEscrito += tamAEscribir;
		fclose(archivo);
		free(ruta);
	}
	free(buffer);
}

cantidadDeBloquesEn(char** bloques)
{
	int i=0;
  while(bloques[i])
  {
  i++;
  }
  return i;
}

int guardarDatos(char* path,int offset, int tam, char* texto){

	int nosirve, bloquesAPedir, maxTam;
	t_infoArchivo info=obtenerInfoArchivo(path);
	maxTam = cantidadDeBloquesEn(info.bloques)* configFS.tamBloque;
	if(offset+tam > maxTam){
		bloquesAPedir = cuantosBloquesLeo(offset+tam-maxTam, &nosirve);
		//pedirBloques(bloquesAPedir);
	}
	//escribirEnBloques(offset, tam, info.bloques, texto);
	return 0;
}


char* obtenerDatos(char* path, int offset, int tam){ //Completa! Requiere Free!
	/*
	validarArchivo(path);
	char* buffer = malloc(tam);
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "rb");
	fseek(archivo, offset, SEEK_SET);
	fread(buffer, 1, tam, archivo);
	fclose(archivo);
	return buffer;
	*/
	FILE* archivo;
	char* data;
	t_infoArchivo info=obtenerInfoArchivo(path);
	if((offset+tam)>info.tamanio)
	{
		free(info.bloques);
		return NULL;
	}
	data=lecturaSegunTamanio(offset,tam,info.bloques);
	return data;

}

/*void guardarDatos(char* path, int offset, int tam, char* buffer){ //Completa!
	validarArchivo(path);
	FILE* archivo;
	archivo = fopen(rutaArchivo(path), "wb");
	fseek(archivo, offset, SEEK_SET);
	fwrite(buffer, 1, tam, archivo);
	fclose(archivo);
}*/

