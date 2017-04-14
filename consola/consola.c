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



int main()
{

	int resultadoDeConexion;
	resultadoDeConexion = crearMenuDeConexion();
	if(resultadoDeConexion!=0)
	{
		return 0;
	}

	crearMenuPrincipal();

	return 0;
}

