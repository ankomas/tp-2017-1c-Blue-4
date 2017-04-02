/*
 * main.c
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#include "main.h"

#include <commons/string.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#define KNRM  "\x1B[0m"
#define KBOL  "\x1B[1m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"


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

void test(char * aString){
	printf("%s[Test] Resultado: %s %s \n",KBOL,aString,KNRM);
}

void testi(int aInt){
	printf("%s[Testi] Resultado: %i %s \n",KBOL,aInt,KNRM);
}

void assert(char * aString, char * anotherString){
	if(strcmp(aString,anotherString) == 0){
		printf("%s[Assert] Correcto: Las cadenas son iguales \n%s",KGRN,KNRM);
	} else {
		printf("%s[Assert] Fallo: Las cadenas NO son iguales \n%s",KRED,KNRM);
	}
}

void asserti(int aInt, int anotherInt){
	if(aInt == anotherInt){
		printf("%s[Asserti] Correcto: Los numeros son iguales \n%s",KGRN,KWHT);
	} else {
		printf("%s[Asserti] Fallo: Los numeros NO son iguales \n%s",KRED,KWHT);
	}
}

int main() {
	return 0;
}
