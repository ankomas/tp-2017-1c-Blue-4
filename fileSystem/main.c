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
	servidor();

	return 0;

}
