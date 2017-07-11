/*
 * heap.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#include "heap.h"

heapMetadata* crearMetadata(){
	heapMetadata *nuevoMetadata = malloc(sizeof(heapMetadata));
	nuevoMetadata->isFree = true;
	nuevoMetadata->size = tamanioPagina-sizeof(heapMetadata);
	return nuevoMetadata;
}

void iniciarBloqueHeap(paginaHeap* unaPagina){
	heapMetadata *nuevoMetadata = crearMetadata();
	unaPagina->contenido = malloc(tamanioPagina);
	memcpy(unaPagina->contenido,nuevoMetadata,sizeof(heapMetadata));

	bloque * nuevoBloque = malloc(sizeof(bloque));
	unaPagina->bloques = list_create();
	nuevoBloque->posicionInicioBloque = 0;
	nuevoBloque->metadata = nuevoMetadata;
	nuevoBloque->tamanioData = nuevoBloque->metadata->size;
	list_add(unaPagina->bloques,nuevoBloque);
}

int guardarDataHeap(paginaHeap* unaPagina,void* data,int32_t tamData){
	int i = 0;
	if(list_size(unaPagina->bloques) > 0){
		while(i < list_size(unaPagina->bloques)){
			bloque * bloqueAux = list_get(unaPagina->bloques,i);
			if(bloqueAux->metadata->isFree == 1 && bloqueAux->metadata->size >=tamData){

				// Manejo la asignacion del bloque de metadata al final de la pagina
				int j = list_size(unaPagina->bloques)-1; // Estoy en el ultimo elemento
				if(i == j){
					if(bloqueAux->metadata->size+sizeof(heapMetadata) >=tamData){
						heapMetadata* metadataFree = crearMetadata();
						memcpy(unaPagina->contenido+bloqueAux->posicionInicioBloque,metadataFree,sizeof(heapMetadata));
					} else {
						return -1; // No hay espacio en esta pagina
					}
				}
				// Fin Manejo la asignacion del bloque de metadata al final de la pagina

				bloqueAux->metadata->isFree = false;
				if(bloqueAux->metadata->size-tamData > sizeof(heapMetadata)){
					// Si hay fragmentacion externa menor a un sizeof(heapMetadata)
					// Se le asigna el bloque entero al pedido
					bloqueAux->tamanioData = tamData;
				}else{
					bloqueAux->metadata->size = tamData;
					bloqueAux->tamanioData = tamData;
					heapMetadata* metadataFree = crearMetadata();
					memcpy(unaPagina->contenido+bloqueAux->posicionInicioBloque,metadataFree,sizeof(heapMetadata));
				}
				memcpy(
					unaPagina->contenido+
					bloqueAux->posicionInicioBloque+
					sizeof(heapMetadata),
					data,
					tamData
				);
				list_add(unaPagina->bloques,bloqueAux);

				//crear nuevo bloque heap
				return 0; //Guardado con exito
			}
			i++;
		}
		// Antes de darnos por vencido, tratamos de ver si compactando tenemos espacio
		if(memoriaLibreContigua(unaPagina) >= tamData){
			if(compactar(unaPagina) == 1)
				guardarDataHeap(unaPagina,data,tamData);
		}
		return -1; // No hay espacio en esta pagina
	}
	return -2; // La pagina no esta inicializada
}

void* leerDataHeap(paginaHeap* unaPagina,uint32_t offset){
	return unaPagina->contenido+sizeof(heapMetadata)+offset;
}

void liberarMemoria(){

}

/*void bajarHeapAMemoria(){

}*/

int memoriaLibre(paginaHeap* unaPagina){
	int i = 0;
	int memoriaLibreTotal = 0;
	if(list_size(unaPagina->bloques) > 0){
		while(i < list_size(unaPagina->bloques)){
			bloque * bloqueAux = list_get(unaPagina->bloques,i);
			if(bloqueAux->metadata->isFree == 1)
				memoriaLibreTotal += bloqueAux->metadata->size;
		i++;
		}
	}
	return memoriaLibreTotal;
}

int memoriaLibreContigua(paginaHeap* unaPagina){
	int i = 0;
	int memoriaLibreTotalContigua = 0;
	int mayorAnterior;
	if(list_size(unaPagina->bloques) > 0){
		while(i < list_size(unaPagina->bloques)){
			int j = i;
			while(j < list_size(unaPagina->bloques)){
				bloque * bloqueAux = list_get(unaPagina->bloques,i);
				if(bloqueAux->metadata->isFree == 1)
					memoriaLibreTotalContigua += bloqueAux->metadata->size+sizeof(paginaHeap);
				else
					break;
			j++;
			}
			if(memoriaLibreTotalContigua > mayorAnterior)
				mayorAnterior = memoriaLibreTotalContigua;
			memoriaLibreTotalContigua = 0;
		i++;
		}
	}
	//Le quito el sizeof del metadata que quedaria si se compactara
	return memoriaLibreTotalContigua-sizeof(paginaHeap);
}

int memoriaReservada(paginaHeap* unaPagina){
	return tamanioPagina-memoriaLibre(unaPagina);
}

int compactar(paginaHeap* unaPagina){
	int i = 0;
	int huboCompactacionParcial = 1;
	int sirvioCompactar = 0;
	while(huboCompactacionParcial == 1){
		huboCompactacionParcial = 0;
		if(list_size(unaPagina->bloques) > 1){
			while(i < list_size(unaPagina->bloques)-1){
				bloque * bloqueAux = list_get(unaPagina->bloques,i);
				int j = i+1;
				bloque * bloqueAux2 = list_get(unaPagina->bloques,j);
				if(bloqueAux->metadata->isFree == 1 && bloqueAux2->metadata->isFree == 1){
					huboCompactacionParcial = 1;
					sirvioCompactar = 1;
					bloqueAux->metadata->size += sizeof(heapMetadata)+bloqueAux2->metadata->size;
					list_remove(unaPagina->bloques,j);//Elimino el segundo bloque
				}
				i++;
			}
		}
	}
	return sirvioCompactar;
	//reorganizar todo en un nuevo void*data y eliminar el anterior
}
