all: servidor cliente

servidor: servidor.c lista.c
		gcc -o servidor servidor.c lista.c -lpthread

cliente: cliente.c
		gcc -o cliente cliente.c -lpthread

clean:
		rm *.o
