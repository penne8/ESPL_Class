all: exec

exec: task2.o LineParser.o
	gcc -g -Wall -o mypipeline task2.o LineParser.o

task2.o: task2.c
	gcc -g -Wall -c -o task2.o task2.c

LineParser.o: LineParser.c
	gcc -g -Wall -c -o LineParser.o LineParser.c
	
.PHONY: clean val

clean:
	rm -rf *.o mypipeline

run:
	./mypipeline

drun:
	./mypipeline -d

val:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./mypipeline

dval:
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./mypipeline -d