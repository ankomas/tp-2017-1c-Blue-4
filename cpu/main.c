/*
 * main.c
 *
 *  Created on: 13/4/2017
 *      Author: utnso
 */
#include <stdio.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <commons/collections/list.h>
#include <parser/metadata_program.h>
#include "conexiones.h"
#include "pcb.h"

#define ID_KERNEL 2
#define ID_MEMORIA 5

void testSerializarStack(){
	package_t paquete;
	t_list *stack,*vars,*args,*lista_de_stacks;
	t_stack *stack_aux;
	t_stack *stack_pos_cero;
	t_var *var,*stack_cero_vars;
	t_pos pos;
	uint32_t retVaroffset_stack_cero;

	bool _pertenece(void *elem){
		if(elem){
		free(elem);
		elem=NULL;
		}
		return true;
	}

	stack=list_create();
	vars=list_create();
	args=list_create();

	pos.off=4;
	pos.pag=0;
	pos.size=4;

	list_add(vars,var_create('a',pos));
	list_add(args,var_create('b',pos));
	list_add(vars,var_create('z',pos));
	list_add(stack,stack_create(args,vars,5,pos));

	paquete=stackAStream(stack);

	list_destroy_and_destroy_elements(vars,(void*)var_destroy);
	list_destroy_and_destroy_elements(args,(void*)var_destroy);
	list_destroy_and_destroy_elements(stack,(void*)stack_destroy);

	printf("Paquete size: %i\n",paquete.data_size);

	lista_de_stacks=streamAStack(paquete.data);

	stack_pos_cero=list_get(lista_de_stacks,0);

	vars=stack_pos_cero->vars;
	args=stack_pos_cero->args;

	retVaroffset_stack_cero=stack_pos_cero->retVar.off;
	stack_cero_vars=list_get(vars,1);

	//printf("vars dir: %p\n",stack_pos_cero->vars);

	printf("offset: %i\n",retVaroffset_stack_cero);
	printf("vars id: %c\n",stack_cero_vars->id);

	list_destroy_and_destroy_elements(vars,(void*)var_destroy);
	list_destroy_and_destroy_elements(args,(void*)var_destroy);
	list_destroy_and_destroy_elements(lista_de_stacks,(void*)stack_destroy);

	free(paquete.data);

}

int main() {
	//testSerializarStack();
	testPCB();
	return 0;
	//int socketKernel,socketMemoria;
	char *ipKernel, *ipMemoria, *puertoKernel, *puertoMemoria;
	t_config* config;

	config = config_create("config.cfg");

	ipKernel = config_get_string_value(config, "IP_KERNEL");
	ipMemoria = config_get_string_value(config, "IP_MEMORIA");
	puertoKernel = config_get_string_value(config, "PUERTO_KERNEL");
	puertoMemoria = config_get_string_value(config, "PUERTO_MEMORIA");

	printf(
			"IP KERNEL: %s\nPUERTO KERNEL: %s\nIP MEMORIA: %s\nPUERTO MEMORIA: %s\n",
			ipKernel, puertoKernel, ipMemoria, puertoMemoria);

	printf("||||||||||||||||||||||||||||||\n");

	memoria=conectar(puertoMemoria, ipMemoria,ID_MEMORIA);
	kernel=conectar(puertoKernel, ipKernel,ID_KERNEL);

	standby(kernel);

	cerrarConexion(kernel);
	config_destroy(config);
	return 0;
}

