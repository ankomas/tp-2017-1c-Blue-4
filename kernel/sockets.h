/*
 * sockets.h
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

const char CONSOLA_ID;
const char KERNEL_ID;
const char CPU_ID;
const char FS_ID;
const char UMC_ID;

int servidor(void);
int conectar(char *puerto, char *ip,int id);


#endif /* SOCKETS_H_ */
