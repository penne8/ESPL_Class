all:patch my_fib hasty_fib

patch: patch.o
	gcc -m32 -fno-pie -o patch patch.o

patch.o: patch.c
	gcc -c -m32 -fno-pie -o patch.o patch.c 


hasty_fib: hasty_fibonacci.o
	gcc -m32 -fno-pie -o hasty_fib hasty_fibonacci.o

hasty_fibonacci.o: hasty_fibonacci.c
	gcc -c -m32 -fno-pie -o hasty_fibonacci.o hasty_fibonacci.c

my_fib: my_fibonacci.o
	gcc -m32 -fno-pie -o my_fib my_fibonacci.o

my_fibonacci.o: my_fibonacci.c
	gcc -c -m32 -fno-pie -o my_fibonacci.o my_fibonacci.c



.PHONY: run dis1 dis2 clean

run: patch my_fib hasty_fib
	./patch my_fib 4ed 84 hasty_fib 65d
	./hasty_fib 33

dis1: my_fib
	objdump -d -R tamy_fib

dis2: hasty_fib
	objdump -d -R hasty_fib

clean:
	rm -rf *.o patch my_fib hasty_fib