/*
 * main.h
 *
 *  Created on: 2/4/2017
 *      Author: utnso
 */

#ifndef MAIN_H_
#define MAIN_H_

	/**
	* @NAME: charToString
	* @DESC: Pasa un char a un String
	*/
	char* charToString(char element);

	/**
	* @NAME: concat
	* @DESC: concatena n elementos
	*/
	char* concat(int n_args, ...);

	/**
	* @NAME: test
	* @DESC: imprime por pantalla un string
	*/
	void test(char * aString);

	/**
	* @NAME: testi
	* @DESC: imprime por pantalla un int
	*/
	void testi(int aInt);

	/**
	* @NAME: assert
	* @DESC: evalua que dos strings sean iguales
	*/
	void assert(char * aString, char * anotherString);

	/**
	* @NAME: asserti
	* @DESC: evalua que dos ints sean iguales
	*/
	void asserti(int aInt, int anotherInt);



#endif /* MAIN_H_ */
