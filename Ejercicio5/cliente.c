/*
 Ejercicio 5 - Trabajo Práctico 3 - Primer entrega

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

#include <sys/ipc.h>   //biblioteca para los flags IPC
#include <sys/shm.h>   //Para utilizar memoria compartida
#include <fcntl.h>     //Para utilizar los flags 0
#include <semaphore.h> //Para utilizar los semaforos POSIX

    typedef struct consulta
{
    int patente;
    char partido[41];
    char titular[20];
    float monto;
} tConsulta;

sem_t *consultar;
sem_t *mensaje;
sem_t *respuesta;

char *msj;

char peticion[100];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void signalHandler(int);
void signalHandler2(int);

void enviar(const char *cad);
void enterParaContinuar();
void ayuda();

int main(int argc, char *argv[])
{
    tConsulta msg;
    int len;
    char peticion[100];
    char tecla;
    int nTecla;
    char menu_option[100];

    char titularAux[20];

    if (argc != 2)
    {
        printf("Debe ingresar nombre del partido.\nVea la ayuda con ./cliente -h\n");
        return 1;
    }

    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-?") == 0)
    {
        //aca va la ayuda...
        ayuda();
        exit(1);
    }

    if (strlen(argv[1]) > 40)
    {
        printf("El nombre del partido debe ser de maximo 40 caracteres\n");
        exit(1);
    }

    strcpy(msg.partido, argv[1]);

    //Abre semaforos para comunicar procesos
    consultar = sem_open("/consultar", 0);
    mensaje = sem_open("/mensaje", 0);
    respuesta = sem_open("/respuesta", 0);

    if (mensaje == NULL || respuesta == NULL || consultar == NULL)
    {
        printf("Semaforos no estan creados.\nServidor cerrado.\n");

        sem_close(consultar);
        sem_close(mensaje);
        sem_close(respuesta);
        exit(0);
    }

    int shmid = shmget(234, 100 * sizeof(char), 0);

    if (shmid == -1)
    {
        printf("La Memoria compartidad no se puede encontrar.\n");

        sem_close(consultar);
        sem_close(mensaje);
        sem_close(respuesta);
        exit(0);
    }

    msj = (char *)shmat(shmid, NULL, 0);

    ////atrapar señales para cerrar los semaforos y desvincular la memoria compartida al terminar////

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);
    signal(SIGUSR1, signalHandler2);

    //Conectandose al servidor

    sprintf(peticion, "n%d", getpid());
    enviar(peticion);

    do
    {
        //system("clear");
        printf("\033c"); //limpia las pantallas, solo en sistemas unix
        printf("--Menu Principal--\n");
        printf("a. Cargar una multa.\n");
        printf("b. Consultar multas por partido.\n");
        printf("c. Consultar conductores suspendidos.\n");
        printf("d. cancelar multas.\n");
        printf("e. Salir.\n");
        printf("\nIngrese una opcion: ");

        nTecla = 0;
        scanf(" %c", &menu_option[0]);

        do
        {
            scanf("%c", &tecla);
            nTecla++;
        } while (tecla != '\n');

        if (nTecla > 1)
            menu_option[0] = 'i';

        switch (menu_option[0])
        {

        case 'a':

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu cargar multas--\n");
            printf("\nIngrese la patente del conductor: ");
            scanf("%s", peticion);

            if (strlen(peticion) > 9)
            {
                printf("\n--patente invalido, debe ser mayor de maximo 9 caracteres.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }

            msg.patente = atoi(peticion);

            if (msg.patente < 1)
            {
                printf("\n--PATENTE invalido, debe ser mayor a 0.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu cargar multa--\n");
            printf("PATENTE: %d\n", msg.patente);
            printf("Ingrease nombre de titular.\n");
            scanf("%s", titularAux);
            strcpy(msg.titular, titularAux);

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu cargar multa--\n");
            printf("PATENTE: %d\n", msg.patente);
            printf("Titular: %s\n", msg.titular);

            printf("\nIngrese un monto: ");
            scanf("%s", peticion);
            msg.monto = atof(peticion);

            if (msg.monto < 0)
            {
                printf("\n--Opcion invalida el monto debe ser mayor a 0.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }
            sprintf(peticion, "a%d,%s,%s,%.2f", msg.patente, msg.partido, msg.titular, msg.monto);
            enviar(peticion);
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;

        case 'b': ///Aca deberia enviarle el patente ,partido, y opcion b.

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu multas por partido--\n");
            printf("\nIngrese la patente del conductor: ");
            scanf("%s", peticion);
            msg.patente = atoi(peticion);

            if (msg.patente < 1)
            {
                printf("\n--PATENTE invalido, debe ser mayor a 0.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }
            sprintf(peticion, "b%d,%s", msg.patente, msg.partido);

            enviar(peticion);
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;

        case 'c': //Aca deberia enviarle el DNI ,y opcion c.

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu SUSPENCIONES--\n");
            printf("\nIngrese la  patente del conductor: ");
            scanf("%s", peticion);
            msg.patente = atoi(peticion);

            if (msg.patente < 1)
            {
                printf("\n--PATENTE invalido, debe ser mayor a 0.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }

            sprintf(peticion, "c%d", msg.patente);
            enviar(peticion);
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;
        case 'd': //Aca deberia enviarle el DNI ,y opcion d.

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu cancelar multa--\n");
            printf("\nIngrese la  patente del conductor: ");
            scanf("%s", peticion);
            msg.patente = atoi(peticion);

            if (msg.patente < 1)
            {
                printf("\n--PATENTE invalido, debe ser mayor a 0.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }

            sprintf(peticion, "d%d", msg.patente);
            enviar(peticion);
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;

        case 'e':
            signalHandler(1);
            break;
        default:
            printf("\nOpcion invalida\n");
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;
        }

    } while (menu_option[0] != 'e');

    shmdt(&msj);
    sem_close(consultar);
    sem_close(mensaje);
    sem_close(respuesta);
    exit(0);
}

//funcion que se dispara al atrapar las señales que cierran el proceso
void signalHandler(int dummy)
{
    sprintf(peticion, "e%d", getpid());
    enviar(peticion);
    printf("programa terminado.\n");
    shmdt(&msj);
    sem_close(consultar);
    sem_close(mensaje);
    sem_close(respuesta);
    exit(0);
}

//funcion que se dispara al recibir la señal SIGUSR1 del servidor
void signalHandler2(int dummy)
{
    printf("Servidor cerrado.\n");
    printf("programa terminado.\n");
    shmdt(&msj);
    sem_close(consultar);
    sem_close(mensaje);
    sem_close(respuesta);
    exit(0);
}

void enterParaContinuar()
{
    char tecla = 'b';
    while (tecla != '\n')
    {
        scanf("%c", &tecla);
    }
    scanf("%c", &tecla);
    while (tecla != '\n')
    {
        scanf("%c", &tecla);
    }
}

void enviar(const char *cad)
{
    pthread_mutex_lock(&mutex); //semaforo bloqueado para que cada send del main al servidor espere que la respuesta la reciba el siguiente thread

    sem_wait(consultar); //Pide la Memoria compartida para que ningun otro cliente pueda usarla
    strcpy(msj, cad);    //Escribe en Memoria compartida
    sem_post(mensaje); //Avisa al servidor que hay un mensaje
    printf("Mensaje enviado, esperando respuesta...\n");
    sem_wait(respuesta); //Espera respuesta del servidor
    printf("\033c");                //limpia las pantallas, solo en sistemas unix
    printf("Respuesta: %s\n", msj); //Imprime respuesta

    if (msj[0] == '-')
    {
        shmdt(&msj);
        sem_post(consultar); //Libera la memoria para otro cliente
        sem_close(consultar);
        sem_close(mensaje);
        sem_close(respuesta);
        printf("Programa terminado.\n");
        exit(0);
    }

    sem_post(consultar); //Libera la memoria para otro cliente
    pthread_mutex_unlock(&mutex);
}

void ayuda()
{
    printf("---------------\ncliente -h\ncliente -help\ncliente -? :\n\t\t muestran esta ayuda\n\n");
    printf("---------------\ncliente 'nombre_del partido'\n\n");
    printf("\t\tEn esta forma el programa se conecta al servidor y se identifica al profesor con el nombre del partido indicada.\n");

    printf("---------------\n\t\tAl iniciar el programa se muestra un menu en el cual el usuario puede agregar un nuevo registro en la base de datos, ");
    printf("consultar el promedio de un infractor de  su partido, o consultar las multas de todos las multas de todos los partidos.\n");
    printf("\t\tLas patentes ingresados deben ser mayor a 0, de maximo 9 caracteres, el nombre de la materia de minimo 1 caracter y de maximo 40 caracteres, ");
    printf("las multas ingresadas deben ser mayor a 0 en caso de ser necesario el separador decimal es el punto.\n");
    printf("\nEjemplo de ejecucuion: ./cliente ramos\n");
    printf("\nArchivo de prueba en multas.txt contiene las patentes 1111-2222-3333\n");
}
