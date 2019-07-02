#include <"listaSE.h">
/*
	# Nombre del Script: Ejercicio2.c		   #
	# Trabajo Practico n°3                             #
	# Integrantes:                                     #
	# Amato, Luciano            DNI: 40.378.763        #
	# Del Greco, Juan Pablo     DNI: 38.435.945        #
	# Meza, Julian              DNI: 39.463.982        #
	# Pompeo, Nicolas Ruben     DNI: 37.276.705        #
	# Entrega: 01/06/2019                              #
	# Entrega n°1                                      #
	#--------------------------------------------------#
*/
#include "cabecera.h"
//main.c -h/maestro_p /directorioRepo archivoSalida
int main(int argc, char *argv[])
{	
//Validaciones varias
	if (argc == 2 && (strcmp(argv[1], "-h") == 0)){
		ayuda();
		exit (1);
	}

	if (argc != 4){
		printf ("No se ha ingresado la cantidad de parametros necesarios, no se puede ejecutar el Script.\n");
		printf ("Ingrese: \"-h\" como unico parametro para obtener la ayuda.\n");
		exit (1);
	}
	const char *directorioSucursales = argv[2]; //directorio de sucursales
	const char *directorioSalida = argv[2]; //Directorio de salida
	int hilos; //Nivel de paralelismo
	pthread_t *vecHilos = NULL;
	int i, j, cantxHilo, sobrante, ubicacion=0, extra=1, contArchivos=0, tam=0, m=0, pos=0;
	t_struct *listaArchivos=NULL;
	struct stat sb;
	struct dirent *ent;
	DIR *dir = opendir(direccionEntrada);

	if (!(stat(directorioSucursales, &sb) == 0 && S_ISDIR(sb.st_mode))){
		printf("El parametro N°2 que corresponde a la ruta del directorio de sucursales no es una ruta valida, no se puede ejecutar el Script.\n");
		printf ("Ingresar: \"-h\" para obtener la Ayuda del programa.\n");
		exit (1);
	}

	if (!(hilos = atoi(argv[3])) || hilos < 1){
		printf ("El parametro N°3 que corresponde al Nivel de Paralelismo fue mal ingresado, no se puede ejecutar el Script.\n");
		printf ("Ingresar: \"-h\" para obtener la Ayuda del programa.\n");
		exit (1);
	}
//Si tengo todo OK, entonces cargo el maestro de productos	
	FILE *pt;
	tLista puntero;
	crearLista(&puntero);
	char linea[2000];
	char *aux;
//argv[1] es el maestro de productos
	pt=fopen(argv[1],"rt");
	if(!pt)
		exit(0);

//Parseo
while(fgets(linea,sizeof(linea),pt) && !feof(pt))
{
//Leo la linea completa        
	aux=strchr(linea,'\n');
	*aux='\0';
//stock
	aux-=5;
	sscanf(aux,"%5d",&reg.stock_minimo);
	*aux='\0';
	printf("stock: %d\t",reg.stock_minimo);
//Producto
	aux-=41;
	sscanf(aux,"%[^0123456789]s",reg.descripcion);
	printf("Descrip: %s\t",reg.descripcion);
	*aux='\0';
//ID
	aux-=8;
	sscanf(aux,"%08d",&reg.id_producto);
	printf("ID: %d\n",reg.id_producto);
	*aux='\0';
//Cargar a memoria
	if(!listaLlena(&puntero)){
		insertarAlFinal(&puntero, &reg);
	}
    }
    	fclose(pt);
	mostrarLista(&puntero,&reg);
//Hilos
	
//////////////////////////////////////////LEE LA CARPETA Y LISTA LOS ARCHIVOS QUE CONTIENE//////////////////////////////////////

        	dir = opendir(directorioSucursales);

        	if (dir == NULL){
          	  printf("No se puede abrir el directorio");
		  exit (1);
		}
        	else{
			while ((ent = readdir(dir)) != NULL){ //d_type = 8 -> archivos, d_type = 4 -> carpetas
              		  	if ((strcmp(ent->d_name, ".") != 0) && (strcmp(ent->d_name, "..") !=0 ) && ent->d_type == 8){
					listaArchivos = (t_struct *)realloc(listaArchivos,(contArchivos+1)*sizeof(t_struct));
					if (!listaArchivos){
						printf("No se pudo reservar memoria");
						exit (1);
					}
		//Reservo el espacio para 1 solo archivo, si hay un archivo mas, vuelvo a reservar para 2, asi hasta reservar solo la cantidad necesaria para los archivos existentes, de esta forma no habra problemas por desconocer la cantidad de archivos.
	listaArchivos[contArchivos].direccion_archivo = NULL;
	listaArchivos[contArchivos].direccion_archivo = (char *)malloc((strlen(direccionEntrada)+strlen(ent->d_name)+2)*sizeof(char));
		//+1 por el "/" y otro +1 para evitar acceso a memoria no permitido.
				if (!listaArchivos[contArchivos].direccion_archivo){
					printf("No se pudo reservar memoria");
					exit (1);
				}

				strcat(strcat(strcpy(listaArchivos[contArchivos].direccion_archivo, direccionEntrada),"/"), ent->d_name);
				contArchivos++;
			  	}
			}
        	}
        	closedir(dir);
/////////////////////////////////////////////////////////////FIN///////////////////////////////////////////////////////////////
    if (contArchivos == 0){
        printf("El directorio de entrada no posee ningun archivo, por lo tanto no se puede continuar con la ejecucion del Script.\n");
        exit (1);
    }
	if (contArchivos < hilos){
		hilos=contArchivos;
		printf("El nivel de paralelismo ingresado es mayor a la cantidad de archivos, por lo tanto, el nivel se igualara con dicha cantidad");
	}
	vecHilos = (pthread_t *)malloc(hilos*sizeof(pthread_t));

	cantxHilo=contArchivos/hilos;
	sobrante=contArchivos%hilos;
///////////////////////////////////////ASIGNA ARCHIVOS A CADA HILO SEGUN NIVEL DE PARALELISMO//////////////////////////////////


	for(i=0;i<hilos;i++){
		if (!sobrante)
			extra=0;
		else
			sobrante--;
		listaArchivos[ubicacion].limite=(cantxHilo+extra);
		listaArchivos[ubicacion].cantChar=i+1; //Este campo contendra el numero de Thread, antes de procesar el archivo, se guarda ese valor, y al finalizar contendra la cantidad de caracteres totales de este.

		listaArchivos[ubicacion].direccion_salida = NULL;

		listaArchivos[ubicacion].direccion_salida = (char *)malloc ((strlen(direccionSalida)+1)*sizeof(char));
		if (!listaArchivos[ubicacion].direccion_salida){
			printf("No se pudo reservar memoria");
			exit (1);
		}
		strcpy(listaArchivos[ubicacion].direccion_salida, direccionSalida);
		pthread_create(&vecHilos[i], NULL, &procesar_archivo, (void *)&listaArchivos[ubicacion]);

		ubicacion+=(cantxHilo+extra);
	}

	for(i=0;i<hilos;i++){ //Si pongo el Join dentro del For anterior, creara un Thread y hasta que no finalice, no creara mas.
		pthread_join(vecHilos[i], NULL);
	}

/////////////////////////////////////////////////////////FIN//////////////////////////////////////////////////////////////////



	for(i=0; i<hilos; i++){
		tam = listaArchivos[pos].limite;
		pos+=tam;
		printf ("============================================================\n");
		printf ("Hilo Nro: %d\nCantidad de archivos asignados: \n", i+1);
		for(j=0;j<tam;j++){
			printf("-%s\n", extraerCadena(listaArchivos[m].direccion_archivo));
			m++;
		}
	}

	printf ("============================================================\n");
	printf ("Archivo/s que poseen la menor cantidad de caracteres: \n");
	cantidad_Letras(listaArchivos, &contArchivos, compararCmM);

	printf ("============================================================\n");
	printf ("Archivo/s que poseen la mayor cantidad de caracteres: \n");
	cantidad_Letras(listaArchivos, &contArchivos, compararCMm);

	printf ("============================================================\n");
	printf ("Primer archivo finalizado: \n");
	archivo_Finalizado(listaArchivos, &contArchivos, compararTmM);

	printf ("============================================================\n");
	printf ("Ultimo archivo finalizado: \n");
	archivo_Finalizado(listaArchivos, &contArchivos, compararTMm);

	extra=0;
	ubicacion=0;
	sobrante=contArchivos%hilos;

	for(i=0;i<hilos;i++){ //Libero el espacio en memoria de direccion_salida (no en todas las posiciones se reservo)
		if (!sobrante)
			extra=0;
		else
			sobrante--;
		free(listaArchivos[ubicacion].direccion_salida);
		ubicacion+=(cantxHilo+extra);
	}

	free(vecHilos); //Libero el espacio reservado para el vector de hilos

	for(i=0;i<contArchivos;i++){ //Libero el espacio reservado para cada direccion
		free(listaArchivos[i].direccion_archivo);
	}
	free(listaArchivos); //Libero el espacio reservado para la estructura utilizada
    	return 0;
}


