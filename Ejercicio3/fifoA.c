/*				Ejercicio n°3
	Trabajo practico n°3
	Integrantes:
	Amato,Luciano 			DNI: 40.378.763
	Pompeo. Nicolas Ruben 	DNI: 37.276.705
	Del Greco, Juan Pablo	DNI: 39.097.812
	Meza, Julian			DNI: 39.463.982

	Entrega 1/07/2019
*/




#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#define TAMBUF 1024
#define VEL_MAX 60
int senial = 1;
typedef struct
{
	int d,
		m,
		a;
}t_fecha;

typedef struct{
	int vel;
	char cam[10];
	char pat[7];
	t_fecha fecha;
	int hora;
	int min;
}t_reg;

void datosBuffer(char *, t_reg *, int *, int *, t_fecha *);
void procesar(t_reg *, FILE* ,FILE* );
void ayuda();
void capturarSigint(int);



void datosBuffer(char *buffer, t_reg *reg, int *hora, int *min, t_fecha *fecha){

	char *actual = buffer;
	while(*actual){
		actual++;
	}
	*actual ='\0';
	actual = strchr(buffer, '_');
	strcpy(reg->cam, ++actual);
	
	*--actual = '\0';
	actual = strchr(buffer, ' ');

	sscanf(actual++,"%d",&reg->vel);
	*--actual = '\0';
	strcpy(reg->pat,buffer);
	

	reg->hora = *hora;
	reg->min = *min;
	reg->fecha.d = fecha->d;
	reg->fecha.m = fecha->m;
	reg->fecha.a = fecha->a;

}

void procesar(t_reg *reg, FILE *archivoMultas, FILE *archivoTransito){
	int multa = 0;
	fprintf(archivoTransito,"%s %dkm/h %s\n",reg->pat,reg->vel,reg->cam);
	printf("%s\t%s\t%d\tkm/h\n",reg->pat,reg->cam,reg->vel);
	sleep(1);

	if(reg->vel > VEL_MAX)
	{
		multa = (reg->vel - VEL_MAX) * 1500;
		fprintf(archivoMultas,"%s %02d:%02d %02d/%02d/%02d %d km/h $%d \n",reg->pat,reg->hora,reg->min,reg->fecha.d,reg->fecha.m,reg->fecha.a,reg->vel,multa);
	}
}

void ayuda(){
	printf("\t-------CAMARAS DE CONTROL DE VELOCIDAD-------\n");
	printf("\n");
	printf("#Este script toma como entrada las velocidades tomadas por distintas camaras \n genera un archivo en el que registra todas las velocidades tomadas \n y otro archivo en el que registra todos los autos en infraccion \n con su respectivo valor a pagar\n");
	printf("\n");
	printf("#Primero debe ejecutar el comando <make> para compilar\n");
	printf("#Luego debe abrir la tuberia ejecutando <./fifoA>\n");
	printf("#Para finalizar, abra 1 terminal por camara y ejecute <./fifoB pathCamara>\n");
	
}

void capturarSigint(int num){
	senial = 0;

}
int main(int argc, char *argv[]){
	int fifo;
	char nombre_fifo[] = "./fifo";
	int no_hay_lectura = 0;
	int entrar = 1;
	char archivoTransito[40];
	char archivoMultas[40];
	char dTransito[] = "dTransito/";
	char dMultas[] = "dMultas/";
	char buffer[TAMBUF];
	t_reg reg;
	t_fecha fechaActual;

	time_t t;
	struct tm *tm;
	char fecha[15];
	t = time(NULL);
	tm = localtime(&t);
	strftime(fecha, 100, "%Y%m%d", tm);
	int horaActual = tm->tm_hour;
	int minActual = tm->tm_min;
	fechaActual.d = tm->tm_mday;
	fechaActual.m = tm->tm_mon+1;
	fechaActual.a = tm->tm_year+1900;

	signal(SIGINT,&capturarSigint);
	if(argc > 2){
		printf("Demasiados parametros, para consultar la ayuda --->  ./fifoA -h \n");
		return -1;
	}

	if(argc == 2 && (strcmp(argv[1], "-h") == 0)){
		ayuda();
		return 0;
	} 
		if(argc == 2 && (strcmp(argv[1], "-h") != 0)){
		printf("Parametros incorrectos, para consultar la ayuda --->  ./fifoA -h\n");
		return -1;
	}


	strcpy(archivoMultas, dMultas);
	strcpy(archivoMultas, "Crear_Multas_");
	strcat(archivoMultas, fecha);
	strcat(archivoMultas, ".txt");

	strcpy(archivoTransito, dTransito);
	strcpy(archivoTransito, "Transito_");
	strcat(archivoTransito, fecha);
	strcat(archivoTransito, ".txt");

	FILE *pfTransito = fopen(archivoTransito,"wt");
	FILE *pfMultas = fopen(archivoMultas, "wt");

	if(!pfMultas || !pfTransito){
		printf("Error al abrir el archivo de transito o multas");
		return -1;
	}

		printf("En espera de lectura de camaras..\n");
	printf("Velocidad maxima 60 km/h\n");

	mkfifo(nombre_fifo,0666);
	fifo = open(nombre_fifo, O_RDONLY);
	if(fifo<0){
		printf("No se pudo crear la tuberia");
		return -1;
	}

	while(entrar && senial){
		while(read(fifo,buffer,TAMBUF) && senial){
			
			if(strcmp("",buffer)!=0){
				datosBuffer(buffer,&reg,&horaActual,&minActual,&fechaActual);
				procesar(&reg,pfMultas,pfTransito);
				no_hay_lectura = 0;
			}
			
			no_hay_lectura = 1;
			

		}
		if(no_hay_lectura && senial){
			printf("No hay camaras por leer, en 10 segundos se cerrara la tuberia\n");
			entrar = 10;
			no_hay_lectura = 0;
		}
		if(senial){
		entrar--;
		sleep(1);
		}
	}

	close(fifo);
	unlink(nombre_fifo);
	printf("Tuberia cerrada\n");
	fclose(pfTransito);
	fclose(pfMultas);

	printf("Archivo multas creado exitosamente\n");
	return 0;

}


