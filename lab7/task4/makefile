all: exec

exec: task4.o LineParser.o pipes.o
	gcc -g -Wall -o myshell task4.o LineParser.o pipes.o

task4.o: task4.c task4.h
	gcc -g -Wall -c -o task4.o task4.c

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c
	
pipes.o: pipes.c 
	gcc -g -Wall -c -o pipes.o pipes.c

.PHONY: clean val

clean:
	rm -rf *.o myshell

run:
	clear
	./myshell

val:
	clear
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./myshell