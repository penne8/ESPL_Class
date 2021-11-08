exec: task2.o
	gcc -g -m32 -Wall -ansi -o bcmp task2.o

task2.o: task2.c
	gcc -g -m32 -Wall -ansi -c -o task2.o task2.c

.PHONY: clean

clean:
	rm *.o bcmp

val1:
	valgrind --leak-check=full --show-reachable=yes ./bcmp

val2:
	valgrind --leak-check=full --show-reachable=yes ./bcmp 1 2

val3:
	valgrind --leak-check=full --show-reachable=yes ./bcmp -r 1 2