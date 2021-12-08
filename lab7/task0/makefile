all: exec

exec: task0.o
	gcc -g -Wall -o mypipe task0.o

task0.o: task0.c
	gcc -g -Wall -c -o task0.o task0.c
	
.PHONY: clean val

clean:
	rm -rf *.o mypipe

run:
	./mypipe