/*
 Ejercicio 4 - Trabajo Práctico 3 - Primer entrega

 Integrantes:

# DNI       |APELLIDO      |NOMBRE

 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include <curses.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <netdb.h>

typedef struct consulta
{
    int patente;
    char partido[41];
    char titular[20];
    int cantidad_m;
    float monto;
} tConsulta;

int socket_cliente;
char msj[100];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void signalHandler(int);

void *threadEscucha();

void enterParaContinuar();

void ayuda();

int main(int argc, char *argv[])
{
    tConsulta msg;
    struct sockaddr_in direccion_server;
    struct hostent *hstnm;
    struct in_addr **addr_list;

    int num_puerto = 7500;
    int len;
    pthread_t recvt;
    char peticion[100];
    char tecla;
    int nTecla;
    char menu_option[100];

    if (argc < 2 || argc > 4)
    {
        printf("Debe ingresar nombre del partido, IP del servidor y puerto de escucha si es distinto de 7500.\nVea la ayuda con ./cliente -h\n");
        return 1;
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "-?") == 0)
        {
            //aca va la ayuda...
            ayuda();
            exit(1);
        }
        else
        {
            printf("Debe ingresar nombre de partido, IP del servidor y puerto de escucha si es distinto de 7500.\nVea la ayuda con ./cliente -h\n");
            exit(1);
        }
    }

    if (strlen(argv[1]) > 40)
    {
        printf("El nombre de la partido debe ser de maximo 40 caracteres\n");
        exit(1);
    }

    strcpy(msg.partido, argv[1]);

    int result = inet_pton(AF_INET, argv[2], &(direccion_server.sin_addr));

    if (result == 1)
    {
        direccion_server.sin_addr.s_addr = inet_addr(argv[2]);
    }
    else
    {
        hstnm = gethostbyname(argv[2]);

        if (hstnm != NULL)
        {
            direccion_server.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr *)hstnm->h_addr));
        }
        else
        {
            printf("Debe ingresar una IP o nombre de dominio de servidor valido.\nVea la ayuda con ./cliente -h\n");
            exit(1);
        }
    }

    if (argc == 4)
    {
        num_puerto = atoi(argv[3]);

        if (num_puerto == 0)
        {
            printf("Debe ingresar un numero puerto valido !!!\nVea la ayuda con ./cliente -h\n");
            exit(1);
        }
    }

    socket_cliente = socket(AF_INET, SOCK_STREAM, 0); //Establesco la conexion via socket del cliente
    memset(direccion_server.sin_zero, '\0', sizeof(direccion_server.sin_zero));
    direccion_server.sin_family = AF_INET;
    direccion_server.sin_port = htons(num_puerto);

    //Solicitud de conexion hacia el servidor
    if (connect(socket_cliente, (struct sockaddr *)&direccion_server, sizeof(direccion_server)) < 0)
    {
        perror("Conexion no establecida! Pruebe conectarse a un puerto en uso!\n");
        exit(1);
    }

    ////atrapar señales para cerrar los sockets al terminar////

    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    signal(SIGQUIT, signalHandler);

    //////////////////////

    pthread_mutex_lock(&mutex);                        //semaforo bloqueado para que cada send del main al servidor espere que la respuesta la reciba el siguiente thread
    pthread_create(&recvt, NULL, threadEscucha, NULL); //crea el thread que escucha al servidor
    pthread_mutex_lock(&mutex);
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
            scanf("%s", msg.titular);

            // msg.titular = atof(peticion);
            /// len = atoi(peticion);
            //strcpy(msg.titular, len);


            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu cargar multa--\n");
            printf("PATENTE: %d\n", msg.patente);
            printf("Titular: %s\n", msg.titular);

            printf("\nIngrese un monto: ");
            scanf("%s", peticion);
            msg.monto = atof(peticion);
            msg.cantidad_m=1;

            sprintf(peticion, "a%d,%s,%s,%d,%.2f", msg.patente, msg.partido, msg.titular,msg.cantidad_m, msg.monto);

            len = send(socket_cliente, peticion, 100, 0); //Se escribe la peticion al servidor
            if (len < 0)
            {
                perror("Mensaje no enviado!!\n");
                exit(1);
            }
            memset(peticion, '\0', sizeof(peticion));
            printf("Mensaje enviado, esperando respuesta...\n");
            pthread_mutex_lock(&mutex); //Espera respuesta
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;

        case 'b': //Aca deberia enviarle el patente ,partido, y opcion b.

            //system("clear");
            printf("\033c"); //limpia las pantallas, solo en sistemas unix
            printf("--Menu multas por partido--\n");
            printf("\nIngrese ela patente del conductor: ");
            scanf("%s", peticion);
            msg.patente = atoi(peticion);

            if (msg.patente< 1)
            {
                printf("\n--PATENTE invalido, debe ser mayor a 0.--\nPresione Enter para continuar.");
                enterParaContinuar();
                break;
            }

            sprintf(peticion, "b%d,%s", msg.patente, msg.partido);

            len = send(socket_cliente, peticion, 100, 0); //Se escribe la peticion al servidor
            if (len < 0)
            {
                perror("Mensaje no enviado!!\n");
                exit(1);
            }
            memset(peticion, '\0', sizeof(peticion));

            printf("Mensaje enviado, esperando respuesta...\n");
            pthread_mutex_lock(&mutex); //Espera respuesta
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

            len = send(socket_cliente, peticion, 100, 0); //Se escribe la peticion al servidor
            if (len < 0)
            {
                printf("Mensaje no enviado!!\n");
                perror("Mensaje no enviado!!\n");
                exit(1);
            }

            memset(peticion, '\0', sizeof(peticion));

            printf("Mensaje enviado, esperando respuesta...\n");
            pthread_mutex_lock(&mutex); //Espera respuesta
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

            len = send(socket_cliente, peticion, 100, 0); //Se escribe la peticion al servidor
            if (len < 0)
            {
                printf("Mensaje no enviado!!\n");
                perror("Mensaje no enviado!!\n");
                exit(1);
            }

            memset(peticion, '\0', sizeof(peticion));

            printf("Mensaje enviado, esperando respuesta...\n");
            pthread_mutex_lock(&mutex); //Espera respuesta
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;

        case 'e':
            break;
        default:
            printf("\nOpcion invalida\n");
            printf("Presione Enter para continuar\n");
            enterParaContinuar();
            break;
        }

    } while (menu_option[0] != 'e');

    close(socket_cliente);
    exit(0);
}

//funcion que se dispara al atrapar las señales que cierran el proceso
void signalHandler(int dummy)
{
    close(socket_cliente);
    printf("Socket cerrado.\n");
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

void *threadEscucha()
{
    int len;
    while ((len = recv(socket_cliente, msj, 100, 0)) > 0) //Se recibe la respuesta del servidor a la peticion
    {
        //system("clear");
        printf("\033c"); //limpia las pantallas, solo en sistemas unix
        printf("respuesta: %s\n", msj);
        if (msj[0] == '-')
        {
            break;
        }

        memset(msj, '\0', sizeof(msj));
        pthread_mutex_unlock(&mutex);
    }

    printf("\nEl servidor se cerro.\n");
    close(socket_cliente);
    exit(0);
}

void ayuda()
{
    printf("---------------\ncliente -h\ncliente -help\ncliente -? :\n\t\t muestran esta ayuda\n\n");
    printf("---------------\ncliente 'nombre_del_partido' 'ip_servidor'\ncliente 'nombre_el_partidoa' 'nombre_servidor' :  \n\n");
    printf("\t\tEn esta forma el programa se conecta al servidor con la ip indicada en el segundo parametro, en el puerto 7500, y se identifica al ");
    printf("profesor con el nombre de materia indicada.\n");
    printf("---------------\ncliente 'nombre_del_partido' 'ip_servidor' 'puerto'\ncliente 'nombre_del_partido' 'nombre_servidor' 'puerto' :  \n\n");
    printf("\t\tEn esta forma el programa se conecta al servidor con la ip indicada en el segundo parametro, en el puerto indicado, y se identifica al ");
    printf("profesor con el nombre de materia indicada.\n");
    printf("---------------\n\t\tAl iniciar el programa se muestra un menu en el cual el usuario puede agregar un nuevo registro en la base de datos, ");
    printf("consultar el promedio de un multas de todos los infrancores de su partido, o consultar las licencias suspendiad pde todos los partidos.\n");
    printf("\t\tLas patentes ingresados deben ser mayor a 0, de maximo 9 caracteres, el nombre del partido de minimo 1 caracter y de maximo 40 caracteres, ");
    printf("las multas ingresadas deben ser , en caso de ser necesario el separador decimal es el punto.\n");
    printf("\nEjemplo de ejecucuion: ./cliente sisop 127.0.0.1\n");
}
