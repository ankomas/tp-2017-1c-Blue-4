/*
 * pcb.h
 *
 *  Created on: 19/6/2017
 *      Author: utnso
 */

#ifndef PCB_H_
#define PCB_H_

#include <commons/collections/list.h>
#include <blue4-lib.h>

typedef struct{
	uint32_t pag;
	uint32_t off;
	uint32_t size;
}t_pos;

typedef struct{
	char id;
	t_pos pos;
}t_var;

typedef struct{
	t_list *args;
	t_list *vars;
	uint32_t retPos;
	t_pos retVar;
}t_stack;

typedef struct{
	uint32_t pid;
	uint32_t pc;
	int32_t sp;
	uint32_t cantPagCod;

	t_pos ultimaPosUsada;

	uint32_t indiceCodigoSize;
	t_intructions* indiceCodigo;

	uint32_t indiceEtiquetasSize;
	char* indiceEtiquetas;

	t_list* indiceStack;

	int32_t exitCode;
}t_pcb2;

t_pcb2 pcb_global;

//Listas
void var_destroy(t_var *self);
t_var *var_create(char id, t_pos pos);
void stack_destroy(t_stack *self);
t_stack *stack_create(t_list *args, t_list *vars, uint32_t retPos, t_pos retVar);
void setExitCode(t_pcb2 *pcb,char *msg,uint32_t exitCode);

t_pos setPos(uint32_t pag,uint32_t off, uint32_t size);
package_t stackAStream(t_list *stackLista);
t_list *streamAStack(char *paquete);
package_t serializarPCB(t_pcb2 pcb);
t_pcb2 deserializarPCB(char* paquete);

void liberarPCB(t_pcb2 pcb);

#endif /* PCB_H_ */
