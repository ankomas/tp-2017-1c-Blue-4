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

void testBitMap()
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
	bitarray_set_bit(bitMap,8);
	//bitarray_set_bit(bitMap,4);
	//bitarray_set_bit(bitMap,5);
	//bitarray_set_bit(bitMap,6);
	bitarray_set_bit(bitMap,5191);
	//bitarray_clean_bit(bitmap,0);
	//bitarray_clean_bit(bitmap,7);
	//bitarray_clean_bit(bitmap,3);

	printf("posicion 0 : %d \n",bitarray_test_bit(bitMap,0));
	printf("posicion 1 : %d \n",bitarray_test_bit(bitMap,1));
	printf("posicion 2 : %d \n",bitarray_test_bit(bitMap,2));
	printf("posicion 3 : %d \n",bitarray_test_bit(bitMap,3));
	printf("posicion 4 : %d \n",bitarray_test_bit(bitMap,4));
	printf("posicion 5 : %d \n",bitarray_test_bit(bitMap,5));
	printf("posicion 8 : %d \n",bitarray_test_bit(bitMap,8));
	printf("posicion 5191 : %d \n",bitarray_test_bit(bitMap,5191));
}


int main(void) {

	inicializarFS();
	//testBitMap();
	servidor();

	return 0;

}
