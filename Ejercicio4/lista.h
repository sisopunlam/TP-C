#ifndef LISTA_H_INCLUDED
#define LISTA_H_INCLUDED

typedef struct
{
    unsigned long patente;
    char partido[41];
    char titular[20];
    int cantidad_m;
    float monto;
} t_registro;

typedef struct sNodo
{
    t_registro registro;
    struct sNodo *sig;
    struct sNodo *ant;
} tNodo;

typedef tNodo *tLista;

void crearLista(tLista *p);
tNodo *listaLlena();
int listaVacia(const tLista *p);
tNodo *ponerEnLista(tLista *p, t_registro *registro);
int mostrarLista(const tLista *p);
void vaciarLista(tLista *p);

int cargarListaConArchivo(tLista *pLista, const char *archivo);
int cargarRegistroEnListaYArchivo(tLista *pLista, const char *archivo, const char *linea);
float multasPartido(const tLista *p, unsigned long patente, const char *Partido);
float registrosSuspender(const tLista *p, const char *partido, char *linea);

int cancelarDeuda(tLista *p, unsigned long patente, const char *Partido);
tNodo *eliminarNodo(tNodo *nodo);
int cargarRegistroEnArchivo(tLista *pLista, const char *archivo);

tNodo *registroExitente(tLista *p, t_registro *registro);
float mostrarRegistros(const tLista *p, const char *partido, char *linea);

#endif // LISTA_H_INCLUDED
