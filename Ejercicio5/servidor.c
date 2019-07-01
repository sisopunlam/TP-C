/*
 Ejercicio 4 - Trabajo Práctico 3

 Integrantes:

# Amato, Luciano            DNI: 40.378.763        #
# Del Greco, Juan Pablo     DNI: 38.435.945        #
# Meza, Julian              DNI: 39.463.982        #
# Pompeo, Nicolas Ruben     DNI: 37.276.705  				#

 */

#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "lista.h"

#include <sys/ipc.h> //biblioteca para los flags IPC
#include <sys/shm.h> //Para utilizar memoria compartida
#include <sys/stat.h>
#include <fcntl.h>	 //Para utilizar los flags 0
#include <semaphore.h> //Para utilizar los semaforos POSIX

sem_t *consultar;
sem_t *mensaje;
sem_t *respuesta;
int shmid;

char *archivo = "multas.txt";

int clients[100];
int conectados = 0;
int cant_client;

tLista lista;
char solicitud[100];
char *msj;
char *car;

void signalHandler(int);

void procesarPeticion(const char *peticion, char *responder);

void ayuda();

int main(int argc, char *argv[])
{

	if (argc < 2 || argc > 3)
	{
		printf("Debe ingresar maxima cantidad de clientes, y archivo de multas (por defecto 'multas.txt').\nVea la ayuda con ./servidor -h\n");
		return 1;
	}

	//AYUDA
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

	if (argc == 3)
	{
		archivo = argv[3];
	}

	crearLista(&lista);

	if (cargarListaConArchivo(&lista, archivo) != 0)
	{
		printf("Error al cargar el archivo de multas\n");
		vaciarLista(&lista);
		return 1;
	}

	//Abre semaforos para comunicar procesos
	consultar = sem_open("/consultar", O_CREAT | O_EXCL, 0666, 0);
	mensaje = sem_open("/mensaje", O_CREAT | O_EXCL, 0666, 0);
	respuesta = sem_open("/respuesta", O_CREAT | O_EXCL, 0666, 0);
	if (mensaje == NULL || respuesta == NULL || consultar == NULL)
	{
		printf("No se pueden crear semaforos, puede que los este usando otro proceso servidor.\nServidor cerrado.\n");
		vaciarLista(&lista);
		//Cierre de semaforos
		sem_close(consultar);
		sem_close(mensaje);
		sem_close(respuesta);
		exit(0);
	}

	shmid = shmget(234, 100 * sizeof(char), IPC_CREAT | IPC_EXCL | 0666);
	//shmid = shmget(234,100 * sizeof(char), IPC_CREAT | 0666 );

	if (shmid == -1)
	{
		printf("La Memoria compartida no se puede obtener, puede que la este usando otro proceso servidor.\n");
		vaciarLista(&lista);
		//Cierre de semaforos
		sem_close(consultar);
		sem_close(mensaje);
		sem_close(respuesta);
		//Marcar semaforo para destruirlo
		sem_unlink("/consultar");
		sem_unlink("/mensaje");
		sem_unlink("/respuesta");

		//shmctl(shmid,IPC_RMID,NULL);	//Marcar la memoria compartida para borrar
		exit(0);
	}

	msj = (char *)shmat(shmid, NULL, 0);

	////atrapar señales para cerrar los sockets al terminar////

	signal(SIGINT, signalHandler);
	signal(SIGTERM, signalHandler);
	signal(SIGQUIT, signalHandler);

	printf("Memoria y semaforos creados\n");
	printf("Esperando al clientes\n");

	sem_post(consultar);

	while (1)
	{
		sem_wait(mensaje);		//Espera un mensaje
		strcpy(solicitud, msj); //Escribe en Memoria compartida
		procesarPeticion(solicitud, msj);
		sem_post(respuesta); //Avisa al servidor que hay un mensaje
	}

	return 0;
}

void signalHandler(int dummy) //funcion que se dispara al atrapar las señales que cierran el proceso
{
	sem_wait(consultar);

	for (int i = 0; i < conectados; i++) //Informa a los clientes que el servidor se cierra
	{
		kill(clients[i], SIGUSR1);
	}

	shmdt(&msj);				   //Desvincular la memoria compartida de la variable local
	shmctl(shmid, IPC_RMID, NULL); //Marcar la memoria compartida para borrar

	//Cierre de semaforos
	sem_close(consultar);
	sem_close(mensaje);
	sem_close(respuesta);

	//Marcar semaforo para destruirlo
	sem_unlink("/consultar");
	sem_unlink("/mensaje");
	sem_unlink("/respuesta");

	vaciarLista(&lista);

	printf("Servidor cerrado.\n");

	exit(0);
}

