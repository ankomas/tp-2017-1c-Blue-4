/*
 * test.c
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <blue4-lib.h>
#include <parser/metadata_program.h>
#include <commons/collections/list.h>
#include "pcb.h"
#include "primitivas.h"

static const char* PROGRAMA2 =
		"begin\n"
		"variables a, b\n"
		"a = 3\n"
		"b = 5\n"
		"a = b + 12\n"
		"end\n"
		"\n";

static const char* PROGRAMA =
		"begin\n"
		"variables a, b\n"
		"a = 3\n"
		"b = 5\n"
		"a = b + 12\n"
		"b <- doble a\n"
		"b = 5\n"
		"end\n"
		"\n"
		"function doble\n"
		"variables f\n"
		"f = $0 + $0\n"
		"return f\n"
		"end\n"
		"\n";

t_pcb2 testPCB(){
	int i;
	t_pcb2 pcb;
	//t_intructions intructions;
	t_metadata_program *metadata;
	t_pos pos={97,98,99};
	t_list *args,*vars;
	char *programa=strdup(PROGRAMA);
	metadata=metadata_desde_literal(programa);

	pcb.pid=1;
	pcb.pc=metadata->instruccion_inicio;
	pcb.sp=0;
	pcb.cantPagCod=2;

	pcb.ultimaPosUsada=pos;

	pcb.indiceCodigoSize=metadata->instrucciones_size;
	pcb.indiceCodigo=metadata->instrucciones_serializado;

	pcb.indiceStack=list_create();
	args=list_create();
	vars=list_create();

	pos.pag=0;
	pos.size=4;
	pos.off=4;

	list_add(args,var_create('a',pos));
	list_add(vars,var_create('b',pos));
	list_add(pcb.indiceStack,stack_create(args,vars,5,pos));

	pcb.indiceEtiquetasSize=metadata->etiquetas_size;
	pcb.indiceEtiquetas=metadata->etiquetas;

	pcb.exitCode=5000;

	package_t paquete;
	t_pcb2 pcb2;

	paquete=serializarPCB(pcb);
	printf("PAQUETE SIZE: %i\n",paquete.data_size);
	pcb2=deserializarPCB(paquete.data);

	list_destroy_and_destroy_elements(pcb.indiceStack,(void*)stack_destroy);

	free(pcb.indiceCodigo);
	free(pcb.indiceEtiquetas);

	printf("Instruccion inicio: %i\n",pcb2.pc);

	printf("Instrucciones size: %i\n",pcb2.indiceCodigoSize);

	for(i=0;i<pcb.indiceCodigoSize;i++)
		printf("Start: %i Offset: %i\n",pcb2.indiceCodigo[i].start,pcb2.indiceCodigo[i].offset);


	printf("Indice etiquetas size: %i\n",pcb2.indiceEtiquetasSize);

	printf("Posicion de doble: %i\n",metadata_buscar_etiqueta("doble",pcb2.indiceEtiquetas,pcb2.indiceEtiquetasSize));

	printf("Ultima pos usada: %i %i %i\n",pcb2.ultimaPosUsada.pag,pcb2.ultimaPosUsada.off,pcb2.ultimaPosUsada.size);

	t_stack *aux;
	aux=list_get(pcb2.indiceStack,0);
	args=aux->args;
	vars=aux->vars;

	list_destroy_and_destroy_elements(pcb2.indiceStack,(void*)stack_destroy);

	free(pcb2.indiceCodigo);
	free(pcb2.indiceEtiquetas);

	free(paquete.data);

	free(metadata);

	free(programa);

	return pcb;
}
/*
char *const conseguirDatosDeLaMemoriaMock(char *start, t_puntero_instruccion offset, t_size i);

t_pcb2 test_crearPCB(t_metadata_program *metadata){
	t_pcb2 pcb;
	t_pos pos={97,98,99};
	t_list *args,*vars;

	pcb.pid=1;
		pcb.pc=metadata->instruccion_inicio;
		pcb.sp=0;
		pcb.cantPagCod=2;

		pcb.ultimaPosUsada=pos;

		pcb.indiceCodigoSize=metadata->instrucciones_size;
		pcb.indiceCodigo=metadata->instrucciones_serializado;

		pcb.indiceStack=list_create();

		pcb.indiceEtiquetasSize=metadata->etiquetas_size;
		pcb.indiceEtiquetas=metadata->etiquetas;

		pcb.exitCode=0;

		return pcb;
}

int test_primitivas() {
	printf("Ejecutando\n");
	t_pcb2 pcb;
	char *programa = strdup(PROGRAMA2);
	t_metadata_program *metadata = metadata_desde_literal(programa);
	int i=0;

	pcb=test_crearPCB(metadata);

	int programCounter = 0;
	while(!terminoElPrograma()){
		char* const linea = conseguirDatosDeLaMemoriaMock(programa,
													  pcb.indiceCodigo[pcb.pc].start,
													  pcb.indiceCodigo[pcb.pc].offset);
		printf("\t Evaluando -> %s", linea);
		analizadorLinea(linea, &funciones, &kernel_functions);
		free(linea);
		pcb.pc++;
	}
	metadata_destruir(metadata);
	printf("================\n");

	return EXIT_SUCCESS;
}

char *const conseguirDatosDeLaMemoriaMock(char *prgrama, t_puntero_instruccion inicioDeLaInstruccion, t_size tamanio) {
	char *aRetornar = calloc(1, 100);
	memcpy(aRetornar, prgrama + inicioDeLaInstruccion, tamanio);
	return aRetornar;
}
*/
