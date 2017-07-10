/*
 * sockets.h
 *
 *  Created on: 4/4/2017
 *      Author: utnso
 */

#ifndef SOCKETS_H_
#define SOCKETS_H_

int servidor(void);
int conectar(char *puerto, char *ip,int id);
void eliminarSiHayCPU(int i);
void eliminarSiHayPROGRAMAs(int i);

#endif /* SOCKETS_H_ */
