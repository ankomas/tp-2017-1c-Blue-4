/*
 * primitivas.c
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "conexiones.h"
#include "primitivas.h"
#include "capaKernel.h"

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

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

t_pos proxPos(t_pos posActual,uint32_t tamPag){
	if(posActual.off==tamPag-4){
		posActual.pag++;
		posActual.off=0;
	}
	else{
		posActual.off+=4;
	}

	return posActual;
}

bool esArg(t_nombre_variable variable){
	int i=variable-'0';
	return i>=0 && i<=9;
}

t_puntero dummy_definirVariable(t_nombre_variable variable){
	t_stack *stack;
	t_list *args,*vars;
	t_pos pos;

	printf("Llamada a "YEL"DEFINIR VARIABLE\n"RESET"Nombre variable: %c\n",variable);

	if(pcb_global.sp==-1){
		args=list_create();
		vars=list_create();
		list_add(pcb_global.indiceStack,(void*)stack_create(args,vars,0,pos));
		pcb_global.sp=0;
		pos=pcb_global.ultimaPosUsada;//va a ser 0 0 0
		pos.pag=pcb_global.cantPagCod;
	}else{
		stack=list_get(pcb_global.indiceStack,pcb_global.sp);
		args=stack->args;
		vars=stack->vars;
		pos=proxPos(pcb_global.ultimaPosUsada,tamPag_global);
	}

	if(pos.pag>maxStack_global+pcb_global.cantPagCod){
		printf("ERROR: stack overflow\n");
		setExitCode(&pcb_global,"stack overflow",10);
		printf("\n");
		return STACK_OVERFLOW;
	}

	pcb_global.ultimaPosUsada=pos;
	printf("Pos asignada a %c: %i\n",variable,posAPuntero(pos,tamPag_global));

	if(esArg(variable))
		list_add(args,(void*)var_create(variable,pos));
	else
		list_add(vars,(void*)var_create(variable,pos));
	printf("\n");
	return posAPuntero(pos,tamPag_global);
}

t_puntero dummy_obtenerPosicionVariable(t_nombre_variable variable) {
	t_var *var=NULL;

	printf("Llamada a " YEL"OBTENER POSICION VARIABLE"RESET" %c\n", variable);

	int _is_the_one(t_var *v){
		bool res=v->id==variable;
		if(res)
			var=v;
		return res;
	}

	int _buscar_variable(t_stack *s){
		return(list_find(s->args,(void*)_is_the_one)||list_find(s->vars,(void*)_is_the_one));
	}

	list_find(pcb_global.indiceStack,(void*)_buscar_variable);

	if(var){
		printf("Posicion de %c: %i\n",var->id,posAPuntero(var->pos,tamPag_global));
		printf("\n");
		return posAPuntero(var->pos,tamPag_global);
	}else{
		setExitCode(&pcb_global,"no existe variable",11);
		printf("\n");
		return NO_EXISTE_VARIABLE;
	}
}

void dummy_finalizar(void){
	printf("Llamada a " YEL "FINALIZAR" RESET "\n");
	printf("\n");
	finPrograma_global='F';
}

t_valor_variable dummy_dereferenciar(t_puntero puntero) {
	t_valor_variable res;
	t_pos pos;

	pos=punteroAPos(puntero,tamPag_global);

	printf("Llamada a "YEL"DEREFERENCIAR"RESET" Pag %i Off %i\n",pos.pag,pos.off);
	pos.size=4;
	res=pedirAMemoria(&pcb_global,pos);

	if(res==-1){
		printf("ERROR: fallo al leer en memoria\n");
		setExitCode(&pcb_global,"fallo al leer en memoria",11);
		printf("\n");
		return NO_EXISTE_VARIABLE;
	}
	printf("Valor obtenido en DEREFERENCIAR: %i\n",res);
	printf("\n");
	return res;
}

void dummy_asignar(t_puntero puntero, t_valor_variable variable) {
	t_pos pos;
	int res;

	pos=punteroAPos(puntero,tamPag_global);

	printf("Llamada a "YEL"ASIGNAR"RESET" en Pag %i Off %i el valor %d\n", pos.pag,pos.off, variable);
	pos.size=4;
	res=asignarAMemoria(pos,variable);

	if(res==-1){
		//printf("ERROR: fallo al escribir en memoria\n");
		setExitCode(&pcb_global,"fallo al escribir en memoria",12);
		printf("\n");
	}
	printf("\n");
}

void dummy_irAlLabel(t_nombre_etiqueta etiqueta){
	memcpy(etiqueta+strlen(etiqueta)-1,"\0",1);
	t_puntero_instruccion instruccion=metadata_buscar_etiqueta(etiqueta, pcb_global.indiceEtiquetas, pcb_global.indiceEtiquetasSize);
	int i;

	printf("Llamada a "YEL"IR AL LABEL"RESET" %s\n",etiqueta);
	printf("sizeof etiqueta %i, strlen etiqueta %i\n",sizeof(etiqueta),strlen(etiqueta));

	printf("Indice etiquetas size: %i\n",pcb_global.indiceEtiquetasSize);
	for(i=0;i<pcb_global.indiceEtiquetasSize;i++){
		printf("%c",pcb_global.indiceEtiquetas[i]);
	}
	printf("\n");

	pcb_global.pc = instruccion;

	if(instruccion==-1){
		setExitCode(&pcb_global,"la etiqueta no existe",13);

	}
	printf("Ahora el PC apunta a %i\n",instruccion);
	printf("\n");
}

t_valor_variable (*AnSISOP_obtenerValorCompartida)(t_nombre_compartida variable);
t_valor_variable (*AnSISOP_asignarValorCompartida)(t_nombre_compartida variable, t_valor_variable valor);
void (*AnSISOP_llamarSinRetorno)(t_nombre_etiqueta etiqueta);
void (*AnSISOP_llamarConRetorno)(t_nombre_etiqueta etiqueta, t_puntero donde_retornar);
void (*AnSISOP_retornar)(t_valor_variable retorno);
