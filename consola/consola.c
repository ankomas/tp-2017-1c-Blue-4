/*
 * consola.c
 *
 *  Created on: 3/4/2017
 *      Author: utnso
 */


#include "conexiones.h"
#include <blue4-lib.h>
#include <commons/config.h>
#include<stdio.h>
#include"operacionesDeConsola.h"
#include "hilos.h"


int main()
{

	//pthread_t hiloPrincipal;
	menuDeControl();
	//pthread_create(&hiloPrincipal,NULL,(void*)crearMenuPrincipal,NULL);
	//pthread_join(hiloPrincipal,NULL);
	crearMenuPrincipal();
	return 0;
}

