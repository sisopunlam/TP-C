/*
 Ejercicio 4 - Trabajo Práctico 3

 Integrantes:

# DNI       |APELLIDO      |NOMBRE
#

# Amato, Luciano            DNI: 40.378.763        #
# Del Greco, Juan Pablo     DNI: 38.435.945        #
# Meza, Julian              DNI: 39.463.982        #
# Pompeo, Nicolas Ruben     DNI: 37.276.705  				#

 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include "lista.h"

struct info_cliente
{
	int id_socket;
	pthread_t id_thread;
};

char *archivo = "multas.txt";
struct info_cliente clients[100];
int conectados = 0;
int escuchar = 1;
tLista lista;
char aux[200];
char *car;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void signalHandler(int);
void *recvmg(void *sock);
void procesarPeticion(const char *peticion, char *respuesta);
void ayuda();

int main(int argc, char *argv[])
{
	aux[199] = '\0';

	socklen_t tam_sus_direcciones;
	struct sockaddr_in name, sus_direcciones;
	pthread_t sendt, recvt;
	int err;
	int socket_cliente;
	int server_socket;
	int PORT = 7500;
	int cant_client;
	int len;
	struct info_cliente cl;
	char ip[INET_ADDRSTRLEN];

	if (argc < 2 || argc > 4)
	{
		printf("Debe ingresar maxima cantidad de clientes, puerto de escucha (por defecto 7500) y archivo de notas (por defecto 'multas.txt').\nVea la ayuda con ./servidor -h\n");
		return 1;
	}

	if (argc == 2)
	{
		if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-?") == 0)
		{
			ayuda();
			exit(1);
		}
	}

	cant_client = atoi(argv[1]);

	if (cant_client < 1 || cant_client > 100)
	{
		printf("Primer parametro debe ser la cantidad maxima de clientes que debe ser un numero entero entre 1 y 100.\nVea la ayuda con ./servidor -h\n");
		exit(1);
	}

	if (argc > 2)
	{
		PORT = atoi(argv[2]); //El puerto a utilizar se ingresa por parametro
	}

	if (argc == 4)
	{
		archivo = argv[3];
	}

	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); //Se establece la conexion via Socket
	memset(name.sin_zero, '\0', sizeof(name.sin_zero));
	name.sin_family = AF_INET;
	name.sin_port = htons(PORT);
	name.sin_addr.s_addr = INADDR_ANY;
	tam_sus_direcciones = sizeof(sus_direcciones);

	if (bind(server_socket, (struct sockaddr *)&name, sizeof(name)) != 0)
	{
		perror("No se pudo enlazar al puerto \n");
		exit(1);
	}
	if (listen(server_socket, cant_client) != 0)
	{
		perror("Error al ejecutar listen \n");
		exit(1);
	}

	crearLista(&lista);

	if (cargarListaConArchivo(&lista, archivo) != 0)
	{
		printf("Error al cargar el archivo de multas\n");
		return 1;
	}

	////atrapar señales para cerrar los sockets al terminar////

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGQUIT, signalHandler);

	printf("Puerto enlazado\n");
	printf("Esperando al clientes\n");

	while (escuchar)
	{

		if ((socket_cliente = accept(server_socket, (struct sockaddr *)&sus_direcciones, &tam_sus_direcciones)) < 0)
		{
			printf("No se pudo establecer la coneccion con el cliente\n");
		}
		else
		{
			//Se verifica si quedan lugares libres en la sala..
			if (conectados < cant_client)
			{
				pthread_mutex_lock(&mutex);
				inet_ntop(AF_INET, (struct sockaddr *)&sus_direcciones, ip, INET_ADDRSTRLEN);
				printf("Se ha conectado un nuevo usuario\nsocket: %d\n", socket_cliente);
				cl.id_socket = socket_cliente;

				clients[conectados].id_socket = socket_cliente;
				pthread_create(&recvt, NULL, recvmg, &cl);
				clients[conectados].id_thread = recvt;
				conectados++;

				len = send(socket_cliente, "Conectado al servidor\n", 100, 0);
				if (len < 0)
				{
					printf("Mensaje no enviado!!\n");
					perror("Mensaje no enviado!!\n");
					exit(1);
				}

				pthread_mutex_unlock(&mutex);
			}
			else
			{
				char msg3[100] = "Rechazado, demasiados clientes conectados actualmente\n";

				if (send(socket_cliente, msg3, 100, 0) < 0)
				{
					printf("Se produjo un error al enviar mensaje\n");
					perror("Se produjo un error al enviar mensaje\n");
					continue;
				}

				close(socket_cliente);

				printf("Intento de conexion fallida: se ha alcanzado el maximo de conexiones posibles\n");
				continue;
			}
		}
	}

	return 0;
}

void signalHandler(int dummy)
{ //funcion que se dispara al atrapar las señales que cierran el proceso

	escuchar = 0;
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < conectados; i++)
	{
		send(clients[i].id_socket, "--Servidor desconectado--", 100, 0);
		close(clients[i].id_socket);
	}
	pthread_mutex_unlock(&mutex);
	vaciarLista(&lista);
	printf("Servidor cerrado.\n");
	exit(0);
}

void *recvmg(void *sock)
{
	struct info_cliente cl = *((struct info_cliente *)sock);
	char peticion[100];
	char respuesta[100] = "Respuesta cliente\n";
	int i;
	int len;
	int j;

	while ((len = recv(cl.id_socket, peticion, 100, 0)) > 0) //Recibo la peticion
	{
		peticion[len] = '\0';
		/// Aca tiene que ir el proceso de la peticion..

		pthread_mutex_lock(&mutex);
		procesarPeticion(peticion, respuesta);
		pthread_mutex_unlock(&mutex);

		//muestro la peticion para probar la comunicacion
		printf("peticion de %d: %s\n", cl.id_socket, peticion);
		sleep(2);
		len = send(cl.id_socket, respuesta, 100, 0); //Aca iria la respuesta de la peticion, para probar la comunicacion le mando la cadena "Respuesta cliente"

		if (len < 0)
		{
			printf("Mensaje no enviado!!\n");
			perror("Mensaje no enviado!!\n");
			vaciarLista(&lista);
			exit(1);
		}
		else
		{
			printf("Mensaje enviado a %d!!\n", cl.id_socket);
		}
		memset(peticion, '\0', sizeof(peticion));
		memset(respuesta, '\0', sizeof(respuesta));
	}
	pthread_mutex_lock(&mutex);
	//Cuando sale del while es porque se desconecto un cliente
	printf("Se desconecto un usuario de la sala.\n");
	for (i = 0; i < conectados; i++)
	{
		if (clients[i].id_socket == cl.id_socket)
		{
			j = i;
			while (j < conectados - 1)
			{
				clients[j].id_socket = clients[j + 1].id_socket;
				clients[j].id_thread = clients[j + 1].id_thread;
				j++;
			}
		}
	}
	conectados--;
	pthread_mutex_unlock(&mutex);
	close(cl.id_socket);
	printf("Socket cerrado: %d\n", cl.id_socket);
	pthread_exit((void *)0);
}

void procesarPeticion(const char *peticion, char *respuesta)
{
	int ini;
	int fin;
	float promedio;
	unsigned long patente;
	char *partido;

	switch (peticion[0])
	{

	case 'a':
		if ((cargarRegistroEnListaYArchivo(&lista, archivo, &peticion[1])) == 1)
		{
			strcpy(respuesta, "Registro no se pudo agregar");
		}
		else
		{
			strcpy(respuesta, "Registro agregado");
		}
		break;
	case 'b':
		//patente////
		ini = 1;
		fin = 1;
		while (peticion[fin] != ',')
		{
			fin++;
		}

		strncpy(aux, &peticion[ini], fin);
		aux[fin - ini] = '\0';

		patente = strtoul(aux, &car, 10);
		//////////////////////////////////////////
		///partido////
		fin++;
		strcpy(aux, &peticion[fin]);
		promedio = multasPartido(&lista, patente, aux);

		if (promedio == -1)
		{
			sprintf(respuesta, "No hay multas registrados de %lu para %s.\n", patente, aux);
		}
		else
		{
			sprintf(respuesta,"Monto total de multas de %s de %lu: %.2f\n", aux, patente, promedio);
		}

		////////////////////////////////////

		break;
	case 'c':
		///patente////

		patente = strtoul(&peticion[1], &car, 10);

		promedio = registrosSuspender(&lista, patente);

		if (promedio == -1)
		{
			sprintf(respuesta, "No hay multas registrados de %lu.\n", patente);
		}
		else
		{
			sprintf(respuesta, "Registro a SUSPENDER\nMonto total de multas de %lu: %.2f\n", patente, promedio);
		}
		break;
	case 'd':
		///patente////
		patente = strtoul(&peticion[1], &car, 10);
		promedio = cancelarDeuda(&lista, patente);

		if (promedio == -1)
		{
			sprintf(respuesta, "No hay multas registrados de %lu.\n", patente);
		}
		else
		{
			sprintf(respuesta, "Funcion de cancelar multas aun en trabajo\n");
			/*sprintf(responder, "Sea han cancelado todas las multas de %lu\n", patente);*/
		}

		////////////////////////////////////

		break;
	default:
		strcpy(respuesta, "opcion invalida");
		break;
	}
}

