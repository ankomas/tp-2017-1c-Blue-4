/*
 * operacionesMemoria.c
 *
 *  Created on: 5/5/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "blue4-lib.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include <pthread.h>
#include "configuraciones.h"


 tablaPaginas_t* ObtenerTablaDePaginas()
 {

 }
void cargarCodigo(uint32_t marco, uint32_t pagina, void* codigo){
	memcpy(memoria+marco*configDeMemoria.tamMarco,
		   codigo+pagina*configDeMemoria.tamMarco,
			configDeMemoria.tamMarco);
}

void actualizarTablaDePaginas(uint32_t pid, uint32_t paginasRequeridas,void* codigo){
	tablaPaginas_t *tablaDePaginas = ObtenerTablaDePaginas();
	//delegar
	unsigned marco,pagina = 0;
	for(marco = 0; marco < configDeMemoria.marcos ; marco++){
		//delegar
		if(tablaDePaginas[marco].pid == -2){
			tablaDePaginas[marco].pid= pid;
			tablaDePaginas[marco].pagina = pagina;
			cargarCodigo(marco,pagina,codigo);
			pagina++;
		}
		//
		if(pagina == paginasRequeridas){
			actualizarMarcosDisponibles(paginasRequeridas);
			return;
		}
	}
	//

}
int marcosSuficientes(int paginasRequeridas){
	return paginasRequeridas <= configDeMemoria.marcosDisponibles;
}

void inicializarPrograma(uint32_t pid,uint32_t paginasRequeridas, void* codigo){
	if(marcosSuficientes(paginasRequeridas)){
		//pthread_mutex_lock(mutexeameGil);
		actualizarTablaDePaginas(pid,paginasRequeridas,codigo);
		//pthread_mutex_unlock(desmutexeameGil)
	}
	else{
		// mandarle error al nucleo!!
	}
}


