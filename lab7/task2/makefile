all: mypipeline

mypipeline: task2.o
	gcc -g -Wall -o mypipeline task2.o

task2.o: task2.c
	gcc -g -Wall -c -o task2.o task2.c
	
.PHONY: clean val

clean:
	rm -rf *.o mypipeline

run: mypipeline
	./mypipeline

drun: mypipeline
	./mypipeline -d



val: mypipeline
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./mypipeline

dval: mypipeline
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./mypipeline -d