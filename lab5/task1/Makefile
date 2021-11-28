%.o: %.asm
	nasm -g -O1 -f elf -o $@ $<

%.o: %.c
	gcc -m32 -g -nostdlib -fno-stack-protector -c -o $@ $<

all: lwca task1c

lwca: lwc.o start.o utoa_s.o atou_s.o
	ld  -melf_i386 -o $@ $^

task1c:	atou_s.o utoa_s.o task1c.c
	gcc -m32 -c -o task1c.o task1c.c
	gcc -m32 -o task1c atou_s.o utoa_s.o task1c.o

clean:
	rm -f *.o lwca
