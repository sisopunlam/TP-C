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
tNodo *ponerEnLista(tLista *p, t_registro *registro)
{
    tNodo *nue = listaLlena();
    if (nue == NULL)
        return NULL;

    nue->registro.patente = registro->patente;
    strcpy(nue->registro.partido, registro->partido);
    strcpy(nue->registro.titular, registro->titular);
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
        printf("%lu,%s,%s,%.2f\n", nodo1->registro.patente, nodo1->registro.partido, nodo1->registro.titular, nodo1->registro.monto);
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

float registrosSuspender(const tLista *p, unsigned long patente)
{
    tNodo *nodo1 = *p;
    tNodo *nodo2;
    int cont = 0;
    float montoTot = 0;

    while (nodo1 != NULL)
    {
        if (nodo1->registro.patente == patente)
        {
            montoTot += nodo1->registro.monto;
            cont++;
        }

        nodo2 = nodo1;
        nodo1 = (nodo2->sig);
    }

    if (cont > 3 || montoTot > 20000)
    //if (montoTot > 20000)
        return (montoTot);
    return -1;
}

///////////*  POR TERMINAR  *//////////////////
float cancelarDeuda(const tLista *p, unsigned long patente)
{
  tNodo *nodo1 = *p;
  tNodo *nodo2;
  int cont = 0;
  float montoTot = 0;
  while (nodo1 != NULL)
  {
      if (nodo1->registro.patente == patente)
      {
          montoTot += nodo1->registro.monto;
          cont++;
      }

      nodo2 = nodo1;
      nodo1 = (nodo2->sig);
  }

  if (cont > 3 || montoTot > 20000)
      return (montoTot);
  return -1;
}
