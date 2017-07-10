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
 * Marca un bloque como ocupado
 * @param bloque
 */
void ocuparBloque(int);

/**
 * Marca un bloque como libre
 * @param bloque
 */
void liberarBloque(int );

/**
 * Crea una carpeta
 * @param nombreCarpeta
 * @return 0 en el caso exito al crearla || -1 en el caso de que la carpera ya exita
 */
int crearCarpeta(char* );

#endif /* ARQUITECTURAFS_H_ */
