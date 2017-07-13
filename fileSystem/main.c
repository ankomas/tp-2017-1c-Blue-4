
#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include "conexiones.h"
#include "arquitecturaFS.h"
#include "test_fileSystem.h"
#include "operacionesFS.h"


void testLectura(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	char* data;
	data=obtenerDatos(ruta,0,34);
	printf("la data es: %s \n",data);
	free(data);
	free(ruta);
}

void testEscrituraFacil(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,0,20,"JUDIOJUDIOJUDIOJUDIO");
	free(ruta);
}

void testEscrituraCompleta(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,21,20,"JUDIOJUDIOJUDIOJUDIO");
	free(ruta);
}

int main(void) {

	inicializarFS();
	//bitMap_TEST();
	//generarArchivoDelFS_TEST();
	//crearArchivo("/archivo_test.bin");
	//borrar("/archivo_test.bin");
	testLectura();
	testEscrituraCompleta();
	//testEscrituraFacil();
	servidor();

	return 0;

}
