/*
 * memoria.h
 *
 *  Created on: 27/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_

extern char* puerto;
extern int marcos, tamMarco, entradasCache, cachePorProceso, retardo;


void mostrarMenuMemoria();

int servidor();

/*Las siguientes FUnciones definidas estan comentadas porque faltan definir tipos.-

Funciones segun Enunciado TP

solicitarMemoriaAlIniciar();

crearEstructurasAdministrativas();

validarPedidoRecibido();

simularAccesoMemoria();

responderSolicitudDeMemoria();

int funcionHashing(int pid, int numeroPagina);

guardarArchivo();

//MEMORIA CACHE

eliminarPaginas();

//OPERACIONES DE MEMORIA

inicializarPrograma(int idPrograma, int pagRequeridas);

solicitarBytesDePagina(int idPrograma, int numPagina, int *offset, size_t tamanio);

almacenarBytesEnPagina(int idPrograma, int numPagina, int *offset, size_t tamanio, char* buffer);

asignarPaginaAProceso(int idPrograma, int pagRequeridas);

finalizarPrograma(int idPrograma);*/

#endif /* MEMORIA_H_ */

