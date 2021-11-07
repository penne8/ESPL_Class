exec: task1c.o
	gcc -g -m32 -Wall -ansi -o bcmp task1c.o

task1c.o: task1c.c
	gcc -g -m32 -Wall -ansi -c -o task1c.o task1c.c

task1b.o: task1b.c
	gcc -g -m32 -Wall -ansi -c -o task1b.o task1b.c

task1a.o: task1a.c
	gcc -g -m32 -Wall -ansi -c -o task1a.o task1a.c

.PHONY: clean

clean:
	rm *.o bcmp

val1:
	valgrind --leak-check=full --show-reachable=yes ./bcmp

val2:
	valgrind --leak-check=full --show-reachable=yes ./bcmp 1 2
