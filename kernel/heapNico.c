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
		if(pag==-1){
			res=-2;
			break;
		}

		res=asignarEnPagHeap(programa,pag,tam);
	}

	return res;

}

////
////				Liberar
////

void liberarPaginaHeap(t_programa *programa,uint32_t pag,t_heap *heapKernel){
	bool _heapByPage(t_heap *self){
		return self->nPag==pag;
	}
	void _destroyByPage(t_heap* self){
		free(self);
	}
	list_remove_and_destroy_by_condition(programa->paginasHeap,(void*)_heapByPage,(void*)_destroyByPage);
	liberarPagina(programa->pcb->pid,pag);
}

int buscarSiguienteMetadataHeap(uint32_t pid, uint32_t pag,uint32_t offmetadata,uint32_t* offsig, t_metadata_heap metadata,t_metadata_heap *siguiente){
	uint32_t offlocal;

	if(metadata.free==true){
		//Puede existir un metadata despues de "metadata"?
		//printf("BUSCAR SIG %i+%i+%i<%i-%i?\n",offmetadata,metadataHeap_tam(),metadata.size,tamanioPagina,metadataHeap_tam());
		//printf("%i<%i?\n",offmetadata+metadataHeap_tam()+metadata.size,tamanioPagina-metadataHeap_tam());
		if(offmetadata+metadataHeap_tam()+metadata.size>tamanioPagina-metadataHeap_tam()){
			return 0;
		}
	}
	//metadata free=false o hubo espacio para otro metadata
	offlocal=offmetadata+metadataHeap_tam()+metadata.size;

	*offsig=offlocal;
	*siguiente=buscarMetadataHeap(pid,pag,offlocal);
	/*
	printf("BUSCAR SIG TEST\n");
	visualizarMetadata(*siguiente);
	*/
	return 1;
}

int compactar(uint32_t pid,uint32_t pag,t_metadata_heap *metadata,uint32_t offset,t_metadata_heap siguiente,t_heap* heapKernel){
	if(metadata->free==false)
		return 0;
	if(siguiente.free==false)
		return 0;

	metadata->size+=siguiente.size+metadataHeap_tam();
	guardarMetadata(*metadata,pid,pag,offset);
	heapKernel->tamDisp+=siguiente.size+metadataHeap_tam();

	return 1;
}

void compactador(t_programa *programa,uint32_t pag,t_heap* heapKernel){
	t_metadata_heap metadata,siguiente;
	uint32_t offset=0,offsetsig,res;

	metadata=buscarMetadataHeap(programa->pcb->pid,pag,0);
	res=buscarSiguienteMetadataHeap(programa->pcb->pid,pag,offset,&offsetsig,metadata,&siguiente);
/*
	printf("COMPACTADOR test\n");
	visualizarMetadata(metadata);
	visualizarMetadata(siguiente);
	printf("Offset: %i, offsig: %i\n",offset,offsetsig);
*/
	while(res){
		if(compactar(programa->pcb->pid,pag,&metadata,offset,siguiente,heapKernel)){
			//Se pudo compactar, busco otro metadata
			printf("COMPACTADO, NUEVO METADATA: \n");
			visualizarMetadata(metadata);
			res=buscarSiguienteMetadataHeap(programa->pcb->pid,pag,offset,&offsetsig,metadata,&siguiente);
/*
			printf("COMPACTADOR test 1\n");
			visualizarMetadata(siguiente);
			printf("Offset: %i, offsig: %i\n",offset,offsetsig);
*/
		}else{
			//No se pudo compactar, me muevo a otro metadata
			res=buscarSiguienteMetadataHeap(programa->pcb->pid,pag,offset,&offset,metadata,&metadata);
/*
			printf("COMPACTADOR test 2\n");
			visualizarMetadata(metadata);
			printf("Offset: %i, offsig: %i\n",offset,offsetsig);
*/
			if(res){
				//Hay mas metadatas
				res=buscarSiguienteMetadataHeap(programa->pcb->pid,pag,offset,&offsetsig,metadata,&siguiente);
/*
				printf("COMPACTADOR test 3, res: %i\n",res);
				visualizarMetadata(siguiente);
				printf("Offset: %i, offsig: %i\n",offset,offsetsig);
*/
			}
			else{
				//No hubo mas metadatas
				res=0;
			}
		}
		//usleep(1000000);
	}

	/*printf("COMPACTADOR metadata size: %i, tampag-metaheaptam: %i\n",metadata.size,tamanioPagina-metadataHeap_tam());
	visualizarMetadata(metadata);*/
	if(metadata.size==tamanioPagina-metadataHeap_tam()){
		liberarPaginaHeap(programa,pag,heapKernel);
	}
/*
	printf("TEST||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	metadata=buscarMetadataHeap(programa->pcb->pid,pag,0);
	printf("COMPACTADOR metadata size: %i, tampag-metaheaptam: %i\n",metadata.size,tamanioPagina-metadataHeap_tam());
	visualizarMetadata(metadata);
	metadata=buscarMetadataHeap(programa->pcb->pid,pag,105);
	printf("COMPACTADOR metadata size: %i, tampag-metaheaptam: %i\n",metadata.size,tamanioPagina-metadataHeap_tam());
	visualizarMetadata(metadata);
	printf("TEST||||||||||||||||||||||||||||||||||||||||||||||||||||\n");*/
	/*
	visualizarMetadata(metadata);
	metadata=buscarMetadataHeap(programa->pcb->pid,pag,110);
	printf("COMPACTADOR metadata size: %i, tampag-metaheaptam: %i\n",metadata.size,tamanioPagina-metadataHeap_tam());
	visualizarMetadata(metadata);
	metadata=buscarMetadataHeap(programa->pcb->pid,pag,165);
	printf("COMPACTADOR metadata size: %i, tampag-metaheaptam: %i\n",metadata.size,tamanioPagina-metadataHeap_tam());
	visualizarMetadata(metadata);
*/
}



int liberar(t_programa* programa,t_puntero puntero){
	t_pos pos=punteroAPos(puntero,tamanioPagina);
	t_pos posMetadata=pos;
	t_metadata_heap metadata;
	t_heap *heapKernel;
	//uint32_t offsetsig=0;

	printf("Llamada a LIBERAR\n");

	posMetadata.off-=metadataHeap_tam();
	metadata=buscarMetadataHeap(programa->pcb->pid,posMetadata.pag,posMetadata.off);

	//buscarSiguienteMetadataHeap(programa->pcb->pid,posMetadata.pag,posMetadata.off,&offsetsig,metadata,&siguiente);


	bool _heapByPage(t_heap *self){
		return self->nPag==posMetadata.pag;
	}

	heapKernel=list_find(programa->paginasHeap,(void*)_heapByPage);

	metadata.free=true;
	heapKernel->tamDisp+=metadata.size;

	programa->cantidadAlocarEjecutados++;
	programa->cantidadAlocarEjecutadosBytes += metadata.size;

	guardarMetadata(metadata,programa->pcb->pid,posMetadata.pag,posMetadata.off);

	compactador(programa,posMetadata.pag,heapKernel);

	return 1;
}

