#include "lista.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/resource.h>

void crearLista(tLista *p)
{
    *p = NULL;
}
////////////////////////////////////////////
tNodo *listaLlena()
{
    return (tNodo *)malloc(sizeof(tNodo));
}
////////////////////////////////////////////
int listaVacia(const tLista *p)
{
    return *p == NULL;
}
////////////////////////////////////////////
/*Conprueba si el registro exixte y lo actualiza  */
tNodo *registroExitente(tLista *p, t_registro *registro)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    tNodo *cont = NULL;

    while (nodo1 != NULL && cont == NULL)
    {
        if (nodo1->registro.patente == registro->patente && strcasecmp(nodo1->registro.partido, registro->partido) == 0)
        {
            nodo1->registro.cantidad_m += 1;
            nodo1->registro.monto += registro->monto;

            cont = nodo1;
        }
        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }

    return cont;
}
//////////////
tNodo *ponerEnLista(tLista *p, t_registro *registro)
{
    tNodo *nue = listaLlena();
    nue = listaLlena();
    if (nue == NULL)
        return NULL;

    nue->registro.patente = registro->patente;
    strcpy(nue->registro.partido, registro->partido);
    strcpy(nue->registro.titular, registro->titular);
    nue->registro.cantidad_m = registro->cantidad_m;
    nue->registro.monto = registro->monto;

    nue->sig = NULL;
    if (listaVacia(p))
    {
        nue->ant = nue;
        *p = nue;
    }
    else
    {
        nue->ant = (*p)->ant;
        (*p)->ant = nue;
        nue->ant->sig = nue;
    }
    return nue;
}
////////////////////////////////////////////
void vaciarLista(tLista *p)
{
    if (*p == NULL)
        return;
    tNodo *elim = *p;

    while (*p != NULL)
    {
        *p = elim->sig;
        free(elim);
        elim = *p;
    }
}
////////////////////////////////////////////
int cargarListaConArchivo(tLista *pLista, const char *archivo)
{
    t_registro registro;
    char linea[200];
    char aux[200];
    char *car;
    int ini;
    int fin;

    FILE *fp;
    fp = fopen(archivo, "ab+"); // read mode

    if (fp == NULL)
    {
        perror("Error while opening the file.\n");
        return 1;
    }

    int nro_opcion = 1;
    while (fgets(linea, 200, fp))
    {

        ///patente////
        ini = 0;
        fin = 0;
        while (linea[fin] != ',')
        {
            fin++;
        }
        strncpy(aux, &linea[ini], fin);
        aux[fin - ini] = '\0';
        registro.patente = strtoul(aux, &car, 10);

        //////////////////////////////////////////
        ///partido////
        fin++;
        ini = fin;
        while (linea[fin] != ',')
        {
            fin++;
        }
        strncpy(registro.partido, &linea[ini], fin);
        registro.partido[fin - ini] = '\0';

        ////////////////////////////////////
        ///titular////
        fin++;
        ini = fin;
        while (linea[fin] != ',')
        {
            fin++;
        }
        strncpy(registro.titular, &linea[ini], fin);
        registro.titular[fin - ini] = '\0';

        ////////////////////////////////////
        ///cantidad de multas////
        fin++;
        ini = fin;
        while (linea[fin] != ',')
        {
            fin++;
        }
        strncpy(aux, &linea[ini], fin);
        aux[fin - ini] = '\0';
        registro.cantidad_m = atof(aux);

        ////////////////////////////////////
        ///monto////
        fin++;
        ini = fin;
        while (linea[fin] != '\n' && linea[fin] != '\0')
        {
            fin++;
        }
        strncpy(aux, &linea[ini], fin);
        aux[fin - ini] = '\0';
        registro.monto = atof(aux);

        if (ponerEnLista(pLista, &registro) == NULL)
        {
            printf("Error al poner en lista\n");
        }
    }
    fclose(fp);
    return 0;
}

