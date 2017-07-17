/*
 * operacionesDeConsola.h
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

#ifndef OPERACIONESDECONSOLA_H_
#define OPERACIONESDECONSOLA_H_

#include <stdint.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <blue4-lib.h>
#include <pthread.h>

void iniciarProgramaAnsisop(char*);

//void gestionarProgramaAnsisop(char* ,int ,pthread_t);
void menuDeControl();
//pthread_t hiloPrograma;

typedef struct
{
	int PID;
	time_t fecha_Y_hora_DeInicio;
	time_t fecha_Y_hora_DeFin;
	int impresionesPorPantalla;
	time_t tiempoTotalDeEjecucion;
}dataProceso_t;


#endif /* OPERACIONESDECONSOLA_H_ */
