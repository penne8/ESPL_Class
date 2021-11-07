exec: task1a.o
	gcc -g -m32 -Wall -ansi -o bcmp task1a.o

task1a.o: task1a.c
	gcc -g -m32 -Wall -ansi -c -o task1a.o task1a.c

.PHONY: clean

clean:
	rm *.o bcmp

val:
	valgrind --leak-check=full --show-reachable=yes ./bcmp