//Primitivas
void crearLista(tLista *p)
{
    *p=NULL;
}

int listaLlena(const tLista *p)
{
    void *auxiliar=malloc(sizeof(tNodo));
    free(auxiliar);
    return auxiliar==NULL;
}

int listaVacia(const tLista *p)
{
    return *p==NULL;
}

int insertarAlPrincipio(tLista *p,const t_info *d)
{
    tNodo *nuevo=(tNodo *)malloc(sizeof(tNodo));
    if(nuevo==NULL) return 0;
    nuevo->informacion= *d;
    nuevo->siguiente= *p;
    *p=nuevo;
    return OK;
}

int insertarAlFinal(tLista *p,const t_info *d)
{
    while(*p)
    {
        p=&(*p)->siguiente;
    }
    *p=(tNodo *)malloc(sizeof(tNodo));
    if(*p==NULL) return 0;
    (*p)->informacion= *d;
    (*p)->siguiente=NULL;
    return OK;
}

int insertarEnOrden(tLista *p,const t_info *d,int (*comparar)(const void *a,const void *b))
{
    tNodo *nuevo;
    while(*p &&  comparar(&(*p)->informacion,d)<0)
    {
        p=&(*p)->siguiente;
        if(comparar(&(*p)->informacion,d)==0) return 2;
        nuevo=(tNodo *)malloc(sizeof(tNodo));
        if(*p==NULL) return 0;
        nuevo->informacion= *d;
        nuevo->siguiente= *p;
        *p=nuevo;
    }
    return OK;
}
int comparar(const tNodo *a,const t_info *b)
{
    int resultado;
    int id1= (*a).informacion.id_producto;
    int id2= (*b).id_producto;
    resultado=id1-id2;
    return resultado;
}

