#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <pthread.h>
#include <semaphore.h>

//Definiciones Macro
#define CANT_SUC 37
#define CANT_ART 21124

//Estructuras necesarias
typedef struct{
    int id;
    char descripcion[41];
    int stockMin;
    int stockAct;
}t_articulo;

typedef struct{
    char ruta[200];
}t_archivo;

typedef struct{
    int nro_thread;
    int leidos;
    int a_procesar;
    int ind_arch[CANT_SUC];
}t_parametro;

//Variables Globales
FILE *arch_master, *archivo_log, *arch_ped, *archivo_stock_consolidado;
pthread_mutex_t mtx1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mtx2 = PTHREAD_MUTEX_INITIALIZER;

t_articulo articulo[CANT_ART+1];
t_archivo lista_arch_suc[CANT_SUC];

//Cabeceras de funciones
void cargarArchMemoria(t_articulo *);
void texto_a_articulo (const char *, t_articulo *);
void *procesar_archivos (void *);
void generar_ped_y_stock_cons();
void ayuda();
void mensaje_error();

//Detalles de funciones
void cargarArchMemoria(t_articulo *art){
    char txt[100];
    int i=0;
    while(fgets(txt, sizeof(txt), arch_master)){
        if(!txt){
            printf("Error registro %d arch_master\n", i);
            exit(4);
        }
        texto_a_articulo(txt, &art[i]);
        i++;
    }
}

void texto_a_articulo (const char *txt, t_articulo *art){
    char aux[41];
    sscanf(txt, "%08d%[^|\n]", &art->id, aux);
    sscanf(txt+48, "%05d", &art->stockMin);
    strncpy(art->descripcion, aux, sizeof(art->descripcion));
    art->descripcion[41]='\0';
    art->stockAct = 0;
}

void *procesar_archivos (void *parametro){
    t_parametro *param = (t_parametro *) parametro;
    FILE *arch_suc;
    char txt[100];
    int id, stock, i;
    for(param->leidos=0; param->leidos < param->a_procesar; param->leidos++){
        if(!(arch_suc = fopen(lista_arch_suc[param->ind_arch[param->leidos]].ruta,"r"))){
        printf("\nError de apertura archivo %s", lista_arch_suc[param->ind_arch[param->leidos]].ruta);	
	fclose(arch_master);
	fclose(arch_ped);
	fclose(archivo_stock_consolidado);
        exit(5);
        }
        printf("\nEstoy leyendo el archivo %s", lista_arch_suc[param->ind_arch[param->leidos]].ruta);
        while(fgets(txt, sizeof(txt), arch_suc)){
            if(!txt){
                printf("Error registro\n");
                exit(4);
            }
            sscanf(txt, "%08d%05d", &id, &stock);
            printf("\nID %d StockActual %d", id, stock);
            i=0;
            //pthread_mutex_lock(&mtx1);
            while((i<CANT_ART) && articulo[i].id != id){
                i++;
            }
            if(articulo[i].id == id){
                articulo[i].stockAct += stock;
            }
            //pthread_mutex_unlock(&mtx1);
        }
        pthread_mutex_lock(&mtx2);
        fprintf(archivo_log,"Thread nro %d proceso el archivo %s\n", param->nro_thread, lista_arch_suc[param->ind_arch[param->leidos]].ruta);
        pthread_mutex_unlock(&mtx2);
        fclose(arch_suc);
    }
    pthread_exit(0);
}

