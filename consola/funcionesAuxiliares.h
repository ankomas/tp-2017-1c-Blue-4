/*
 * funcionesAuxiliares.h
 *
 *  Created on: 30/4/2017
 *      Author: utnso
 */

#ifndef FUNCIONESAUXILIARES_H_
#define FUNCIONESAUXILIARES_H_

#define KNRM  "\x1B[0m"
#define KBOL  "\x1B[1m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#include "string.h"
#include "hilos.h"
#include "unistd.h"

void textoEnColor(char* ,int ,int );

void gestionarProgramaAnsisop(dataHilos_t);

int cerrarSocket(int );

#endif /* FUNCIONESAUXILIARES_H_ */
