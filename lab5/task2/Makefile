%.o: %.asm
	nasm -g -O1 -f elf -o $@ $<

%.o: %.c
	gcc -m32 -g -nostdlib -fno-stack-protector -c -o $@ $<

all: task2b

task2a: system_calls.o utoa_s.o atou_s.o task2a.o
	ld  -melf_i386 -o $@ $^

task2b: system_calls.o utoa_s.o atou_s.o task2b.o
	ld  -melf_i386 -o $@ $^

clean:
	rm -f *.o task2a task2b