int cargarRegistroEnListaYArchivo(tLista *pLista, const char *archivo, const char *linea)
{
    t_registro registro;
    char aux[200];
    char *car;
    int ini;
    int fin;

    FILE *fp;
    fp = fopen(archivo, "a"); // append mode

    if (fp == NULL)
    {
        perror("Error al abrir el archivo.\n");
        return 1;
    }

    ///patente////
    ini = 0;
    fin = 0;
    while (linea[fin] != ',')
    {
        fin++;
    }

    strncpy(aux, &linea[ini], fin);
    aux[fin - ini] = '\0';

    registro.patente = strtoul(aux, &car, 10);
    //////////////////////////////////////////
    ///partido////
    fin++;
    ini = fin;
    while (linea[fin] != ',')
    {
        fin++;
    }
    strncpy(registro.partido, &linea[ini], fin);
    registro.partido[fin - ini] = '\0';

    ////////////////////////////////////
    ///titular////
    fin++;
    ini = fin;

    while (linea[fin] != ',')
    {
        fin++;
    }

    strncpy(registro.titular, &linea[ini], fin);
    registro.titular[fin - ini] = '\0';

    ////////////////////////////////////
    ///cantidad de multas////
    fin++;
    ini = fin;

    while (linea[fin] != ',')
    {
        fin++;
    }
    strncpy(aux, &linea[ini], fin);
    aux[fin - ini] = '\0';
    registro.cantidad_m = atof(aux);

    ///////////////////////////////////
    ///monto////
    fin++;
    ini = fin;

    while (linea[fin] != '\n' && linea[fin] != '\0')
    {
        fin++;
    }

    strncpy(aux, &linea[ini], fin);
    aux[fin - ini] = '\0';
    registro.monto = atof(aux);

    //existe archivo, salir y cerrar
    if (registroExitente(pLista, &registro) != NULL)
    {
        fclose(fp);
        cargarRegistroEnArchivo(pLista, archivo);
        return 0;
    }
    if (ponerEnLista(pLista, &registro) == NULL)
    {
        printf("Error al poner en lista\n");
    }

    fprintf(fp, "%s\n", linea);
    fclose(fp);
    return 0;
}

int mostrarLista(const tLista *p)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    while (nodo1 != NULL)
    {
        printf("%lu,%s,%s,%d,%.2f\n", nodo1->registro.patente, nodo1->registro.partido, nodo1->registro.titular, nodo1->registro.cantidad_m, nodo1->registro.monto);
        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }
    return 0;
}

float multasPartido(const tLista *p, unsigned long patente, const char *partido)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    int cont = 0;
    float promedio = 0;

    while (nodo1 != NULL)
    {
        if (nodo1->registro.patente == patente && strcasecmp(nodo1->registro.partido, partido) == 0)
        {
            promedio += nodo1->registro.monto;
            cont++;
        }
        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }

    if (cont == 0)
        return -1;

    return (promedio);
}

float registrosSuspender(const tLista *p, const char *partido, char *linea)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    char aux[200];
    char aux2[50];

    float cont = -1;

    while (nodo1 != NULL)
    {
        if ((nodo1->registro.cantidad_m > 3 || nodo1->registro.monto > 20000) && strcasecmp(nodo1->registro.partido, partido) == 0)
        {
            sprintf(aux2, "%ld \n", (nodo1->registro.patente));
            strcat(aux,aux2);
            cont++;
        }

        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }
    sprintf(linea, "%s \n", aux);
    return cont;
}

///////////*  POR TERMINAR  *//////////////////
int cancelarDeuda(tLista *p, unsigned long patente, const char *partido)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    int cont = -1;

    while (nodo1 != NULL)
    {
        if (nodo1->registro.patente == patente && strcasecmp(nodo1->registro.partido, partido) == 0)
        {
            nodo1 = eliminarNodo(nodo1);
            cont++;
        }
        nodo2 = nodo1;
        if (nodo2 != NULL)
            nodo1 = (nodo2->sig);
    }

    return cont;
}

tNodo *eliminarNodo(tNodo *nodo)
{
    tNodo *dead = nodo;
    tNodo *nodosig = (nodo->sig);
    tNodo *nodoant = (nodo->ant);
    if (nodosig != NULL)
    {
        nodosig->ant = dead->ant;
    }

    nodoant->sig = dead->sig;
    if (nodoant == dead)
    {
        nodoant = NULL;
    }

    free(dead);
    return nodoant;
}

int cargarRegistroEnArchivo(tLista *pLista, const char *archivo)
{
    t_registro registro;
    char linea[200];
    tNodo *nodo1 = *pLista;
    tNodo *nodo2;
    t_registro regis;

    FILE *fp;
    fp = fopen(archivo, "w+"); // write mode

    if (fp == NULL)
    {
        perror("Error al abrir el archivo.\n");
        return 1;
    }

    while (nodo1 != NULL)
    {
        regis = nodo1->registro;
        sprintf(linea, "%ld,%s,%s,%d,%.2f", regis.patente, regis.partido, regis.titular, regis.cantidad_m, regis.monto);

        fprintf(fp, "%s\n", linea);

        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }

    fclose(fp);
    return 0;
}

float mostrarRegistros(const tLista *p, const char *partido, char *linea)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    char aux[500] ="";
    char aux2[100] ="";

    float cont = -1;
    while (nodo1 != NULL)
    {
        if (strcasecmp(nodo1->registro.partido, partido) == 0)
        {
            sprintf(aux2,"%lu\t%s\t%d\t%.2f\n", nodo1->registro.patente, nodo1->registro.titular, nodo1->registro.cantidad_m, nodo1->registro.monto);
            strcat(aux, aux2);
            cont++;
        }

        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }
    sprintf(linea, "%s", aux);
    return cont;
}