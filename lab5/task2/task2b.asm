section .bss
    ws_word: resb 32
    

section .data
	buff: times 50 db 0
    word_count: dd 0
    read_count: dd 0
    in_word: db 0
    ws_flag: db 0
    word_offset: dd 0
    good_word: db 0
    ; ws_word: dd 0
    file_des: dd 0

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
    jne     open_file           ; for non-w and non-ws occasions

    inc     ecx
    cmp     BYTE [ecx], 'w'
    jne     open_file           ; for non-w and non-ws occasions

    inc     ecx

    cmp     BYTE [ecx], 0
    je      set_w_file_name     ; for w flag
    
    cmp     BYTE [ecx], 's'
    jne     open_file           ; for non-w and non-ws occasions

    inc     ecx
    cmp     BYTE [ecx], 0
    je      set_ws_file_name    ; for ws flag

    jmp     open_file           ; for non-w and non-ws occasions

set_w_file_name:
    mov 	ecx, esp
	add 	ecx, 12		    ; char *argv[1]
    mov     ecx, [ecx]      ; char *str_file_name
    jmp     cw_open_file    ; for w and ws occasions

set_ws_file_name:
    mov 	ecx, esp
    add 	ecx, 12		            ; char *argv[1]
    mov     ebx, [ecx]              ; using ebx for next mov op
    mov     ebx, [ebx]
    mov     dword [ws_word], ebx    ; char *word

	add 	ecx, 4		            ; char *argv[2]
    mov     ecx, [ecx]              ; char *str_file_name

    mov     BYTE [ws_flag], 1       ; light up ws flag
    jmp     cw_open_file            ; for w and ws occasions

cw_open_file:
    push    0           ; open file for read-only
    push    ecx         ; name of file
    call    open
    mov     [file_des], eax    ; file_des

cw_read_file:
    push    50                  ; read 50 chars at a time
    push    buff
    push    dword [file_des]    ; file_des
    call    read
    mov     [read_count], eax

cw_check_EOF:
    cmp     dword [read_count], 0   ; check if we read EOF
    je      write_count

    mov     edx, 0                  ; reset buffer offset

count_words:
    cmp     edx, dword [read_count]     ; if end of buffer reached
    je      cw_read_file                ; take another buffer

    mov     eax, buff
    add     eax, edx                    ; eax = curr char from buff

    cmp     BYTE [eax], 10  ; '\n'
    je      not_in_word

    cmp     BYTE [eax], 32  ; ' '
    je      not_in_word

    cmp     BYTE [eax], 9   ; '/t'
    je      not_in_word

is_in_word:
    cmp     BYTE [ws_flag], 1
    je      ws_word_check

w_word_check:
    cmp     BYTE [in_word], 1   ; if not new word, skip to next char
    je      next_char

start_w_new_word:
    inc     dword [word_count]  ; arrived to new word, inc count
    mov     BYTE [in_word], 1   ; change flag
    jmp     next_char

ws_word_check:
    cmp     BYTE [in_word], 1       ; if not new word, continue check
    je      continue_ws_word_check

start_ws_new_word:
    mov     dword [word_offset], 0  ; for new word, start reading from the start of ws_word
    mov     BYTE [good_word], 1     ; for new word, assume good word

continue_ws_word_check:
    mov     ebx, ws_word           
    add     ebx, [word_offset]      ; ebx = current char at ws_word

    cmp     BYTE [ebx], 0           ; check if ebx point on ws_word ending
    je      bad_word                ; if so, its a bad word
    
    movzx   eax, BYTE [eax]         ; eax = curr char from buff. eax will reset in next char, needed to perfom cmp
    cmp     BYTE [ebx], al          ; check for equal chars on ws_word and curr buffer
    jne     bad_word
    
    inc     dword [word_offset]     ; word is good, increment
    jmp     next_char               ; and check next char

not_in_word:
    mov     BYTE [in_word], 0       ; reset in-word for next word apperance
    
    cmp     BYTE [ws_flag], 0       
    je      next_char

continue_ws_not_in_word:
    mov     ebx, ws_word
    add     ebx, dword [word_offset]    ; ebx = curr ws_word char

    cmp     BYTE [ebx], 0               ; check if ws_word reached end
    jne     bad_word                    ; if not, its bad word

    cmp     BYTE [good_word], 1         ; check if good word
    jne      next_char                  ; if not, skip to next char

    inc     dword [word_count]          ; word is good, increment count
    jmp     bad_word                    ; prevent double spaces to count word twice

bad_word:
    mov     BYTE [good_word], 0

next_char:
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


; without flags arrives here:

open_file:
    push    0           ; open file for read-only
    push    ecx         ; name of file
    call    open
    mov     dword [file_des], eax    ; file_des

read_file:
    push    50          ; read 50 chars at a time
    push    buff
    push    dword [file_des]         ; file_des
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
    push    dword [file_des]     ; fd
    call    close

exit:
    mov ebx, 0
    mov eax, 1
    int 80h