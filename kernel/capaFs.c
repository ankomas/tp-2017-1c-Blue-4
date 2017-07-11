/*
 * capaFs.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include "main.h"

void imprimirPorConsola(){

}

uint32_t abrirFD(char* path){
	return 0;
}

void cerrarFD(uint32_t fd){
	// un archivo no puede ser borrado si existe en la TGA
	// Verificar si hay alguna entrada en la TGA que tenga "abierto" en 0 y sacarlo de la tabla
}

void escribirFD(uint32_t fd){
	//permisos
	// si es fd == 1, ejecutar imprimirPorConsola()
}

void leerFD(uint32_t fd){
	//permisos
}
