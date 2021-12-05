all: exec

exec: task1c.o LineParser.o
	gcc -g -Wall -o myshell task1c.o LineParser.o

task1c.o: task1c.c
	gcc -g -Wall -c -o task1c.o task1c.c

task1b.o: task1b.c
	gcc -g -Wall -c -o task1b.o task1b.c

task1a.o: task1a.c
	gcc -g -Wall -c -o task1a.o task1a.c

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c
	
.PHONY: clean

clean:
	rm -rf *.o myshell