%.o: %.asm
	nasm -g -O1 -f elf -o $@ $<

%.o: %.c
	gcc -m32 -g -nostdlib -fno-stack-protector -c -o $@ $<

all: task2

task2: system_calls.o utoa_s.o atou_s.o task2.o
	ld  -melf_i386 -o $@ $^

clean:
	rm -f *.o task2
