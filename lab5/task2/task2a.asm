section .data
	buff: times 50 db 0

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
    push    0           ; open file for read-only
    push    ecx         ; name of file
    call    open
    mov     ebx, eax    ; fd

read_file:
    push    50          ; read 50 chars at a time
    push    buff
    push    ebx         ; fd
    call    read

check_EOF:
    cmp     eax, 0      ; check if we read EOF
    je      close_file

write_file:
    push    eax         ; write as many as was read
    push    buff
    push    1           ; stdout fd
    call    write

    jmp     read_file

close_file:
    push    ebx     ; fd
    call    close

exit:
    mov ebx, 0
    mov eax, 1
    int 80h