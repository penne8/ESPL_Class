section .data
	buff: times 50 db 0
    word_count: dd 0
    read_count: dd 0
    in_word: db 0

section .text

global _start

extern read		; int read(int fd, char *buf, int size);
extern write	; int write(int fd, char *buf, int size);
extern open		; int open(char *name, int flags);
extern close	; int close(int fd);
extern strlen	; int strlen(char *s);
extern utoa_s   ; char *utoa_s(int i);

_start:
	mov 	ecx, esp
	add 	ecx, 8		; char *argv[1]
    mov     ecx, [ecx]  ; char *str_file_name or flag

check_flag:
    cmp     BYTE [ecx], '-'
    jne     open_file

    inc     ecx
    cmp     BYTE [ecx], 'w'
    jne     open_file

    inc     ecx
    cmp     BYTE [ecx], 0
    jne     open_file
    
    mov 	ecx, esp
	add 	ecx, 12		; char *argv[1]
    mov     ecx, [ecx]  ; char *str_file_name or flag


cw_open_file:
    push    0           ; open file for read-only
    push    ecx         ; name of file
    call    open
    mov     ebx, eax    ; fd

cw_read_file:
    push    50          ; read 50 chars at a time
    push    buff
    push    ebx         ; fd
    call    read
    mov     [read_count], eax

cw_check_EOF:
    cmp     dword [read_count], 0      ; check if we read EOF
    je      write_count

    mov     edx, 0

count_words:
    cmp     edx, dword [read_count]
    je      cw_read_file

    mov     eax, buff
    add     eax, edx

    cmp     BYTE [eax], 10  ; '\n'
    je      not_in_word

    cmp     BYTE [eax], 32  ; ' '
    je      not_in_word

    cmp     BYTE [eax], 9   ; '/t'
    je      not_in_word

    jmp check_and_inc_in_word

not_in_word:
    mov     BYTE [in_word], 0
    jmp     incrementor

check_and_inc_in_word:
    cmp     BYTE [in_word], 1
    je      incrementor
    
    inc     dword [word_count]
    mov     BYTE [in_word], 1

incrementor:
    inc     edx
    jmp     count_words



write_count:
    push    dword [word_count]
    call    utoa_s
    mov     edx, eax    ; pointer of number to write

    push    eax
    call    strlen

    push    eax         ; the number representation length
    push    edx         ; pointer of number to write
    push    1           ; stdout fd
    call    write

    jmp     close_file




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