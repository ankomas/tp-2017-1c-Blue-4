/*
 * pcb.c
 *
 *  Created on: 19/6/2017
 *      Author: utnso
 */

#include "pcb.h"

void var_destroy(t_var *self){
	free(self);
}

t_var *var_create(char id, t_pos pos){
	t_var *new=malloc(sizeof(t_var));
	new->id=id;
	new->pos=pos;
	return new;
}

void stack_destroy(t_stack *self){
	free(self);
}

t_stack *stack_create(t_list *args, t_list *vars, uint32_t retPos, t_pos retVar){
	t_stack *new=malloc(sizeof(t_stack));
	new->args=args;
	new->vars=vars;
	new->retPos=retPos;
	new->retVar=retVar;
	return new;
}

char *intructionsAStream(t_intructions* instructions,uint32_t size){
	int i,offset=0;
	char *paquete=NULL;

	paquete=malloc(sizeof(t_intructions)*size);

	for(i=0;i<size;i++){
		memcpy(paquete+offset,&instructions->start,sizeof(t_puntero_instruccion));
		offset+=sizeof(t_puntero_instruccion);
		memcpy(paquete+offset,&instructions->offset,sizeof(t_size));
		offset+=sizeof(t_size);
	}

	return paquete;
}

t_intructions *streamAIntructions(char* paquete,uint32_t size){
	int i,offset=0;
	t_intructions *res;
	res=malloc(sizeof(t_intructions)*size);

	for(i=0;i<size;i++){
		memcpy(&res[i].start,paquete+offset,sizeof(t_puntero_instruccion));
		offset+=sizeof(t_puntero_instruccion);
		memcpy(&res[i].offset,paquete+offset,sizeof(t_size));
		offset+=sizeof(t_size);
	}

	return res;
}

package_t stackAStream(t_list *stackLista){
	package_t res;
	t_stack *stack;
	uint32_t sizePaqTotal=0,sizePaq=0,varstam=0,argstam=0,i,j,offset=0,offset2=0,aux=0;
	t_pos aux2;
	t_var *varaux;
	char *paqueteAux=NULL;
	char *paquete=NULL;

	//printf("STACK A STREAM |||||||||||||||\n");

	//Copio cantidad de entradas del stack
	paquete=malloc(4);
	aux=(uint32_t)stackLista->elements_count;
	memcpy(paquete,&aux,sizeof(uint32_t));
	offset+=sizeof(uint32_t);
	sizePaqTotal+=sizeof(uint32_t);

	for(i=0;i<stackLista->elements_count;i++){
		//Preparo valores
		stack=list_get(stackLista,i);
		varstam=sizeof(t_var)*stack->vars->elements_count;
		argstam=sizeof(t_var)*stack->args->elements_count;
		sizePaq=sizeof(uint32_t)+varstam+sizeof(uint32_t)+argstam+sizeof(uint32_t)+sizeof(t_pos);
		paqueteAux=malloc(sizePaq);

		//Copio la cantidad de args
		aux=(uint32_t)stack->args->elements_count;
		memcpy(paqueteAux+offset2,&aux,sizeof(uint32_t));
		offset2+=sizeof(uint32_t);

		//Copio args
		for(j=0;j<stack->args->elements_count;j++){
			varaux=list_get(stack->args,j);
			//printf("arg pag: %i off: %i size: %i\n",varaux->pos.pag,varaux->pos.off,varaux->pos.size);
			memcpy(paqueteAux+offset2,varaux,sizeof(t_var));
			offset2+=sizeof(t_var);
		}

		//Copio la cantidad de vars
		aux=(uint32_t)stack->vars->elements_count;
		memcpy(paqueteAux+offset2,&aux,sizeof(uint32_t));
		offset2+=sizeof(uint32_t);

		//Copio vars
		for(j=0;j<stack->vars->elements_count;j++){
			varaux=list_get(stack->vars,j);
			memcpy(paqueteAux+offset2,varaux,sizeof(t_var));
			offset2+=sizeof(t_var);
		}

		//Copio retPos
		aux=(uint32_t)stack->retPos;
		memcpy(paqueteAux+offset2,&aux,sizeof(uint32_t));
		offset2+=sizeof(uint32_t);

		//Copio retVar
		aux2=(t_pos)stack->retVar;
		memcpy(paqueteAux+offset2,&aux2,sizeof(t_pos));
		offset2+=sizeof(t_pos);

		//Copio toddo en el paquete final
		sizePaqTotal+=sizePaq;
		paquete=realloc(paquete,sizePaqTotal);
		memcpy(paquete+offset,paqueteAux,sizePaq);
		offset+=sizePaq;
		//printf("sizePaq: %i\nsizePaqTotal: %i\noffset: %i\noffset2: %i\n",sizePaq,sizePaqTotal,offset,offset2);
		offset2=0;

		if(paqueteAux)
			free(paqueteAux);

	}

	res.data=paquete;
	res.data_size=sizePaqTotal;

	return res;
}

