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

/* SERIALIZAODR DESERIALIZADOR */

package_t serializar(int numArgs, ...)
{
	va_list listaArg,listaArg2;
	uint32_t tamTotal,i,puntero,n;
	uint32_t nuevoTam =0;
	package_t paqueteRetorno;
	char *aCopiar;
	char* paquete=NULL;

	tamTotal=((numArgs)/2)*sizeof(uint32_t);

	va_start(listaArg,numArgs);

	va_copy(listaArg2,listaArg);

	for(i=0;i<numArgs;i++)
	{
		if(i%2!=0){
			va_arg(listaArg2,char*);
		}else{
			n=va_arg(listaArg2,uint32_t);
			tamTotal+=n;
		}
	}

	paquete=calloc(1,tamTotal);
	puntero=0;

	for(i=0;i<numArgs;i++)
	{
		if(i%2!=0){
			aCopiar=va_arg(listaArg,void*);
			memcpy(paquete+puntero,aCopiar,nuevoTam);
			puntero+=nuevoTam;
		}else{
			nuevoTam=va_arg(listaArg,uint32_t);
			memcpy(paquete+puntero,&nuevoTam,sizeof(uint32_t));
			puntero+=sizeof(uint32_t);
		}
	}

	va_end(listaArg);

	paqueteRetorno.data_size=tamTotal;
	paqueteRetorno.data=paquete;

	return paqueteRetorno;

}

package_t deserializar(uint32_t *pointer,char *stream){
	uint32_t tam;
	void* contenido;
	package_t res;

	memcpy(&tam,stream+*pointer,sizeof(uint32_t));
	contenido=calloc(1,tam);
	memcpy(contenido,stream+*pointer+sizeof(uint32_t),tam);

	*pointer+=sizeof(uint32_t)+tam;

	res.data=(char*)contenido;
	res.data_size=tam;

	//printf("tam: %i, pointer: %i\n",tam,*pointer);

	return res;
}

/* SOCKETS */

int sendall(uint32_t s, char *buf, uint32_t *len)
{
	uint32_t total = 0;        // how many bytes we've sent
	uint32_t bytesleft = *len; // how many we have left to send
	uint32_t n;
	char *aux;
	char *aux2;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        if (n == 0) {n=-1; break; }
        total += n;
        bytesleft -= n;
    }
    aux=string_itoa(total);
    aux2=concat(2,"Cantidad enviada:",aux);
    //printf("A: %i\n",s);
    //textoVerde(aux2);
    free(aux);
    free(aux2);
    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

int recvall(uint32_t s, char *buf, uint32_t len)
{
	char *aux;
	uint32_t total = 0;        // how many bytes we've sent
	uint32_t bytesleft = len; // how many we have left to send
	uint32_t n;

    while(total < len) {
        n = recv(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        if (n == 0) { break; }
        total += n;
        bytesleft -= n;
    }
    aux=string_itoa(total);
    //textoVerde(concat(2,"Cantidad recibida:",aux));
    free(aux);
    len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
}

/* FIN SOCKETS */

void anuncio(char *aString) {
	printf("%s %s %s \n",KBOL,aString,KNRM);
}

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

char * intToStream(uint32_t unNumero){
	char* dataStream = malloc(4);
	memset(dataStream,0,sizeof(unNumero));
	memcpy(dataStream,&unNumero,sizeof(unNumero));
	return dataStream;
}

char * signedIntToStream(int32_t unNumero){
	char* dataStream = malloc(4);
	memset(dataStream,0,sizeof(unNumero));
	memcpy(dataStream,&unNumero,sizeof(unNumero));
	return dataStream;
}

void killme(){
	char* killme = string_new();
	strcpy(killme, "kill -9 ");
	strcat(killme, string_itoa(process_getpid()));
	system(killme);
}

int obtenerConfiguracion(t_config *cfg,char *propiedadConfiguracion)
{
	if(config_has_property(cfg,propiedadConfiguracion))
		return config_get_int_value(cfg,propiedadConfiguracion);
	else
		printf("Error al obtener una propiedad del archivo de configuracion.");
	return -1;
}

char* obtenerConfiguracionString(t_config *cfg,char *propiedadConfiguracion)
{
	if(config_has_property(cfg,propiedadConfiguracion))
		return config_get_string_value(cfg,propiedadConfiguracion);
	else
		printf("Error al obtener una propiedad del archivo de configuracion.");
	return (char*)-1;
}


char** obtenerConfiguracionArray(t_config *cfg,char *propiedadConfiguracion)
{
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
			p[strlen(p)-5] = '\0';
			char * ret = malloc(strlen(p)+1);
			*ret = '\0';
			string_append(&ret,p);
		return ret;
	}
	else {
		perror("Error al obtener una ruta absoluta");
		return (char*)-1;
	}
}

char * rutaAbsolutaDe(char * archivo){
	char * unaRuta = string_new();
	char * laRutaAbsoluta = rutaAbsoluta();
	string_append(&unaRuta,laRutaAbsoluta);
	string_append(&unaRuta,archivo);
	free(laRutaAbsoluta);
	return unaRuta;
}

void test(char * aString){
	pthread_mutex_lock(&mutex_test);
	printf("%s[Test] Resultado: %s %s \n",KBOL,aString,KNRM);
	pthread_mutex_unlock(&mutex_test);
}

void testi(int aInt){
	printf("%s[Testi] Resultado: %i %s \n",KBOL,aInt,KNRM);
}

void loggearConInt(char*aString,int aInt){
	printf("%s[Testi] Resultado: %i %s \n",KBOL,aInt,KNRM);
}

void textoAzul(char *aString) {
	printf("%s %s %s \n",KBLU,aString,KNRM);
}

void textoAmarillo(char *aString) {
	printf("%s %s %s \n",KYEL,aString,KNRM);
}

void textoRojo(char *aString) {
	printf("%s %s %s \n",KRED,aString,KNRM);
}

void textoVerde(char *aString) {
	printf("%s %s %s \n",KGRN,aString,KNRM);
}
