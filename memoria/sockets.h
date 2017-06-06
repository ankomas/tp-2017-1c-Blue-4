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
uint32_t peticionMemoria(uint32_t);

typedef struct
{
	char codOp;
	int socket;
}dataHilo_t;

void operacionesMemoria(dataHilo_t*);


#endif /* SOCKETS_H_ */
