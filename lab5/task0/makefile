all: prog

prog: prog.o progasm.o
	gcc -m32 -o prog prog.o progasm.o

progasm.o: prog.asm
	nasm -g -f elf -w+all -o progasm.o prog.asm

prog.o: prog.c
	gcc -m32 -c -o prog.o prog.c
	
.PHONY: clean

clean:
	rm -rf *.o prog