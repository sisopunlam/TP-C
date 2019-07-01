#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define TAMBUF 1024

void mostrarAyuda(){

	printf("#Para ejecutar una camara <./fifoB pathcamara>, debe antes abrir la tuberia con\n <./fifoA> en otra terminal, para mas info ejecute <./fifoA -h>\n");
}

int main(int argc, char *argv[]){

	int fifo;
	char nombre_fifo[] = "./fifo";
	FILE* pfCam = fopen (argv[1], "r");
	char reg [50];
	char nombre_camara[50];

	if(argc != 2){
		printf("Minimo debe haber dos argumentos: ./fifoB ./nombreArchivoCamara\n " );
		return -1;
	}	

	if(argc == 2 && strcmp(argv[1], "-h") == 0){
		mostrarAyuda();
		return 0;
	}

	if(!pfCam){
		printf("No se pudo abrir el archivo de la camara\n");
		return -1;
	}

	strcpy(nombre_camara,argv[1]);
	char *directorio = strrchr(nombre_camara,'/');
	if(directorio){
		strcpy(nombre_camara,directorio+1);
	}
	
	char *ext = strrchr(nombre_camara,'.');
	if(ext){
		*ext = '\0';
		strcpy(nombre_camara,nombre_camara);

	}
	fifo = open(nombre_fifo, O_WRONLY);
	if(fifo<0){
		printf("El fifo no se encuentra creado");
		return -1;
	}

	while(fscanf(pfCam,"%[^\n]\n",reg) == 1){
		sleep(rand() % 5 + 1);;
		strcat(reg,"_");
		strcat(reg,nombre_camara);
		write(fifo,reg,TAMBUF);
	}
	close(fifo);
	fclose(pfCam);
	printf("Fin de escritura en fifo de la camara %s\n",nombre_camara);
	return 0;
}