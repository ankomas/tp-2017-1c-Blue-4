/*
 * operacionesDeConsola.c
 *
 *  Created on: 14/4/2017
 *      Author: utnso
 */

void iniciarProgramaAnsisop(char* programaAsisop)
{

}


void finalizarPrograma(int id)
{

}


void desconectarConsola()
{

}


void limpiarMensajes()
{
	system("clear");
}




int crearMenuDeConexion()
{
	int opcion,resultado;

	do{
	system("clear");
	printf("BIENVENIDO A LA CONSOLA \n");
	printf("PUEDE ELEGIR DOS OPCIONES \n\n");
	printf("1-PARA CONECTARSE A UN NUCLEO \n");
	printf("2-PARA SALIR \n\n");
	printf("OPCION A ELEGIR:");
	scanf("%d",&opcion);

	switch(opcion)
	{
	case 1:
		printf("Conectando al Kernel..... \n\n");
		resultado=conectarseAlKernel();
		if(resultado!=0)
			return 2;
		system("clear");
		getchar();
		return 0;
	case 2:
		return 1;
	default:
		system("clear");
		getchar();
		break;
	}
	}while(opcion!=2 && opcion!=1);

	return 2;
}







void crearMenuPrincipal()
{
	int opcion,id;
	char* programa;

	do{

		printf("BIENVENIDO AL MENU PRINCIPAL DE LA CONSOLA \n");
		printf("ELIJA UNA OPCION \n\n");
		printf("1-INICIAR PROGRAMA \n");
		printf("2-FINALIZAR PROGRAMA \n");
		printf("3-DESCONECTAR CONSOLA \n");
		printf("4-LIMPIAR MENSAJES");

		printf("OPCION A ELEGIR:");
		scanf("%d",&opcion);

		switch(opcion)
			{
			case 1:
				printf("Ingrese programa ANSISOP ");
				scanf("%s",&programa); //TODO averiguar si es la ruta o un programa por medio de un string!!!
				printf("Iniciando programa ANSISOP..... \n\n");
				iniciarProgramaAnsisop(programa);
				getchar();
				break;
			case 2:
				printf("ingrese el id del programa que quiere finalizar ");
				scanf("%d",&id);
				finalizarPrograma(id);
				getchar();
				break;
			case 3:
				printf("Desconectando Consola..... \n\n");
				desconectarConsola();
				getchar();
				break;
			case 4:
				limpiarMensajes();
				getchar();
				break;
			default:
				printf("Opcion ingresada invalida \n\n");
				getchar();
				break;
			}
	}while(opcion!=3);
}