void ayuda()
{
	printf("---------------\n\t\tEste programa es un servidor que responde a las solicitudes que hacen los usuarios, a travez de otro programa cliente, ");
	printf("para agregar multas, o cunsultar monto de multas y licencias suspendidas que estan en un archivo de texto.\n");
	printf("\n----Formas de usar el programa----\n");
	printf("---------------\nservidor -h\nservidor -help\nservidor -? :\n\t\t muestran esta ayuda\n\n");
	printf("---------------\nservidor 'max_cant_clientes' 'puerto' 'nombre_de_archivo_de_multas'\nservidor 'max_cant_clientes' 'puerto'\nservidor 'max_cant_clientes'\n\n");
	printf("\t\tAl iniciar el programa el servidor establece un maximo de clientes indicado en el primer parametro que debe ser un valor entero entre 1 y 100.\n");
	printf("Por defecto escucha en el puerto 7500, a menos que en el segundo parametro se le indique otra cosa.\n");
	printf("Por defecto el archivo con las multas es 'multas.txt', a menos que se le indique otro archivo con el tercer parametro se le indique otra cosa.\n");
	printf("Si el archivo de texto no existe se crea, y si existe se cargan los datos y agregan al final los nuevos registros.\n");
	printf("\nEjemplo de ejecucuion: ./servidor 10\n");
}