void generar_ped_y_stock_cons(){
    int x;
    for(x=0;x<CANT_ART;x++){
        if(articulo[x].stockAct < articulo[x].stockMin){
            fprintf(arch_ped,"%08d%05d\n",articulo[x].id, (articulo[x].stockMin*2 - articulo[x].stockAct));
            fprintf(archivo_stock_consolidado,"%08d%-40s%05d",articulo[x].id, articulo[x].descripcion, articulo[x].stockMin*2);
        }
        else{
            fprintf(archivo_stock_consolidado,"%08d%-40s%05d",articulo[x].id, articulo[x].descripcion, articulo[x].stockAct);
        }
    }
}
void ayuda(){
	printf("Trabajo Practico nro3 - Ejercicio 2\n");
	printf(".Descripcion: Programa que consolida el stock entre la casa central y las sucursales, a partir del uso de threads\n");
	printf(".Sintaxis: ./nombre_programa <Cantidad de Threads> <Directorio del stock de cada sucursal> <Ruta al maestro de productos>\n");
	printf(".Ejemplo ejecutando la ayuda: ./ejercicio2 -h");
	printf(".Ejemplo ejecutando el programa: ./ejercicio2 5 ./Sucursales ./articulos.txt");
	printf(".Integrantes del grupo:\n..Amato, Luciano (DNI 40.378.763)\n..Del Greco, Juan Pablo (DNI 38.435.945)\n..Meza, Julian (DNI 39.463.982)\n..Pompeo Nicolas Ruben (DNI 37.276.705)\n");
	printf(".1ra Reentrega\n");
}

//Programa Principal
int main(int argc, char* argv[]){
    /*
    ///Ayuda y validaciones
    if(strcmp(argv[1], "-h")==0){
        ayuda();
        return 1;
    }
    if(argc!=4){
            printf("No ingresaste correctamente los parametros. Ejecuta ./ejercicio2 -h para obtener la ayuda.\n");
            return 1;
    }
    */

//Declaracion de variables
    char ruta_dir[200], ruta_arch[100];
    DIR *directorio;
    struct dirent *entrada;
    int i = 0, cant_arch = 0, cant_hilos = *(int *)argv[1];
    t_parametro param[cant_hilos];
    pthread_t vec_hilos[cant_hilos];

    strcpy(ruta_arch, (char *)argv[3]);
    if(!(arch_master = fopen(ruta_arch,"r"))){
        puts("\nError de apertura archivo maestro");
        exit(1);
    }
    if(!(archivo_log = fopen("./log.txt","w"))){
        puts("\nError al crear el archivo log.txt");
	fclose(arch_master);
        exit(2);
    }
    if(!(arch_ped = fopen("./pedidos.txt","w"))){
        puts("\nError al crear el archivo pedidos.txt");
	fclose(arch_master);
	fclose(arch_ped);
        exit(3);
    }
    if(!(archivo_stock_consolidado = fopen("./stock_consolidado.txt","w"))){
        puts("\nError al crear el archivo stock_consolidado.txt");
	fclose(arch_master);
	fclose(arch_ped);
	fclose(archivo_stock_consolidado);
        exit(4);
    }
    cargarArchMemoria(articulo);
    fclose(arch_master);
    strcpy(ruta_dir, (char *)argv[2]);      //copio la ruta del directorio a una variable
    directorio = opendir("ruta_dir");
    if(!directorio){
        printf("\nError al abrir el directorio de sucursales");
	fclose(arch_master);
	fclose(arch_ped);
	fclose(archivo_stock_consolidado);
        exit(2);
    }
    for(i=0;i<cant_hilos;i++){
        param[i].a_procesar=0;
    }
    while((entrada = readdir(directorio))){ //Armo un vector con las rutas de los archivos de stock por sucursal
        if((strcmp(entrada->d_name, ".")!=0) && (strcmp(entrada->d_name, "..")!=0)) //Para ignorar las carpetas ocultas
        {
            strcpy(lista_arch_suc[i].ruta, ruta_dir);
            strcat(lista_arch_suc[i].ruta, "/");
            strcat(lista_arch_suc[i].ruta, entrada->d_name);

            param[i%cant_hilos].ind_arch[param[i%cant_hilos].a_procesar]=i;
            param[i%cant_hilos].a_procesar++;
            i++;
        }
    }
    for(i=0; i<cant_hilos;i++){
        param[i].nro_thread = i+1;
        pthread_create(&vec_hilos[i], NULL, &procesar_archivos, (void*) &param[i]);
        pthread_join(vec_hilos[i],NULL);
    }
    generar_ped_y_stock_cons();
    closedir(directorio);

    fclose(archivo_log);
    fclose(arch_ped);
    fclose(archivo_stock_consolidado);
    printf("\nFin del programa!!!");
    return 0;
}
