/*
 * capaFs.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include "capaFs.h"

void imprimirPorConsola(){

}

uint32_t abrirFD(t_programa* unPrograma,char* path, char* permisos){
	// En desarrollo
	unPrograma->FDCounter++;
	t_entradaTAP * nuevaEntradaTAP = malloc(sizeof(nuevaEntradaTAP));
	nuevaEntradaTAP->flags = permisos;

	t_entradaTGA * nuevaEntradaTGA;
	if(dictionary_has_key(tablaGlobalArchivos,path)){
		nuevaEntradaTGA = dictionary_get(tablaGlobalArchivos,path);
		nuevaEntradaTGA->abierto++;
	} else {
		nuevaEntradaTGA = malloc(sizeof(nuevaEntradaTGA));
		nuevaEntradaTGA->archivo = path;
		nuevaEntradaTGA->abierto = 0;
		nuevaEntradaTGA->indice = GlobalFDCounter;
		dictionary_put(tablaGlobalArchivos,path,nuevaEntradaTGA);
		GlobalFDCounter++;
	}

	nuevaEntradaTAP->globalFD = nuevaEntradaTGA->indice;
	nuevaEntradaTAP->indice = unPrograma->FDCounter;

	return nuevaEntradaTAP->indice;
}

void cerrarFD(t_programa* unPrograma,uint32_t fd){
	// Verificar si hay alguna entrada en la TGA que tenga "abierto" en 0 y sacarlo de la tabla
}

void escribirFD(t_programa* unPrograma,uint32_t fd, uint32_t offset, uint32_t tamanio){
	//checkear que el fd tenga permisos de escritura
	// si es fd == 1, ejecutar imprimirPorConsola()
}

void leerFD(t_programa* unPrograma,uint32_t fd, uint32_t offset, uint32_t tamanio){
	//checkear que el fd tenga permisos de escritura
}
