/*
 * tests.c
 *
 *  Created on: 5/6/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <pthread.h>
#include "configuraciones.h"
#include "menuMemoria.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include "estructurasAdministrativas.h"
#define true 1
#define false 0

int testLeerYEscribirMemoria(){
	int test1, test2;
	inicializarPrograma(100,1,"HOLA SOY UN CODE ANSISOP");
	test1 = (leerMemoria(100,0,0,23) == "HOLA SOY UN CODE ASISOP");
	escribirMemoria(100, 0, 0, 21, "ESTE TEST ES LA POSTA");
	test2 = (leerMemoria(100,0,0,21) == "ESTE TEST ES LA POSTA");
	if(test1 && test2){
		printf("Test Leer Y Escribir Memoria: Exitoso!\n");
		return true;
	}
	else{
		printf("Test Leer Y Escribir Memoria: Fallido\n");
		return false;
	}
}
