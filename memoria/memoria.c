#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <pthread.h>
#include "configuraciones.h"
#include "menuMemoria.h"
#include "operacionesMemoria.h"
#include "memoria.h"
#include "estructurasAdministrativas.h"

char* puerto;
int marcos, tamMarco, entradasCache, cachePorProceso, retardo;


void inicializarEstructurasAdministrativas()
{
	pthread_mutex_init(&escribiendoMemoria,NULL);
	pthread_mutex_init(&escribiendoMemoriaCache,NULL);
	pthread_mutex_init(&mutex_procesosActivos,NULL);
	pthread_mutex_init(&mutex_operacion,NULL);
	pthread_mutex_init(&mutex_test,NULL);
	pthread_mutex_init(&mutex_tablaDePaginas,NULL);
	pthread_mutex_init(&mutex_tablaCache,NULL);
	inicializarDataConfig();
	inicializarMemoria();
	inicializarCache();
}

void eliminarEstructurasAdministrativas()
{
	free(memoria);
	free(procesosActivos);
}

void testLecturaMemoria()
{
		inicializarPrograma(100,1);
		printf("la wea dice: %s\n",(char*)leerMemoria(100,0,0,25));
		escribirMemoria(100,0,0,20,"tu vieja en tanga me gusta");
		printf("la wea dice: %s\n",(char*)leerMemoria(100,0,0,20));
		asignarPaginasAUnProceso(100,20);
		escribirMemoria(100,18,0,16,"salva es re mani");
		printf("la wea dice: %s\n",(char*)leerMemoria(100,18,0,16));
}

void testLecturaCache(){
	agregarProcesoACache(1,0);
	agregarProcesoACache(1,1);
	agregarProcesoACache(1,2);
	agregarProcesoACache(2,0);
	agregarProcesoACache(2,1);
	agregarProcesoACache(2,2);
	agregarProcesoACache(2,3);
	agregarProcesoACache(3,0);
	agregarProcesoACache(3,1);
	agregarProcesoACache(4,0);
	mostrarCache();
	escribirCache(getMarcoCache(1,0),0, 12, "hola soy tom");
	escribirCache(getMarcoCache(1,1),0,12, "aloh yos mot");
	agregarProcesoACache(4,1);
	agregarProcesoACache(5,0);
	agregarProcesoACache(5,1);
	agregarProcesoACache(6,0);
	agregarProcesoACache(6,1);
	agregarProcesoACache(7,0);
	agregarProcesoACache(7,1);
	agregarProcesoACache(8,0);
	agregarProcesoACache(9,1);
	agregarProcesoACache(10,0);

}

void testLecturaPosta()
{
	inicializarPrograma(100,1);
	printf("la wea dice: %s\n",(char*)leer(100,0,0,25));
	escribir(100,0,0,20,"tu vieja en tanga me gusta");
	printf("la wea dice: %s\n",(char*)leer(100,0,0,20));
	asignarPaginasAUnProceso(100,20);
	escribir(100,18,0,16,"salva es re mani");
	printf("la wea dice: %s\n",(char*)leer(100,18,0,16));
}


void testFuncionesAProcesosActivos()
{
	agregar_DataDeProcesoActivo(200,5);
	agregar_DataDeProcesoActivo(300,1);
	agregar_DataDeProcesoActivo(400,3);
	eliminar_DataDeProcesoActivo(300);
	aumentar_PaginasActualesDeProcesoActivo(400,5);
	printf("las paginas totales con el aumento de paginas es : %d \n",obtener_paginasActualesDeProcesoActivo(400));
	listar_DataDeTodosLosProcesosActivos();
	printf("termino de listar todos los procesos \n");
	printf("empiezo a listar un proceso especifico \n");
	listar_DataDeProcesoActivo(200);
}

int main(void) {

	inicializarEstructurasAdministrativas();
	//inicializarPrograma(1,3);
	//NPAC(1,2);
	//nuevoMarcoDelMismoProceso(1);
	testLecturaCache();
	//testLecturaPosta();
	//inicializarPrograma(100,8);
	//escribir(100,0,0,4, "Hola");
	pthread_t hiloMostrarMenu;
	pthread_create(&hiloMostrarMenu, NULL, (void *) mostrarMenuMemoria, NULL);
	servidor();
	pthread_join(hiloMostrarMenu, NULL);
	eliminarEstructurasAdministrativas();
	return 0;

}
