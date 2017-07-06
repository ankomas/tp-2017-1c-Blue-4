/*
 * arquitecturaFS.h
 *
 *  Created on: 5/7/2017
 *      Author: utnso
 */

#ifndef ARQUITECTURAFS_H_
#define ARQUITECTURAFS_H_

typedef struct{
	char* puntoMontaje;
	int tamBloque;
	int bloques;
}configFS_t;

configFS_t configFS;


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
void ocuparBloque(int);

#endif /* ARQUITECTURAFS_H_ */
