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
	if(booleano)
	fclose(archivo);
	free(ruta);
	return booleano;
}

int validarArchivo(char* path){ //Completa!
	if(!archivoValido(path)){
		log_error(logFS,"Archivo NO Encontrado");
		perror("Archivo No Encontrado");
		return -1;
	} else{
		log_trace(logFS,"Archivo Valido");
		return 0;
	}
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
		//printf("ruta_directorio: %s \n",ruta_directorio);
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


int crearArchivoo(char* ruta)
{
	FILE* archivo;
	char *numero_string,*bloques;
	int bloqueLibre=getBloqueLibre();
	if(bloqueLibre<0) return -1;
	//crear Directorios si los hay!!
	crearDirectorios(ruta);
	ocuparBloque(bloqueLibre);
	char* ruta_Archivo=rutaArchivo(ruta);
	//printf("Ruta Archivo: %s\n", ruta_Archivo);
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

int crearArchivo(char* ruta)
{
 FILE* archivo;
 char *numero_string,*bloques;
 int bloqueLibre=getBloqueLibre();
 //printf("bloqueLibre: %i\n", bloqueLibre);
 if(bloqueLibre<0)return -1;
 //crear Directorios si los hay!!
 crearDirectorios(ruta);
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
	if(validarArchivo(path)<0) {
		log_error(logFS,"NO puede ejecutarse el borrar con este archivo");
		perror("No se puede borrar un archivo que no existe");
		return;
	}
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
	log_trace(logFS,"....Archivo borrado correctamente");
}



char* obtenerArchivoSegunBloque(char* numeroDeBloque)
{
	char* archivo=string_new();
	string_append(&archivo,"/");
	string_append(&archivo,numeroDeBloque);
	string_append(&archivo,".bin");
	return archivo;
}


int cuantosBloquesRepresenta(int tam){
	int i=0;
	printf("tam : %d \n ",tam);
	while(tam >= configFS.tamBloque){
		tam -= configFS.tamBloque;
		i++;
	}
	printf("bloques a pedir : %d \n",i);
	if(tam>0) i++;
	return i;
}

/**
 * Esta funcion tiene por parametros:
 * -Offset: Donde empiezo a leer/Escbribir
 * -Tam: tam de lo que leo/Escribo
 * -BloqueInicial: En que bloque empiezo a leer/Escribir
 * -OffsetInicial: Una variable para guardar el offset del bloque que comienza
 * -OffsetFinal: Cuantos bytes leo en el ultimo bloque
 * Return: Cantidad de bloques que necesito leer/escribir, INCLUYE EL PRIMERO Y EL ULTIMO
 */
int cuantosBloquesNecesito(int offset, int tam,int* bloqueInicial ,int* offsetInicial,int* offsetFinal){
	int i=0,bloquesEvitados=0;
	printf("Offset: %i, Tam: %i\n", offset, tam);
	while(offset >= configFS.tamBloque){
		offset -= configFS.tamBloque;
		bloquesEvitados++;
	}
	*bloqueInicial = bloquesEvitados;
	*offsetInicial = offset;
	while(tam-offset >= configFS.tamBloque){ //tam = offset+tam
		i++;
		tam -= configFS.tamBloque;
	}
	if(i==0) {
		if(offset+tam > configFS.tamBloque){
			*offsetFinal = offset + tam - configFS.tamBloque;
			return 2;
		}
		else{
			*offsetFinal = offset + tam;
			return 1;
		}
	}
	if(offset>0) i++;
	if(tam>0) i++;
	*offsetFinal = tam-offset;
	return i;
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

void* lecturaSegunTamanio(int offset, int tam, char** bloques) {
	int i, offsetInicial, offsetFinal, bloqueInicial, cantBloquesLeo, tamALeer,
			tamLeido = 0;
	char* buffer = malloc(configFS.tamBloque);
	void* cadena = malloc(tam);
	char* ruta;
	cantBloquesLeo = cuantosBloquesNecesito(offset, tam, &bloqueInicial,
			&offsetInicial, &offsetFinal);
	//printf("Bloque Inicial : %i, Cuantos Bloques Leo: %i, Offset: %i, Tam: %i, Offset Inicial: %i, Offset Final: %i\n",
	//		bloqueInicial, cantBloquesLeo, offset, tam, offsetInicial, offsetFinal);
	if(cantBloquesLeo == 0) return NULL;
	for (i = 0; i < cantBloquesLeo; i++) {
		ruta = obtenerRutaSegunBLoque(bloques[bloqueInicial + i]);
		FILE *archivo = fopen(ruta, "rb");
		tamALeer = configFS.tamBloque;
		if (offsetInicial > 0) {
			fseek(archivo, offsetInicial, SEEK_SET);
			tamALeer -= offsetInicial;
			offsetInicial = 0;
		}
		if (i + 1 == cantBloquesLeo && offsetFinal > 0) tamALeer = offsetFinal;
		fread(buffer, 1, tamALeer, archivo);
		//printf("lo que se lee del fread es : %s \n",buffer);
		char* string = concat(2, "La cadena leida es: ", buffer);
		log_trace(logFS, string);
		free(string);
		memcpy(cadena + tamLeido, buffer, tamALeer);
		tamLeido += tamALeer;
		fclose(archivo);
		free(ruta);
	}
	free(buffer);
	return cadena;
}

int escribirEnBloques(int offset, int tam, char** bloques, char* cadena) {
	int i, offsetInicial, offsetFinal, bloqueInicial, cantBloquesEscribo,
			tamAEscribir, tamEscrito = 0, bitPower=0;
	char* buffer = malloc(configFS.tamBloque);
	char* ruta;
	cantBloquesEscribo = cuantosBloquesNecesito(offset, tam, &bloqueInicial,
			&offsetInicial, &offsetFinal);
	//printf("Bloque Inicial : %i, Cuantos Bloques Escribo: %i, Offset: %i, Tam: %i, Offset Inicial: %i, Offset Final: %i\n",
	//			bloqueInicial, cantBloquesEscribo, offset, tam, offsetInicial, offsetFinal);
	if(cantBloquesEscribo ==0) return -1;
	for (i = 0; i < cantBloquesEscribo; i++) {
		ruta = obtenerRutaSegunBLoque(bloques[bloqueInicial + i]);
		//printf("Escribo en bloque: %s\n", bloques[bloqueInicial + i]);
		FILE *archivo = fopen(ruta, "rb+");
		tamAEscribir = configFS.tamBloque;
		if (i + 1 == cantBloquesEscribo && offsetFinal > 0) tamAEscribir = offsetFinal;
		if (offsetInicial > 0) {
			if (i + 1 == cantBloquesEscribo && offsetFinal > 0) tamAEscribir = offsetFinal - offsetInicial;
			else tamAEscribir -= offsetInicial;
			fseek(archivo, offsetInicial, SEEK_SET); printf("Hago el fseek\n");
			offsetInicial = 0;
		}
		//printf("Tam a escribir: %i\n", tamAEscribir);
		//if(!bitPower){
		memcpy(buffer, cadena + tamEscrito, tamAEscribir);
		tamEscrito += tamAEscribir;
		//}
		//bitPower=0;
		//printf("Cadena a escribir: %s\n", buffer);
		fwrite(buffer, 1, tamAEscribir, archivo);
		char* string = concat(4, "Se escribio en el bloque ", bloques[bloqueInicial+i], " la cadena: ", buffer);
		log_trace(logFS, string);
		free(string);
		fclose(archivo);
		free(ruta);
	}
	free(buffer);
	return 0;
}

int cantidadDeBloquesEn(char** bloques)
{
	int i=0;
  while(bloques[i])
  {
  i++;
  }
  return i;
}



char* generarPrimerosBloques(char** bloques)
{
	int i=0;
	char* cadena;
	while(bloques[i])
	{
		if(i==0)
		{
			cadena=obtenerFormatoDeBloques(NULL,bloques[i]);
		}
		else cadena=obtenerFormatoDeBloques(cadena,bloques[i]);
		i++;
	}
	return cadena;
}

char* pedirBloques(int bloquesAPedir,char** bloquesAnteriores)
{
	int bloqueLibre,i=0;
	char* bloques=NULL,*numero_string;


	while(i<bloquesAPedir)
	{
		bloqueLibre=getBloqueLibre();
		printf("bloque libre: %d \n",bloqueLibre);
		if(bloqueLibre<0)return NULL;
		ocuparBloque(bloqueLibre);
		if(i==0)bloques=generarPrimerosBloques(bloquesAnteriores);
		numero_string=string_itoa(bloqueLibre);
		bloques=obtenerFormatoDeBloques(bloques,numero_string);
		free(numero_string);
		i++;
	}
	bloques=obtenerFormatoDeBloques(bloques,NULL);
	return bloques;
}

t_infoArchivo actualizarArchivo(char* path,char* bloques,int tam)
{
	cambiarFormatoDeArchivo(path,tam,bloques);
	t_infoArchivo info=obtenerInfoArchivo(path);
	return info;

}

int guardarDatos(char* path,int offset, int tam, char* texto){

	int bloquesAPedir,nuevoTam;
	//char* ruta=rutaEnPuntoMontaje("/Archivos",path);
	t_infoArchivo info=obtenerInfoArchivo(path);
	if(offset+tam>=info.tamanio)nuevoTam=offset+tam;
	else nuevoTam=info.tamanio;
	//SIGUE PIDIENDO BLOQUESS!
	printf("Bloques de offset+tam: %i\n", cuantosBloquesRepresenta(offset+tam));
	printf("Bloques de info.bloques: %i\n", cantidadDeBloquesEn(info.bloques));

	if(cuantosBloquesRepresenta(offset+tam) > cantidadDeBloquesEn(info.bloques)){
		bloquesAPedir = cuantosBloquesRepresenta(offset+tam) - cantidadDeBloquesEn(info.bloques);
		printf("bloques q pido: %d \n",bloquesAPedir);
		char* bloques=pedirBloques(bloquesAPedir,info.bloques);
		printf("los bloques son: %s \n",bloques);
		if(bloques==NULL)
		{
			free(info.bloques);
			//free(ruta);
			free(bloques);
			return -1;
		}
		free(info.bloques);
		info=actualizarArchivo(path,bloques,nuevoTam);
		printf("Offset: %i, tam: %i, info.bloques: %p, texto: %p\n",offset,tam,info.bloques,texto);
		escribirEnBloques(offset, tam, info.bloques, texto);
		//free(ruta);
		free(bloques);
		return 0;
	}
	char* bloques=generarPrimerosBloques(info.bloques);
	bloques=obtenerFormatoDeBloques(bloques,NULL);
	info = actualizarArchivo(path,bloques,nuevoTam);
	escribirEnBloques(offset, tam, info.bloques, texto);
	free(bloques);
	//free(ruta);
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

