/*
 * blue4-lib
 *
 *  Created on: 2/4/2017
 *      Author: alejandrokrumkamp
 */

#include "blue4-lib.h"

#include <commons/config.h>
#include <commons/string.h>
#include <commons/process.h>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>

#define KNRM  "\x1B[0m"
#define KBOL  "\x1B[1m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* SOCKETS */

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int recvall(int s, char *buf, int len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = len; // how many we have left to send
    int n;

    while(total < len) {
        n = recv(s, buf+total, bytesleft, MSG_WAITALL);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

/* FIN SOCKETS */

void assert(char * aString, char * anotherString){
	if(strcmp(aString,anotherString) == 0){
		printf("%s[Assert] Correcto: Las cadenas son iguales \n%s",KGRN,KNRM);
	} else {
		printf("%s[Assert] Fallo: Las cadenas NO son iguales \n%s",KRED,KNRM);
		printf("%s[Assert] Cadena 1: %s \n%s",KRED,aString,KNRM);
		printf("%s[Assert] Cadena 2: %s \n%s",KRED,anotherString,KNRM);
	}
}

void asserti(int aInt, int anotherInt){
	if(aInt == anotherInt){
		printf("%s[Asserti] Correcto: Los numeros son iguales \n%s",KGRN,KWHT);
	} else {
		printf("%s[Asserti] Fallo: Los numeros NO son iguales \n%s",KRED,KWHT);
		printf("%s[Asserti] Numero uno: %i \n%s",KRED,aInt,KWHT);
		printf("%s[Asserti] Numero dos: %i \n%s",KRED,anotherInt,KWHT);
	}
}

char* charToString(char element) {
	char* new = malloc(2);
	*new = element;
	*(new + 1) = '\0';
	return new;
}

char* concat(int n_args, ...){
        register int i;
        char* a;
        va_list ap;

        va_start(ap, n_args);
        char * unaRuta = string_new();
        for(i = 1; i <= n_args; i++) {
        	a = va_arg(ap, char*);
        	string_append(&unaRuta,a);
        }

        va_end(ap);
        return unaRuta;
}

void killme(){
	char* killme = string_new();
	strcpy(killme, "kill -9 ");
	strcat(killme, string_itoa(process_getpid()));
	system(killme);
}

int obtenerConfiguracion(char *rutaArchivo,char *propiedadConfiguracion)
{
	t_config *cfg;
	cfg=config_create(rutaArchivo);
	if(config_has_property(cfg,propiedadConfiguracion))
		return config_get_int_value(cfg,propiedadConfiguracion);
	else
		printf("Error al obtener una propiedad del archivo de configuracion.");
	return -1;
}

char* obtenerConfiguracionString(char *rutaArchivo,char *propiedadConfiguracion)
{
	t_config *cfg;
	cfg=config_create(rutaArchivo);
	if(config_has_property(cfg,propiedadConfiguracion))
		return config_get_string_value(cfg,propiedadConfiguracion);
	else
		printf("Error al obtener una propiedad del archivo de configuracion.");
	return (char*)-1;
}


char** obtenerConfiguracionArray(char *rutaArchivo,char *propiedadConfiguracion)
{
	t_config *cfg;
	cfg=config_create(rutaArchivo);
	if(config_has_property(cfg,propiedadConfiguracion))
		return config_get_array_value(cfg,propiedadConfiguracion);
	else
		printf("Error al obtener una propiedad del archivo de configuracion.");
	return (char**)-1;
}

char * rutaAbsoluta(){
	char cwd[1024];
	if (getcwd(cwd, sizeof(cwd)) != NULL){
		char *p = cwd;
		// strlen(p)-6 es donde estaria la palabra: Debug
		if(string_ends_with(cwd,"/Debug"))
			p[strlen(p)-5] = 0;

		return p;
	}
	else {
		perror("Error al obtener una ruta absoluta");
		return (char*)-1;
	}
}

char * rutaAbsolutaDe(char * archivo){
	char * unaRuta = string_new();
	string_append(&unaRuta,rutaAbsoluta());
	string_append(&unaRuta,archivo);
	return unaRuta;
}

void test(char * aString){
	printf("%s[Test] Resultado: %s %s \n",KBOL,aString,KNRM);
}

void testi(int aInt){
	printf("%s[Testi] Resultado: %i %s \n",KBOL,aInt,KNRM);
}