t_list *streamAStack(char *paquete,uint32_t size){
	t_list *stack=list_create();
	t_list *args=NULL;
	t_list *vars=NULL;
	t_var *var_aux=NULL/*,*var_aux2=NULL*/;
	uint32_t cant,cantvars,offset=0,retPos;
	t_pos retVar;
	int i,j;

	memcpy(&cant,paquete,sizeof(uint32_t));
	offset+=sizeof(uint32_t);

	for(i=0;i<cant;i++){
		vars=list_create();
		args=list_create();

		//Copio la cantidad de args
		memcpy(&cantvars,paquete+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		//Copio args
		for(j=0;j<cantvars;j++){
			var_aux=malloc(sizeof(t_var));
			memcpy(var_aux,paquete+offset,sizeof(t_var));
			offset+=sizeof(t_var);

			list_add(args,var_aux);

			//var_aux2=list_get(args,0);
			//printf("arg pag: %i off: %i size: %i\n",var_aux2->pos.pag,var_aux2->pos.off,var_aux2->pos.size);
		}

		//Copio la cantidad de vars
		memcpy(&cantvars,paquete+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		//Copio vars
		for(j=0;j<cantvars;j++){
			var_aux=malloc(sizeof(t_var));
			memcpy(var_aux,paquete+offset,sizeof(t_var));
			offset+=sizeof(t_var);

			list_add(vars,var_aux);

			//var_aux2=list_get(vars,0);
			//printf("vars pag: %i off: %i size: %i\n",var_aux2->pos.pag,var_aux2->pos.off,var_aux2->pos.size);
		}

		//Copio retPos
		memcpy(&retPos,paquete+offset,sizeof(uint32_t));
		offset+=sizeof(uint32_t);

		//Copio retVar
		memcpy(&retVar,paquete+offset,sizeof(t_pos));
		offset+=sizeof(t_pos);

		list_add(stack,stack_create(args,vars,retPos,retVar));
	}
	//t_stack *stackaux;
	//t_var *var_aux3;
	//stackaux=list_get(stack,0);
//	var_aux3=list_get(stackaux->vars,0);
//	printf("vars pag: %i off: %i size: %i\n",var_aux3->pos.pag,var_aux3->pos.off,var_aux3->pos.size);
	//printf("vars dir: %p\n",stackaux->vars);
	return stack;
}


package_t serializarPCB(t_pcb2 pcb){
	package_t paquete,etiquetasP;
	char *indiceCodigo=NULL;

	indiceCodigo=intructionsAStream(pcb.indiceCodigo,pcb.indiceCodigoSize);

	paquete=serializar(18,
			4,&pcb.pid,
			4,&pcb.pc,
			4,&pcb.sp,
			4,&pcb.cantPagCod,

			4,&pcb.indiceCodigoSize,
			sizeof(t_intructions)*pcb.indiceCodigoSize,pcb.indiceCodigo,

			4,&pcb.indiceEtiquetasSize,
			pcb.indiceEtiquetasSize,pcb.indiceEtiquetas,

			4,&pcb.exitCode);
/*
	indiceCodigoAStream(pcb.indiceCodigoSize,pcb.indiceCodigo);
	4,&pcb.indiceCodigoSize,
	sizeof(t_intructions)*pcb.indiceCodigoSize,pcb.indiceCodigo,

	4,&pcb.indiceStackSize,
	pcb.indiceStackSize,pcb.indiceStack,
*/
	return paquete;
}
