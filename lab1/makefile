exec: task2.o
	gcc -g -m32 -Wall -ansi -o char2ascii task2.o

task2.o: task2.c
	gcc -g -m32 -Wall -ansi -c -o task2.o task2.c

.PHONY: clean

clean:
	rm *.o char2ascii