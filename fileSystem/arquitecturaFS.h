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
 * Agrega el path a la carpeta de archivos
 */
char* rutaArchivo(char*);

/**
 * BloqueLibre(int bloque)
 *
 * Devuelve 1 si el bloque esta libre
 */
int bloqueLibre(int);

#endif /* ARQUITECTURAFS_H_ */
