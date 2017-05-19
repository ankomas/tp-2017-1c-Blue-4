/*
 * memoria.h
 *
 *  Created on: 27/4/2017
 *      Author: utnso
 */

#ifndef MEMORIA_H_
#define MEMORIA_H_


pthread_t mutex_tabla, mutex_memoria;

typedef struct {
	uint32_t pid;
	uint32_t pagina;
}tablaPaginas_t;

// La memoria esta conformada por paginas de tamaño fijo

typedef struct { // El header contiene la disponibilidad de los datos '0' libre, '1' ocupado y el tamaño del bloque
	char disp;
	uint32_t tam;
}headerB_t;

typedef struct { // Los bloques tienen un header, y los datos
	headerB_t header;
	void* data;
}bloque_t;

typedef struct { // Cada pagina tiene uno o mas bloques
	bloque_t* bloques;
}pagina_t;




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

