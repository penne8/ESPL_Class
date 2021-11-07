exec: bubblesort.o hexaPrint.o
	gcc -g -m32 -Wall -ansi -o bubblesort bubblesort.o
	gcc -g -m32 -Wall -ansi -o hexaPrint hexaPrint.o

bubblesort.o: bubblesort.c
	gcc -g -m32 -Wall -ansi -c -o bubblesort.o bubblesort.c

hexaPrint.o: hexaPrint.c
	gcc -g -m32 -Wall -ansi -c -o hexaPrint.o hexaPrint.c

.PHONY: clean

clean:
	rm *.o bubblesort hexaPrint

val:
	valgrind --leak-check=full --show-reachable=yes ./hexaPrint exampleFile.txt
