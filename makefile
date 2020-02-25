FLAGS=-W -Wall -pedantic

all: main clean

main: MySemaphore.o CPU.o TableAllocation.o GenerateurProc.o main.o
	gcc -o leSuperCPU MySemaphore.o GenerateurProc.o TableAllocation.o CPU.o main.o $(FLAGS) -lpthread

MySemaphore.o: MySemaphore.c
	gcc -o MySemaphore.o -c MySemaphore.c $(FLAGS)

TableAllocation.o: TableAllocation.c MySemaphore.h
	gcc -o TableAllocation.o -c TableAllocation.c $(FLAGS)

CPU.o: CPU.c MySemaphore.h
	gcc -o CPU.o -c CPU.c $(FLAGS)

GenerateurProc.o : GenerateurProc.c MySemaphore.h
	gcc -o GenerateurProc.o -c GenerateurProc.c $(FLAGS)

main.o: main.c TableAllocation.h CPU.h MySemaphore.h
	gcc -o main.o -c main.c $(FLAGS)

clean:
	rm -rf *.o

mrproper: clean
	rm -rf leSuperCPU