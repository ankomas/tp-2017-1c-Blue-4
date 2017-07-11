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




int main(void) {

	inicializarFS();
	//testBitMap();
	//generarArchivoDelFS_TEST();
	servidor();

	return 0;

}
