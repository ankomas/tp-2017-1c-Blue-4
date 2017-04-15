/*
 * sockets.h
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

const char CONSOLA_ID = '1';
const char KERNEL_ID = '2';
const char CPU_ID = '3';
const char FS_ID = '4';
const char UMC_ID = '5';

int servidor(void);
int conectar(char *puerto, char *ip,int id);


#endif /* SOCKETS_H_ */
