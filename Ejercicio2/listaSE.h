#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define CANT 6
///////////////////////////////////////
typedef struct{
    int id_producto;
    char descripcion_producto[40];
    int stock_minimo;
 
}t_dato;
 
typedef struct s_nodo{
    t_dato dato;
    struct s_nodo *sig;
}t_nodo;
 
typedef struct{
    int id_producto;
    int stock_actual;
}t_datoL;
 
typedef struct s_nodoL{
    t_datoL dato;
    struct s_nodoL *sig;
}t_nodoL;
 
typedef t_nodo* t_lista;
typedef t_nodoL* t_listaL;
typedef int (*t_cmp)(const void *e1,const void *e2);
typedef int (*t_cmpL)(const void *e1,const void *e2);
///////////////////////////////////////
void   crearLista(t_lista*);
int    insertarOrdenado(t_lista*,const t_dato*,t_cmp cmp);
void   mostrarLista(t_lista*);
void   vaciarLista(t_lista*);
int    cmp(const void*,const void*);
int    eliminarDato(t_lista *p, const t_dato *,t_cmp);
int    size(t_lista*);
void   cargarArchivosEnLista(t_lista*, char []);
int    exist(t_lista *,const t_dato *,t_cmp);
double   buscarHora(t_lista *,const t_dato *,t_cmp);
 
void    crearListaL(t_listaL*);
int     insertarOrdenadoL(t_listaL*,const t_datoL*,t_cmpL cmpL);
int     cmpL(const void*,const void*);
void    imprimirListaL(t_listaL *, FILE*);
