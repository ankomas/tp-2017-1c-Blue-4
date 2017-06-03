/*
 * primitivas.c
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */
#include <parser/parser.h>

AnSISOP_funciones functions = {
};
AnSISOP_kernel kernel_functions = { };

void floca(){
	char* hola=NULL;
	analizadorLinea(hola,&functions,&kernel_functions);
}
