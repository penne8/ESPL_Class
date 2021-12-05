all: exec

exec: task2.o LineParser.o
	gcc -g -Wall -o myshell task2.o LineParser.o

task2.o: task2.c
	gcc -g -Wall -c -o task2.o task2.c

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean:
	rm -rf *.o myshell