section .data
	buff: times 50 db 0
    curr: dd -1

section .text

global _start

extern read		; int read(int fd, char *buf, int size);
extern write	; int write(int fd, char *buf, int size);
extern open		; int open(char *name, int flags);
extern close	; int close(int fd);
extern strlen	; int strlen(char *s);

_start:
	mov 	ecx, esp
	add 	ecx, 8		; char *argv[1]
    mov     ecx, [ecx]  ; char *str_file_name

open_file:
    push    0
    push    ecx
    call    open
    mov     ebx, eax    ; fd

read_file:
    push    1
    push    buff
    push    ebx         ; fd
    call    read
    add     dword [curr], 1

check_EOF:
    mov     edx, buff
    add     edx, [curr]
    cmp     BYTE [edx], -1  ; check if we read EOF
    je      close_file

write_file:
    push    1                   ; size
    push    edx    ; buffer
    push    1                   ; fd
    call    write

    jmp     read_file

close_file:
    push    ecx
    call    close

exit:
    mov ebx, 0
    mov eax, 1
    int 80h