/*
 * arquitecturaFS.h
 *
 *  Created on: 5/7/2017
 *      Author: utnso
 */

#ifndef ARQUITECTURAFS_H_
#define ARQUITECTURAFS_H_

#include <commons/bitarray.h>

typedef struct{
	char* puntoMontaje;
	int tamBloque;
	int bloques;
}configFS_t;

typedef struct{
	char** bloques;
	int tamanio;
}t_infoArchivo;

configFS_t configFS;

t_bitarray* bitMap;

/**
 * inicializa las configuraciones del fileSystem
 */
void inicializarFS();


/**
 * Agrega el path a la carpeta dada
 *
 * Parametros: char* carpeta -> carpeta
 * 				char* path	 -> path del archivo
 */
char* rutaEnPuntoMontaje(char*, char*);


/**
 * crea un bitArray
 */
void crearBitmap();

/**
 * BloqueLibre(int bloque)
 *
 * Devuelve 1 si el bloque esta libre
 */
int bloqueLibre(int);

/**
 * Devuelve el archivo bitmap, abierto en el modo dado
 *
 * Parametro: modo de abrir el archivo ("rb", "wb", etc...)
 */
FILE* abrirBitmap(char*);

/**
 * Devuelve el proximo bloque libre
 */
int getBloqueLibre();


/**
 * Crea una carpeta
 * @param nombreCarpeta
 * @return 0 en el caso exito al crearla || -1 en el caso de que la carpera ya exita
 */
int crearCarpeta(char* );

/**
 * Obtiene la info de un archivo segun el FS_SADICA
 * @param ruta
 * @return t_infoArchivo (Importante,liberar el campo bloques de este struct!!!)
 */
t_infoArchivo obtenerInfoArchivo(char* );

/**
 * Cambia la info contenida en un archivo segun el FS_SADICA
 * @param ruta
 * @param tamanio
 * @param bloques
 */
void cambiarFormatoDeArchivo(char* ,int ,char* );

/**
 * Genera el formato que contiene la info de un archivo segun el FS_SADICA
 * @param ruta
 */
void generarFormatoArchivo(char* );

/**
 * Marca un bloque como libre en el bitArray
 * @param bloque
 */
void liberarBloque(int );

/**
 * Marca un bloque como ocupado en el bitArray
 * @param bloque
 */
void ocuparBloque(int );

/**
 * Devuelve la cantidad de bloques libres que existen en el bitArray
 * @return total_bloques_libres
 */
int cantidad_bloquesLibres();


/**
 * Devuelve la expresion de los bloques segun el FS_SADICA,segun los parametros que se le pasan
 * @param inicio
 * @param numeroAAagregarse
 * @return expresion segun los parametros iniciados
 */
char* obtenerFormatoDeBloques(char* ,char* );

#endif /* ARQUITECTURAFS_H_ */
