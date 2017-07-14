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
#include <signal.h>

#include "conexiones.h"
#include "pcb.h"
#include "primitivas.h"
#include "primitivasKernel.h"
#include "capaMemoria.h"
#include "capaKernel.h"
#include "test.h"
#include "main.h"

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

	list_destroy_and_destroy_elements(lista_de_stacks,(void*)stack_destroy);

	free(paquete.data);

}

void initFunciones(){
	AnSISOP_funciones f={
			.AnSISOP_definirVariable		= dummy_definirVariable,
			.AnSISOP_obtenerPosicionVariable= dummy_obtenerPosicionVariable,
			.AnSISOP_finalizar 				= dummy_finalizar,
			.AnSISOP_dereferenciar			= dummy_dereferenciar,
			.AnSISOP_asignar				= dummy_asignar,
			.AnSISOP_irAlLabel				= dummy_irAlLabel,
			.AnSISOP_retornar				= dummy_retornar,
			.AnSISOP_llamarSinRetorno		= dummy_llamarSinRetorno,
			.AnSISOP_llamarConRetorno		= dummy_llamarConRetorno,
			.AnSISOP_obtenerValorCompartida = dummy_obtenerValorCompartida,
			.AnSISOP_asignarValorCompartida = dummy_asignarValorCompartida,
	};
	AnSISOP_kernel j = {
			.AnSISOP_wait					= dummy_wait,
			.AnSISOP_signal					= dummy_signal,

			.AnSISOP_reservar				= dummy_reservar,
			.AnSISOP_liberar				= dummy_liberar,

			.AnSISOP_abrir					= dummy_abrir,
			.AnSISOP_borrar					= dummy_borrar,
			.AnSISOP_cerrar					= dummy_cerrar,
	};
	funciones=f;
	kernel_functions=j;
}

void initGlobales(){
	printf("\n");
	printf("Seteando valores de variables globales\n");

	//Valores ajenos -----------------------------------------------
	tamPag_global=pedirTamGlobal(memoria);
	printf("Tamanio de pagina: %i\n",tamPag_global);

	maxStack_global=recibirTamStack();//pedirMaxStack(kernel);
	printf("Maximo stack: %i\n",maxStack_global);

	//Valores locales ----------------------------------------------
	finPrograma_global='Y';
	printf("Valor inicial de fin programa: %c\n",finPrograma_global);

	exit_global='N';
	printf("Exit: %c\n",exit_global);

	printf("\n");
}

void signalHandler(int signum)
{
    if (signum == SIGUSR1)
    {
        printf("SIGUSR1! se terminara este proceso al finalizar la tarea en ejecucion\n");
        exit_global='Y';
    }
}

int main() {
	/*initFunciones();
	testSerializarStack();
	testPCB();
	return 0;*/
	char *ipKernel, *ipMemoria, *puertoKernel, *puertoMemoria;
	t_config* config;

	signal(SIGUSR1, signalHandler);

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

	//testTraerCodigo(memoria);
	//return 0;
	//testTraerLinea(memoria);
	//return 0;
	initFunciones();
	initGlobales();
	standby(kernel);

	cerrarConexion(kernel);
	cerrarConexion(memoria);
	config_destroy(config);
	return 0;
}

