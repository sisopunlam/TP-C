#include "lista.h"
/////////////////////////////////////////
void crearLista(t_lista *p){
    *p = NULL;
}
//////////////////////////////////////////////////////////////////////////////////
int insertarOrdenado(t_lista *p,const t_dato *d,t_cmp cmp){
    t_nodo *nue;
    while(*p && (cmp(&((*p)->dato),d)<0)){
        p= &(*p)->sig;
    }
    if(*p && (cmp(&((*p)->dato),d)==0)){
        return 0;
    }
    nue = (t_nodo*) malloc(sizeof(t_nodo));
    if(!nue){
        return 0;
    }
    nue->sig=*p;
    nue->dato= *d;
    *p=nue;
    return 1;
 }
//////////////////////////////////////////////////////////////////////////////////
 int cmp(const void *e1,const void *e2){
    //e1 es la clave de la lista
    //e2 es la clave de la nueva info
    return ((t_dato*)e1)->id_producto-((t_dato*)e2)->id_producto;
}
//////////////////////////////////////////////////////////////////////////////////
void vaciarLista(t_lista *p){
    t_nodo *aux;
    while(*p){
        aux = *p;
        p = &(*p)->sig;
        free(aux);
    }
}
//////////////////////////////////////////////////////////////////////////////////
void mostrarLista(t_lista *p){
    if(*p == NULL){
        return;
    }
    while(*p){
        printf("ID: %d Descripcion: %s - Stock Minimo: %d\n", ((*p)->dato).id_producto, ((*p)->dato).descripcion,((*p)->dato).stock_minimo );
        p = &(*p)->sig;
    }
}
//////////////////////////////////////////////////////////////////////////////////
 
// 1 = ELimino
// 0 = no existe
 
int eliminarDato(t_lista *p, const t_dato *d,t_cmp cmp){
    t_nodo *aux;
    while(*p && (cmp(&((*p)->dato),d)<0)){
        p= &(*p)->sig;
    }
    if(*p && (cmp(&((*p)->dato),d)==0)){
        aux = *p;
        *p = aux->sig;
        free(aux);
        return 1;
    }
    return 0;
}
//////////////////////////////////////////////////////////////////////////////////
int size(t_lista *p){
    int size = 0;
    while(*p){
        size++;
        p = &(*p)->sig;
    }
    return size;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void cargarArchivosEnLista(t_lista* p, char pIn[]){
    t_dato d;
    DIR * dirp;
    struct dirent * entry;
 
    dirp = opendir(pIn);
 
    if(!dirp){
        printf("No se pudo abrir el directorio.\n");
        exit(1);
    }
 
    entry = readdir(dirp);
 
    while(entry != NULL) {
        if(entry->d_type == DT_REG){ // Si es archivo regular
            //strcpy(d.patente, entry->d_patente);
            //d.hora = 0;
            insertarOrdenado(p,&d,cmp);
        }
        entry = readdir(dirp);
    }
    closedir(dirp);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
 
// 0 = NO EXISTE
// 1 = EXISTE
 
int exist(t_lista *p,const t_dato *d,t_cmp cmp){
    t_nodo *nue;
    while(*p && (cmp(&((*p)->dato),d)<0)){
        p= &(*p)->sig;
    }
    if(*p && (cmp(&((*p)->dato),d)==0)){
        return 1;
    }
    return 0;
 }
//////////////////////////////////////////////////////////////////////////////////
 
// devuelve los segundos
double buscarHora(t_lista *p,const t_dato *d,t_cmp cmp){
    t_nodo *nue;
    while(*p && (cmp(&((*p)->dato),d)<0)){
        p= &(*p)->sig;
    }
    if(*p && (cmp(&((*p)->dato),d)==0)){ 
        return (((*p)->dato.hora*60) + ((*p)->dato.min) );
    }
    return 0;
 }
 
//////////////////////////////////////////////////////////////////////////////////
void crearListaL(t_listaL *p){
    *p = NULL;
}
//////////////////////////////////////////////////////////////////////////////////
int insertarOrdenadoL(t_listaL *p,const t_datoL *d,t_cmpL cmpL){
    t_nodoL *nue;
     
    while(*p && (cmpL(&((*p)->dato),d)<0)){
        p= &(*p)->sig;
    }
    if(*p && (cmpL(&((*p)->dato),d)==0)){
        (*p)->dato.precio += d->precio;
        return 1;
    }
    nue = (t_nodoL*) malloc(sizeof(t_nodoL));
    if(!nue){
        return 0;
    }
    nue->sig=*p;
    nue->dato= *d;
    *p=nue;
    return 1;
}
//////////////////////////////////////////////////////////////////////////////////
int cmpL(const void *e1,const void *e2){
    //e1 es la clave de la lista
    //e2 es la clave de lo que estoy buscando
    return ((t_datoL*)e1)->id_producto-((t_datoL*)e2)->id_producto;
}
//////////////////////////////////////////////////////////////////////////////////
void imprimirListaL(t_listaL *p, FILE* fp){
    char msg[60];
    while(*p){
        if((*p)->dato.precio != 0){
            sprintf(msg,"%s %.2f\n", (*p)->dato.id_producto, (*p)->dato.precio);
        }else{
            sprintf(msg,"%s 'Aun en el garage.'\n", (*p)->dato.patente);
        }
        fputs(msg,fp);
        p = &(*p)->sig;
    }
