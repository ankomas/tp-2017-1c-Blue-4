
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
	data=obtenerDatos(ruta,0,49);
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
	guardarDatos(ruta,0,14,"Hola soy TOM. ");
	guardarDatos(ruta,14,20,"Vengo a decirte Hola");
	guardarDatos(ruta,30,15,"Que sos un gil!");
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

void testDefinitivo(){
	char*ruta=rutaEnPuntoMontaje("/Archivos","/lala.bin");
	int i = crearArchivo("/lala.bin");
	if (i>=0) printf("Archivo Creado\n");
	guardarDatos(ruta,0,15,"Mi buen Archivo");
	guardarDatos(ruta,15,13," Es muy lindo");
	printf("Archivo listo para leer\n");
	printf("Aca Leemos: %s\n", obtenerDatos(ruta,0,28));
	printf("Archivo Leido\n");
	borrarArchivo("/lala.bin");
	free(ruta);
}

int main(void) {

	inicializarFS();
	//bitMap_TEST();
	//testDefinitivo();
	//generarArchivoDelFS_TEST();
	//crearArchivo("/archivo_test.bin");
	//borrar("/archivo_test.bin");
	//testEscrituraFacil();
	//testEscrituraAlfabeto();
	//testEscrituraCompleta();
	//testoo();
	//testLectura();
	servidor();

	return 0;

}
