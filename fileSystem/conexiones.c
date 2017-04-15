/*
 * conexiones.c
 *
 *  Created on: 5/4/2017
 *      Author: utnso
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <blue4-lib.h>
#include <commons/config.h>
#include "conexiones.h"



typedef struct{
	char* ip;
	int puerto_kernel;
}datosConfig_t;


struct data_socket
{
	int socket;
	struct sockaddr_in direccionServidor;
};

struct data_socket crearSocket(int puerto, char *ip)
{
	int miSocket;
	struct data_socket datosDelSocket;
	struct sockaddr_in direccionServidor;
	direccionServidor.sin_family = AF_INET;
	direccionServidor.sin_addr.s_addr = inet_addr(ip);
	direccionServidor.sin_port = htons(puerto);
	miSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (miSocket < 0)
	{
		perror("no se pudo crear el socket ");
		exit(1);
	}
	datosDelSocket.socket = miSocket;
	datosDelSocket.direccionServidor = direccionServidor;
	return datosDelSocket;
}




int enviarMensaje(int socket,char* mensaje,int tamanioMensaje)
{
	//char mensaje[1000];
	//scanf("%s", mensaje);
	//memset(mensaje,'\0',1000);
	//memcpy(mensaje,mensajeAEnviar,tamanioMensajeAEnviar);
	//tamanioMensaje = strlen(mensaje);
	int bytesEnviados = send(socket, mensaje, tamanioMensaje, 0);
	int bytesRestantes = tamanioMensaje - bytesEnviados;
	int enviadosEnEsteCiclo;
	if(bytesEnviados <= 0)
		perror("no se envio el mensaje");
	while(bytesRestantes > 0)
	{
		enviadosEnEsteCiclo = send(socket, mensaje+bytesEnviados, bytesRestantes, 0);
		if(enviadosEnEsteCiclo == -1)
			break;
		bytesEnviados += enviadosEnEsteCiclo;
		bytesRestantes -= enviadosEnEsteCiclo;
	}
	return (bytesEnviados == tamanioMensaje);

}

int conectar(int puerto, char *ip)
{
	struct data_socket datosDelSocket = crearSocket(puerto, ip);
	int socketCliente = datosDelSocket.socket;
	struct sockaddr_in direccionServidor = datosDelSocket.direccionServidor;
	if (connect(socketCliente, (void*) &direccionServidor, sizeof(direccionServidor)) != 0)
	{
		perror("No se pudo conectar");
		return 1;
	}
	//while (1)
	//{
		char* mensaje = "hola soy un proceso Consola";
		enviarMensaje(socketCliente,mensaje,strlen(mensaje));
	//}
	return 0;
}


// esta funcion es provisoria
// opcion 1 : estblecer un id por cada proceso que este definido en un protocolo
// opcion 2 : manejarse por medio de mensajes con strings
// opcion 3 : combinar las dos opciones aneteriores
// duda : el hs se encarga de cortar las comunicaciones de una conexion no deseada o lo hace el propio proceso

int establecerHandshake(int socket,char* mensaje,int tamanioMensaje)
{
	char buf[4];

	enviarMensaje(socket,mensaje,tamanioMensaje);

	memset(buf,'\0',4);

	if(recv(socket,buf,sizeof(uint32_t),0) == -1 ){
		perror("recv");
		exit(1);
	}
	printf("Recibi del cliente : %s , de tamanio : %d \n",buf,strlen(buf));

	return 0;
}



datosConfig_t obtenerEstructurasDelConfig()
{
	datosConfig_t datos;
	datos.ip = obtenerConfiguracionString(rutaAbsolutaDe("configConsola.cfg"),"IP_KERNEL");

	printf("el ip: %s \n",datos.ip);
	datos.puerto_kernel = obtenerConfiguracion(rutaAbsolutaDe("configConsola.cfg"),"PUERTO_KERNEL");
	return datos;
}




int conectarseAlKernel()
{
	int resultado;
	datosConfig_t datosDelConfig = obtenerEstructurasDelConfig();
	char* ip = datosDelConfig.ip;
	printf("el ip usado es: %s \n",ip);
	int puerto_kernel = datosDelConfig.puerto_kernel;
	printf("el puerto usado es: %d \n",puerto_kernel);
	resultado=conectar(puerto_kernel, ip);
	if(resultado==0)
	{
		printf("Conexion Exitosa!!! \n\n");
		return 0;
	}
	return 1;
}

void servidor(int puerto) {

struct sockaddr_in direccionServidor;

	direccionServidor.sin_family = AF_INET;

	direccionServidor.sin_addr.s_addr = INADDR_ANY;

	direccionServidor.sin_port = htons(puerto);

	int servidor = socket(AF_INET, SOCK_STREAM, 0);

	int activado = 1;
	setsockopt(servidor, SOL_SOCKET, SO_REUSEADDR, &activado, sizeof(activado));

	if (bind(servidor, (void*) &direccionServidor, sizeof(direccionServidor)) != 0) {

	perror("Falló el bind");

	}

	printf("Estoy escuchando\n");

	listen(servidor, 100);

	//------------------------------Establecer HandShake

	struct sockaddr_in direccionCliente;

	unsigned int tamanioDireccion;

	int cliente = accept(servidor, (void*) &direccionCliente, &tamanioDireccion);

	printf("Recibí una conexión en %d!!\n", cliente);

	send(cliente, "Hola NetCat!", 13, 0);

	send(cliente, ":)\n", 4, 0);

	//------------------------------

	char* buffer = malloc(1000);

	while (1) {

	int bytesRecibidos = recv(cliente, buffer, 1000, 0);

	if (bytesRecibidos <= 0) {

	perror("El Kernel se desconectó.\n");

	}
	buffer[bytesRecibidos] = '\0';

	printf("Me llegaron %d bytes con %s\n", bytesRecibidos, buffer);
	}

	free(buffer);
}