void procesarPeticion(const char *peticion, char *responder)
{
	int ini;
	int fin;
	int i;
	int j;
	float promedio;
	unsigned long patente;
	int pid_cliente;
	char *partido;
	char aux[100];

	switch (peticion[0])
	{

	case 'n':
		pid_cliente = atoi(&peticion[1]);
		//Se verifica si quedan lugares libres en la sala..
		if (conectados < cant_client)
		{
			clients[conectados] = pid_cliente;
			printf("Se ha conectado un nuevo usuario\nPid: %d\n", pid_cliente);
			conectados++;
			strcpy(responder, "Conectado al servidor.");
		}
		else
		{
			printf("Cliente rechazado, demasiados clientes conectados actualmente\n");
			strcpy(responder, "--Intento de conexion fallida: se ha alcanzado el maximo de conexiones posibles\n");
		}
		break;
	case 'a':
		if ((cargarRegistroEnListaYArchivo(&lista, archivo, &peticion[1])) == 1)
		{
			strcpy(responder, "Registro no se pudo agregar");
		}
		else
		{
			strcpy(responder, "Registro agregado");
		}
		break;
	case 'b':
		///patente////
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
			sprintf(responder, "No hay multas registrados de %lu para %s.\n", patente, aux);
		}
		else
		{
			sprintf(responder, "Monto total de multas de %s de %lu: %.2f\n", aux, patente, promedio);
		}

		////////////////////////////////////
		break;
	case 'c':
		///patente////
		patente = strtoul(&peticion[1], &car, 10);
		promedio = registrosSuspender(&lista, patente);

		if (promedio == -1)
		{
			sprintf(responder, "El registrados de %lu no se encunetra suspendido.\n", patente);
		}
		else
		{
			sprintf(responder, "Registro a SUSPENDER\nMonto total de multas de %lu: %.2f\n", patente, promedio);
		}
		break;
	case 'd':
		///patente////
		patente = strtoul(&peticion[1], &car, 10);
		promedio = cancelarDeuda(&lista, patente);
		if (promedio == -1)
		{
			sprintf(responder, "No hay multas registrados de %lu.\n", patente);
		}
		else
		{
			sprintf(responder, "Funcion de cancelar multas aun en trabajo\n");
			/*sprintf(responder, "Sea han cancelado todas las multas de %lu\n", patente);*/
		}

		break;
	case 'e':
		///Salio un cliente////
		pid_cliente = atoi(&peticion[1]);
		printf("Se desconecto un usuario de la sala. Pid: %d\n", pid_cliente);
		strcpy(responder, "--Servidor saco al cliente correctamente.\n");
		for (i = 0; i < conectados; i++)
		{
			if (clients[i] == pid_cliente)
			{
				j = i;
				while (j < conectados - 1)
				{
					clients[j] = clients[j + 1];
					clients[j] = clients[j + 1];
					j++;
				}
			}
		}
		conectados--;
		
		break;
	default:
		strcpy(responder, "opcion invalida");
		break;
	}
}

void ayuda()
{
	printf("---------------\n\t\tEste programa es un servidor que responde a las solicitudes que hacen los usuarios, a travez de otro programa cliente, ");
	printf("para agregar multas, o cunsultar monto de multas y licencias suspendidas que estan en un archivo de texto.\n");
	printf("\n----Formas de usar el programa----\n");
	printf("---------------\nservidor -h\nservidor -help\nservidor -? :\n\t\t muestran esta ayuda\n\n");
	printf("---------------\nservidor 'max_cant_clientes' 'nombre_de_archivo_de_multas'\nservidor 'max_cant_clientes'\n\n");
	printf("\t\tAl iniciar el programa el servidor establece un maximo de clientes indicado en el primer parametro que debe ser un valor entero entre 1 y 100.\n");
	printf("Por defecto el archivo con las notas es 'multas.txt', a menos que se le indique otro archivo con el segundo parametro se le indique otra cosa.\n");
	printf("Si el archivo de texto no existe se crea, y si existe se cargan los datos y agregan al final los nuevos registros.\n");
	printf("El archivo de multas debe estar en el directorio del programa para que lo reconozca\n");
	printf("Solo puede correr un proceso servidor en el sistema, para que los semaforos y memoria compartida no se vean modificados incorrectamente.\n");
	printf("\nEjemplo de ejecucuion: ./servidor 10\n");
}
