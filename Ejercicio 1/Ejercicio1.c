#include <sys/types.h> 
#include <sys/stat.h> 
#include<sys/wait.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <errno.h> 
#include <unistd.h>
#include <syslog.h> 
#include <string.h>
#include<signal.h>

int main(int argc, char *argv[])
{
					int l;
					int i,j,k;
					char help;
					fflush(stdin);
						if(argc!=2){
					printf("\nCantidad de Argumentos no validos.\nIngrese -a para obtener ayuda.\nEjemplo \n\t $gcc ./Ejercicio1.c\n\t ./a.out -a\n");
					exit(0);
						}
						if(strcmp(argv[1],"-a")==0)
						{
					printf("\n------\nAyuda\n------\n\nTrabajo practico nº3 -- Ejercicio 1\n---------------------------------------------------------\nIntegrantes:\nAmato, Luciano\t\t\tDNI:  	40.378.763\nPompeo, Nicolas Ruben\t\tDNI:	37.276.705\nDel Greco, Juan Pablo\t\tDNI:	39.097.812\nMeza,Julian	\t\tDNI:	39.463.982\n---------------------------------------------------------\n\n\nEntrega:1/7/2019 - Lunes\n\n1er Entrega\n\nEste Script tiene 3 formas de Ejecucion. \nSi preciona la tecla...\n");
					printf("\n\n");
printf("-d\tcrea dos procesos demonios, que deberán simplemente ejecutar cada 2 segundos un\n loop hasta 1.000.000 realizando una acumulación en una variable, y no finalizan \n(simplemente para que generen algo de procesamiento).\n");
printf("-h\tcrea 10 procesos hijos, donde cada uno realiza el mismo procesamiento que en el caso\n anterior, el padre se quedará esperando a que se presione una tecla y en este momento se cierran\ntodos los procesos.\n");
printf("-z\tcrea 6 procesos nietos, de dos hijos distintos, y 2 de ellos deberán quedar como\nzombies. El padre se quedará esperando a que se presione una tecla para poder verificar la\ncorrecta generación de los procesos, y luego cierra los procesos.\n");
				
					}
					int acum=0;
					if(strcmp(argv[1],"-d")==0)
						{
							pid_t piddemon = fork();
							pid_t piddemon2;
								if(piddemon==-1){
									puts("No se puede crear");
									exit(1);
								}
								if(piddemon == 0){
								
													printf("\nPID: %d ; Pid padre: %d, Parentezco=Hijo Tipo: DEMONIO\n", getpid(),getppid());
													while(1){
														acum=0;
													while(acum<1000000){
														acum++;}
													sleep(1);																		
																}
								}
							
							if(piddemon>0) // vuelve al padre y crea otro hijo
							{
							
								piddemon2= fork();
							if(piddemon2==-1){
								puts("No se puede crear");
								exit(1);
								}
							if(piddemon2 == 0){
							printf("\nPID: %d ; Pid padre: %d, Parentezco=Hijo Tipo: DEMONIO\n", getpid(),getppid());
													while(1){
													acum=0;
													while(acum<1000000){
														acum++;
														}
													sleep(1);																		
																}
							}
							}
							
						}
				
						if(strcmp(argv[1],"-h")==0)
						{
					pid_t pid ;
				    	pid_t ppid ;
					pid_t pid1;
					pid_t pid2  ;
					pid_t pid3  ;
					pid_t pid4  ;
					pid_t pid5  ;
					pid_t pid6  ;
					pid_t pid7  ;
					pid_t pid8  ;
					pid_t pid9  ;
					pid_t pid10 ;
					int acum=0;
				int i=0;
printf("\n");
				//////Creacion de procesos//////////
				for(i=1;i<=10;i++)
				{

						pid = fork();
						
						if ( pid == -1 ){
							printf("\nError al crear nuevo proceso\n");
								   return 1;
						}
						
						if ( pid == 0 ) {
			                //formula para calcular el numero unico de este proceso
							
							printf("PID: %d - Pid Padre: %d - Parentezco: Hijo - Tipo: Normal\n",getpid(),getppid());
							acum=0;
							while(acum<1000000){
								acum++;		
							}
exit(1);
						}				
						
						else {
							if ( i == 1 ) {

								pid1 = pid;	
									
							}
							if ( i == 2 ) {

								pid2 = pid;				
														
							}
							if ( i == 3 ) {

								pid3 = pid;
			
							}
							if ( i == 4 ) {

								pid4 = pid;				
							}
							if ( i == 5 ) {

								pid5 = pid;				
							}
							if ( i == 6 ) {

								pid6 = pid;						
							}
							if ( i == 7 ) {

								pid7 = pid;					
							}if ( i == 8 ) {

								pid8 = pid;				
							}if ( i == 9 ) {

								pid9 = pid;						
							}if ( i == 10 ) {

								pid10 = pid;					
							}
						}
			}
					
			
				/*	printf("\n\nIngrese una tecla para finalizar....");
					kill (pid1, SIGKILL);
						kill (pid2, SIGKILL);
						kill (pid3, SIGKILL);
						kill (pid4, SIGKILL);
						kill (pid5, SIGKILL);
						kill (pid6, SIGKILL);
						kill (pid7, SIGKILL);
						kill (pid8, SIGKILL);
						kill (pid9, SIGKILL);
						kill (pid10, SIGKILL);*/
return 0;
				
			}
				
				
						if(strcmp(argv[1],"-z")==0)
{
					int i=0,j=0;
					pid_t pid=fork();
							if(pid == -1){
								printf("No se puede crear Proceso");
								exit(0);
							}
							if(pid == 0){
									printf("PID: %d - Pid Padre: %d - Parentezco: Hijo -Tipo: Normal.\n",getpid(),getppid());
									 for(j=0;j<4;j++){
										 pid_t pidh = fork();
									 		if(pidh == -1){
												printf("No se puede crear Proceso");
												exit(0);
												}
											if(pidh==0){
												printf("PID: %d - Pid Padre: %d - Parentezco: Nieto - Tipo: Normal.\n",getpid(),getppid());
												while(1);
												exit(0);
												}
											
										}
					while(1);
							 }
					i=0;
					j=0;
					char t;
					pid_t  pid2;
					if(pid2 == -1){
						printf("No se puede crear Proceso");
						exit(0);
						}
					if(pid2 == 0){
							printf("PID: %d - Pid Padre: %d - Parentezco: Hijo -Tipo:  Normal.\n",getpid(),getppid());
							
							 for(j=0;j<2;j++){
								 pid_t pidh2 = fork();
							 	if(pidh2 == -1){
									printf("No se puede crear Proceso");
									exit(0);
								}
								if(pidh2==0){
									printf("PID: %d - Pid Padre: %d - Parentezco: Nieto- Tipo:  Zombie.\n",getpid(),getppid());
									exit(0);
								}
							
						 } 
						while(1);
					}
	
			getchar();
	kill(pid2,SIGKILL);
kill(pid,SIGKILL);
				return 0;
				
	}
return 0 ;
	
	}