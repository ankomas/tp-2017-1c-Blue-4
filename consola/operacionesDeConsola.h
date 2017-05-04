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
#include <stdlib.h>
#include <blue4-lib.h>
#include <pthread.h>

void iniciarProgramaAnsisop(char*);

//void gestionarProgramaAnsisop(char* ,int ,pthread_t);
void menuDeControl();
//pthread_t hiloPrograma;

pthread_attr_t hiloPrincipalDetachable;
pthread_t hiloPrincipal;

#endif /* OPERACIONESDECONSOLA_H_ */
