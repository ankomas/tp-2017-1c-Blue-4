/*
 * sockets.h
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

int servidor();
int handshakeHandler(int);
void peticionMemoria(uint32_t);

typedef struct
{
	char codOp;
	int socket;
}dataHilo_t;

//fd_set master;
//int fdmax;

void operacionesMemoria(int);


#endif /* SOCKETS_H_ */
