fifoA fifoB: fifoA.o fifoB.o
		gcc -o fifoA fifoA.o
		gcc -o fifoB fifoB.o

fifoA.o fifoB.o: fifoA.c fifoB.c
	gcc -c fifoA.c fifoB.c

clean:
	rm*.o