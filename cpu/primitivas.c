/*
 * primitivas.c
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */
#include <parser/metadata_program.h>
#include <parser/parser.h>
#include "pcb.h"
#include "conexiones.h"
/*
t_puntero dummy_definirVariable(t_nombre_variable variable){
	t_pos pos;
	t_stack nuevoStack;
	t_var nuevaVar;
	t_list *nuevo,*nuevoStackLista;

	printf("Definir la variable %c\n", variable);

	if(memoriaPedirStack(&pos)!=0){
		printf("Error al pedir stack\n");
	}

	nuevoStack.args=NULL;
	nuevoStack.vars=NULL;

	nuevaVar.id=variable;
	nuevaVar.pos=pos;

	nuevo=list_create();
	list_add(nuevo,nuevaVar);

	agregarPosStack(if(pcb.indiceStack==NULL){
		pcb.indiceStack=nuevoStackLista;
	} else {

	})

}*/

t_puntero dummy_obtenerPosicionVariable(t_nombre_variable variable) {
	printf("Obtener posicion de %c\n", variable);
	return 0;
}

void dummy_finalizar(void){
	printf("Finalizar\n");
}

t_valor_variable dummy_dereferenciar(t_puntero puntero) {
	printf("Dereferenciar/n");
	return 0;
}

void dummy_asignar(t_puntero puntero, t_valor_variable variable) {
	printf("Asignando en %d el valor %d\n", puntero, variable);
}
