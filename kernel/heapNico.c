/*
 * heapNico.c
 *
 *  Created on: 13/7/2017
 *      Author: utnso
 */
#include <stdio.h>
#include "capaMemoria.h"
#include "heapNico.h"

t_puntero posAPuntero(t_pos pos,uint32_t tamPag){
	return pos.pag*tamPag+pos.off;
}

t_pos punteroAPos(t_puntero puntero, uint32_t tamPag){
	//printf("PUNTERO A POS: %i\n",puntero);
	t_pos pos;
	pos.pag = puntero/tamPag;
	pos.off = puntero - tamPag * pos.pag;
	pos.size = 0;
	//printf("POS: %i, OFF: %i, SIZE:%i\n",pos.pag,pos.off,pos.size);
	return pos;
}

t_size metadataHeap_tam(){
	return sizeof(uint32_t)+sizeof(bool);
}
/*
int buscarPaginaAdecuada(t_list *heap,uint32_t tam,int *desde,int inicioHeap){
	t_heap *aux;
	printf("BUSCAR PAGINA ADECUADA\n");
	printf("Tam lista heap: %i, desde: %i, inicio heap: %i\n",list_size(heap),*desde,inicioHeap);
	while(*desde<list_size(heap)+inicioHeap){
		aux=list_get(heap,*desde-inicioHeap);
		if(aux->tamDisp>=tam)
			return aux->nPag;
		*desde+=1;
	}
	return -1;

}
*/
int buscarPaginaAdecuada(t_list *heap,uint32_t tam,int *desde,uint32_t inicioHeap){
	t_heap *aux=NULL;

	printf("BUSCAR PAGINA ADECUADA\n");
	printf("Tam lista heap: %i, desde: %i, inicio heap: %i\n",list_size(heap),*desde,inicioHeap);
	while(1){
		bool _heapByPage(t_heap *self){
			return self->nPag==*desde;
		}
		//Esta la pag x en el heap?
		aux=list_find(heap,(void*)_heapByPage);

		if(aux==NULL)
			return -1;
		printf("Encontre la pag %i del heap, tamanio disponible: %i\n",aux->nPag,aux->tamDisp);
		//La pag x del heap tiene tam suficiente?
		if(aux->tamDisp>=tam)
			return *desde;
		printf("El tamanio pedido %i es mayor al tamanio disponible %i\n",tam,aux->tamDisp);
		*desde+=1;
}
}

char* metadataHeapAStream(t_metadata_heap metadata){
	char* res=malloc(metadataHeap_tam());

	memcpy(res,&metadata.free,sizeof(bool));
	memcpy(res+sizeof(bool),&metadata.size,sizeof(uint32_t));

	return res;
}

t_metadata_heap streamAMetadataHeap(char* stream){
	t_metadata_heap res;
	memcpy(&res.free,stream,sizeof(bool));
	memcpy(&res.size,stream+sizeof(bool),sizeof(uint32_t));

	return res;
}

int nuevaPagHeap(t_programa *programa,uint32_t tam,uint32_t pag){
	int res = pedirPagias(programa->pcb->pid,1);
	if(res<0)
		return -1;
	uint32_t pagPedida=pag;
	//crea estructuras
	t_heap *nuevoHeap=malloc(sizeof(t_heap));
	t_metadata_heap nuevoMetaHeap;
	char *metadataStream;

	nuevoHeap->pid=programa->pcb->pid;
	nuevoHeap->nPag=pagPedida;
	nuevoHeap->tamDisp=tamanioPagina-metadataHeap_tam();

	nuevoMetaHeap.free=true;
	nuevoMetaHeap.size=tamanioPagina-metadataHeap_tam();

	metadataStream=metadataHeapAStream(nuevoMetaHeap);

	printf("NUEVA PAG HEAP Guardar en memoria el metadata\n");
	guardarEnMemoria(idUMC , programa->pcb->pid,pagPedida,0,metadataHeap_tam(),metadataStream);

	list_add(programa->paginasHeap,nuevoHeap);

	return pagPedida;
}

t_metadata_heap buscarMetadataHeap(uint32_t pid,uint32_t pag,uint32_t offset){
	t_metadata_heap metadata;
	char* buffer;
	package_t paquete;

	cargarDeMemoria(idUMC,pid,pag, offset, metadataHeap_tam(), &paquete);
	buffer=paquete.data;
	metadata=streamAMetadataHeap(buffer);
	free(buffer);

	return metadata;
}

void guardarMetadata(t_metadata_heap metadata,uint32_t pid,uint32_t pag,uint32_t off){
	char *stream=metadataHeapAStream(metadata);

	guardarEnMemoria(idUMC ,pid,pag,off,metadataHeap_tam(),stream);
}

void visualizarMetadata(t_metadata_heap metadata){
	printf("Metadata ");
	if(metadata.free==true)
		printf("FREE\n");
	else
		printf("USED\n");
	printf("Metadata size: %i\n",metadata.size);
}

int asignarEnPagHeap(t_programa *programa,uint32_t pag, uint32_t tam){
	t_metadata_heap metadata,newMetadata;
	int offset=0,offset2=0;
	t_pos pos;
	t_heap* heapKernel;

	while(offset<=tamanioPagina){
	metadata=buscarMetadataHeap(programa->pcb->pid,pag,offset);
	visualizarMetadata(metadata);

	if(metadata.free==true){
		if(metadata.size>=tam+metadataHeap_tam()){
				//crear estructura a la derecha con size restante
				newMetadata.free=true;
				newMetadata.size=metadata.size-tam-metadataHeap_tam();

				//cambiar a ocupado
				metadata.free=false;
				//restar el size
				metadata.size=tam;

				offset2=offset+metadataHeap_tam(); // guardo el offset despues de la estructura en memoria

				guardarMetadata(metadata,programa->pcb->pid,pag,offset);
				offset+=metadataHeap_tam()+metadata.size;

				guardarMetadata(newMetadata,programa->pcb->pid,pag,offset);
				offset+=metadataHeap_tam();

				bool _heapByPage(t_heap *self){
					return self->nPag==pag;
				}

				heapKernel=list_find(programa->paginasHeap,(void*)_heapByPage);
				heapKernel->tamDisp-=tam+metadataHeap_tam();

				pos.pag=pag;
				pos.off=offset2;
				pos.size=tam;

				printf("Asignar en heap correcto, pos: %i (%i,%i,%i)\n",posAPuntero(pos,tamanioPagina),pos.pag,pos.off,pos.size);
				return posAPuntero(pos,tamanioPagina);
			}
		}
	offset+=metadata.size+metadataHeap_tam();
	printf("OFFSET AHORA ES: %i\n",offset);
	}

	return -1;
}

t_puntero allocEnHeap(t_programa *programa,uint32_t tam){
	int pag;
	int res=-1;
	int inicioHeap=programa->paginasCodigo+tamanioStack;
	int desde=inicioHeap;


	printf("ALLOC EN HEAP\n");
	printf("Tam: %i, tamanio pagina: %i, metadata tam: %i\n",tam,tamanioPagina,metadataHeap_tam());
	if(tam>tamanioPagina-metadataHeap_tam()*2){
		return -1;
	}

	while(res==-1){
		//todo hacelo bien
		printf("\n");
		pag=buscarPaginaAdecuada(programa->paginasHeap,tam,&desde,inicioHeap);
		printf("Res buscar pag adecuada: %i, valor desde: %i\n",pag,desde);
		printf("\n");
		if(pag==-1)
			pag=nuevaPagHeap(programa,tam,desde);

		res=asignarEnPagHeap(programa,pag,tam);
	}

	return res;

}

