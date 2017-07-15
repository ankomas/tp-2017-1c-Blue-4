
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
	data=obtenerDatos(ruta,0,60);
	printf("la data es: %s \n",data);
	free(data);
	free(ruta);
}

void testEscrituraFacil(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,0,20,"MIAUMIAU:3GuauGuau:3");
	free(ruta);
}

void testEscrituraCompleta(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,0,25,"CARACARACARACARACARACAGON");
	guardarDatos(ruta,25,20,"POPOPOPOPOPOPOPOPOPO");
	guardarDatos(ruta,35,20,"PEDOPEDOPEDOPEDOPEDO");
	free(ruta);
}

void testE1(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,14,4,"HOLA");
	free(ruta);
}

void testL1(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	char* data;
	data = obtenerDatos(ruta,14,4);
	printf("La data es: %s \n", data);
	free(data);
	free(ruta);
}

void testEscrituraAlfabeto(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,0,10,"AAAAAAAAAA");
	guardarDatos(ruta,10,10,"BBBBBBBBBB");
	guardarDatos(ruta,20,10,"CCCCCCCCCC");
	guardarDatos(ruta,30,10,"DDDDDDDDDD");
	guardarDatos(ruta,40,10,"EEEEEEEEEE");
	guardarDatos(ruta,50,10,"FFFFFFFFFF");
	free(ruta);
}

void testoo(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	guardarDatos(ruta,2,2,"oo");
	free(ruta);
}

int main(void) {

	inicializarFS();
	//bitMap_TEST();
	//generarArchivoDelFS_TEST();
	//crearArchivo("/archivo_test.bin");
	//borrar("/archivo_test.bin");
	//testEscrituraAlfabeto();
	//testEscrituraFacil();
	//testEscrituraCompleta();
	//testoo();
	//testLectura();
	servidor();

	return 0;

}
