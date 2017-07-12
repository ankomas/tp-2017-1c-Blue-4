/*
 * test_fileSystem.c
 *
 *  Created on: 11/7/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include <commons/bitarray.h>
#include <commons/collections/dictionary.h>
#include <commons/string.h>
#include "conexiones.h"
#include "arquitecturaFS.h"
#include "test_fileSystem.h"

void generarArchivoDelFS_TEST()
{
	char* ruta=rutaEnPuntoMontaje("/Archivos","/archivo_test.bin");
	t_config elConfig;//=malloc(sizeof(t_config));
	t_dictionary* diccionario = dictionary_create();
	dictionary_put(diccionario,"TAMANIO",0);
	dictionary_put(diccionario,"BLOQUES",0);
	elConfig.properties=diccionario;
	elConfig.path=ruta;
	config_save_in_file(&elConfig,ruta);
	config_set_value(&elConfig,"TAMANIO","2");
	config_set_value(&elConfig,"BLOQUES","[1,2,3]");
	config_save(&elConfig);
	int tamanio=config_get_int_value(&elConfig,"TAMANIO");
	char** bloques=config_get_array_value(&elConfig,"BLOQUES");
	printf("el tamanio es: %d \n",tamanio);
	printf("el bloque es: %s \n",bloques[3]);
	free(ruta);
	free(bloques);
	dictionary_destroy(diccionario);

}



void bitMap_TEST()
{
	/*
	char* bit=calloc(1,configFS.bloques);
	t_bitarray* bitmap=bitarray_create_with_mode(bit,configFS.bloques/8,MSB_FIRST);
	printf("el tamaño que hay en el bitmap es: %d",bitmap->size);
	*/
	printf("tamaño del bitMap : %d \n",bitarray_get_max_bit(bitMap));
	bitarray_set_bit(bitMap,0);
	//bitarray_set_bit(bitMap,1);
	bitarray_set_bit(bitMap,2);
	//bitarray_set_bit(bitMap,4);
	//bitarray_set_bit(bitMap,5);
	//bitarray_set_bit(bitMap,6);
	bitarray_set_bit(bitMap,7);
	//bitarray_clean_bit(bitmap,0);
	//bitarray_clean_bit(bitmap,7);
	//bitarray_clean_bit(bitmap,3);

	printf("posicion 0 : %d \n",bitarray_test_bit(bitMap,0));
	printf("posicion 1 : %d \n",bitarray_test_bit(bitMap,1));
	printf("posicion 2 : %d \n",bitarray_test_bit(bitMap,2));
	printf("posicion 3 : %d \n",bitarray_test_bit(bitMap,3));
	printf("posicion 4 : %d \n",bitarray_test_bit(bitMap,4));
	printf("posicion 5 : %d \n",bitarray_test_bit(bitMap,5));
	printf("posicion 6 : %d \n",bitarray_test_bit(bitMap,6));
	printf("posicion 7 : %d \n",bitarray_test_bit(bitMap,7));
}



void obtenerExpresionDeBloques_TEST()
{
	int numero=20;
	char* numero_string=string_itoa(numero);
	char* bloques=obtenerFormatoDeBloques(NULL,numero_string);
	printf("el formato de inicio de los bloque es: %s \n",bloques);
	bloques=obtenerFormatoDeBloques(bloques,NULL);
	printf("el formato final de los bloque es: %s \n",bloques);
	free(bloques);
	bloques=obtenerFormatoDeBloques(NULL,numero_string);
	printf("el formato de inicio de los bloques es: %s \n",bloques);
	bloques=obtenerFormatoDeBloques(bloques,"30");
	printf("el formato intermedio de los bloques es: %s \n",bloques);
	bloques=obtenerFormatoDeBloques(bloques,"5");
	printf("el formato intermedio de los bloques es: %s \n",bloques);
	bloques=obtenerFormatoDeBloques(bloques,NULL);
	printf("el formato final de los bloques es: %s \n",bloques);
	free(bloques);
	free(numero_string);
}
