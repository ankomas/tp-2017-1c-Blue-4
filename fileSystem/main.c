#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "conexiones.h"


int main(void) {

	anuncio(concat(2,"Puerto: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUERTO")));
	anuncio(concat(2,"Punto de Montaje: ",obtenerConfiguracionString(rutaAbsolutaDe("config.cfg"),"PUNTO_MONTAJE")));

	servidor();

	return 0;

}
