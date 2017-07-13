/*
 * operacionesFS.h
 *
 *  Created on: 5/7/2017
 *      Author: utnso
 */

#ifndef OPERACIONESFS_H_
#define OPERACIONESFS_H_

typedef struct{
	int tam;
	int* bloques;
}dataArchivo_t;



/**
 * Valida si existe un archivo
 * Parametro: char* path	-> Ubicacion del archivo
 *
 * Retorno: int 			-> -1 archivo no encontrado, 0 OK!
 */
int validarArchivo(char*);

/**
 * Crea un archivo
 * Parametro: char* path	-> Ubicacion del archivo
 * Retorno: -1 para error, 0 para OK!
 */
int crearArchivo(char*);

/**
 * Borra un archivo y todo lo relacionado con el
 * Parametro: char* path	-> Ubicacion del archivo
 *
 */
void borrarArchivo(char* );

/**
 * Lee parte de un archivo
 * Parametros:	char* path	-> Ubicacion del archivo
 * 				int offset	-> Desplazamiento
 * 				int tam		-> Cantidad de bytes a leer
 *
 * Retorno: char*	-> Stream deseado!
 */
char* obtenerDatos(char*, int, int);

/**
 * Escribe un archivo
 * Parametros: 	char* path	-> Ubicacion del archivo
 * 				int offset	-> Desplazamiento
 * 				int tam		-> Cantidad de bytes a escribir
 * 				char* buffer-> Datos a escribir
 */
int guardarDatos(char*, int, int, char*);

#endif /* OPERACIONESFS_H_ */
