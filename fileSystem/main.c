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
	printf("bloqueLibre(0) = %i\n", bloqueLibre(0));
	printf("bloqueLibre(1) = %i\n", bloqueLibre(1));
	printf("getBloqueLibre() = %i\n", getBloqueLibre());
	//ocuparBloque(getBloqueLibre());
	//printf("ocuparBloque(getBloqueLibre())");
	printf("getBloqueLibre() = %i\n", getBloqueLibre());
	servidor();

	return 0;

}
