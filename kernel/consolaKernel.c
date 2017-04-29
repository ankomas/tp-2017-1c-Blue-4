/*
 * consolaKernel.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "planificador.h"

void *consolaKernel(){
	while(1){
		printf("soy la consola del kernel\n");
		sleep(1000);
	}
	return 0;
}
