#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "conexiones.h"
#include "arquitecturaFS.h"


int main(void) {
	inicializarFS();
	printf("Bloque 0: %i, Bloque 1: %i\n", bloqueLibre(0), bloqueLibre(1));
	ocuparBitmap(1);
	liberarBitmap(0);
	printf("Bloque 0: %i, Bloque 1: %i\n", bloqueLibre(0), bloqueLibre(1));
	servidor();

	return 0;

}