void mostrarLista(const tLista *p){
printf("ID\tDescripcion\tStock\n");
while(*p){
	printf("%d %s %d\n",(*p)->informacion.id_producto,(*p)->informacion.descripcion,(*p)->informacion.stock_minimo);
	p=&(*p)->siguiente;
	}
}
//Ayuda
void ayuda(){
	printf("Aqui va la ayuda");
}

void *procesar_archivo(void *args){
/*
Este archivo no esta terminado; le falta que procese la lista contra los archivos de sucursales,
"consolidar" el archivo de sucursales, el log y que genere la orden de compra.
*/
	FILE *arch;
	char cadena[200];
	t_struct *listaArchivos = (t_struct *) args;
	int i, j, numHilo, cantArchivos = listaArchivos[0].limite;
	long int cantVocal, cantConsonante, cantOtro;
	char *direccion_final = NULL, *pos = NULL, *nombre = NULL;
/*
	struct timeval t_ini, t_fin;
	time_t tiempo = time(0);
	struct tm *tiempo_inicial, *tiempo_final;
	char tiempo_ini[128], tiempo_fin[128]; 
*/	
	numHilo=listaArchivos[0].cantChar;

	for(j=0;j<cantArchivos;j++){

		cantVocal=0;
		cantConsonante=0;
		cantOtro=0;

		abrir_archivo (&arch, listaArchivos[j].direccion_archivo, "r");
		
		//empezar a contar tiempo
		gettimeofday(&t_ini, NULL); //Comienza a contar milisegundos y microsegundos
		tiempo_inicial = localtime(&tiempo);//Comienza a contar el tiempo en horas:minutos:segundos
	
		while(fgets(cadena, sizeof(cadena), arch)){ //aca procesa

			for (i=0; i<strlen(cadena); i++){
				if (esVocal (&cadena[i])){
					cantVocal++;
				}
				else if (esConsonante(&cadena[i])){
					cantConsonante++;
				}
				else{
					if (cadena[i] != '\n') //de lo contrario cuenta tambien el fin de linea.
						cantOtro++; 
				}
			} //Fin for

		} //Fin while

		//terminar de contar tiempo
		tiempo_final = localtime(&tiempo);//Termina de contar las horas
		gettimeofday(&t_fin, NULL);//Termina de contar milisegundos y microsegundos
		
		
		strftime(tiempo_ini,128,"%H:%M:%S", tiempo_inicial);
		strftime(tiempo_fin,128,"%H:%M:%S", tiempo_final);

		listaArchivos[j].tiempo = t_fin.tv_usec - t_ini.tv_usec; //tiempo que tardo en procesador determinado archivo
		medidorTiempo++;
		listaArchivos[j].tiempo = medidorTiempo;

		pos = strrchr(listaArchivos[j].direccion_archivo, split);

		direccion_final = (char *)malloc((strlen(listaArchivos[0].direccion_salida)+strlen(pos)+2)*sizeof(char));

		if (!direccion_final){
			printf("No se pudo reservar memoria");
			exit (1);
		}
		strcat(strcpy(direccion_final, listaArchivos[0].direccion_salida), pos);

		generar_Archivo(direccion_final, tiempo_ini, &numHilo, &cantVocal, 
		                &cantConsonante, &cantOtro, tiempo_fin,
		                t_ini.tv_sec, t_ini.tv_usec, t_fin.tv_sec, t_fin.tv_usec);

		free(nombre); //Libero el espacio reservado para el nombre
		free(direccion_final); //Libero el espacio reservado para la direccion final (que incluye el nombre del archivo).

		
		//Uso ese campo, para no agregar mas cosas
		listaArchivos[j].cantChar=cantVocal+cantConsonante+cantOtro;

		fclose(arch);
	}
}
