/*
 * capaKernel.h
 *
 *  Created on: 4/7/2017
 *      Author: utnso
 */

#ifndef CAPAKERNEL_H_
#define CAPAKERNEL_H_

char finPrograma_global;

void recibirPCB(int socket);
void enviarPCB(int socket);

int32_t obtenerVarGlobal(t_nombre_compartida nombre,int32_t *compartida);
int asignarVarGlobal(t_nombre_compartida nombre,t_valor_variable valor);

char semWait(t_nombre_semaforo sem);
char semSignal(t_nombre_semaforo sem);

char alloc(t_valor_variable valor,t_puntero *puntero);

#endif /* CAPAKERNEL_H_ */
