section .text
	
global _start
global read		; int read(int fd, char *buf, int size);
global write	; int write(int fd, char *buf, int size);
global open		; int open(char *name, int flags);
global close	; int close(int fd);
global strlen	; int strlen(char *s);

extern main		; int main(int argc, char **argv)

_start:
	mov 	ebx,dword [esp]	; int argc
	mov 	ecx, esp
	add 	ecx, 4			; char **argv
	push 	ecx				; char **argv
	push 	ebx				; int argc 
	call	main
    mov     ebx,eax
	mov		eax,1
	int 	0x80

; int read(int fd, char *buf, int size);
read:
	push 	ebp 			; stack maintenance
	push 	ebx				; stack maintenance
	push 	ecx				; stack maintenance
	push 	edx				; stack maintenance
	mov 	ebp, esp 		; stack maintenance

	mov 	eax, 3			; sys_read
	mov 	ebx, [ebp+20]	; int fd
	mov 	ecx, [ebp+24]	; char *buf
	mov 	edx, [ebp+28]	; int sizeSS
	int 	0x80

	mov 	esp, ebp 		; stack maintenance
	pop		edx				; stack maintenance
	pop		ecx				; stack maintenance
	pop 	ebx				; stack maintenance
	pop 	ebp 			; stack maintenance
	ret

; int write(int fd, char *buf, int size);
write:
	push 	ebp 			; stack maintenance
	push 	ebx				; stack maintenance
	push 	ecx				; stack maintenance
	push 	edx				; stack maintenance
	mov 	ebp, esp 		; stack maintenance

	mov 	eax,4			; sys_write
	mov 	ebx, [ebp+20]	; int fd
	mov 	ecx, [ebp+24]	; char *buf
	mov 	edx, [ebp+28]	; int size
	int 	0x80

	mov 	esp, ebp 		; stack maintenance
	pop 	edx				; stack maintenance
	pop 	ecx				; stack maintenance
	pop 	ebx				; stack maintenance
	pop 	ebp 			; stack maintenance
	ret

; int open(char *name, int flags);
open:
	push 	ebp 			; stack maintenance
	push 	ebx				; stack maintenance
	push 	ecx				; stack maintenance
	mov 	ebp, esp 		; stack maintenance

	mov 	eax,5			; sys_open
	mov 	ebx, [ebp+16]	; char *name
	mov 	ecx, [ebp+20]	; int flags
	int 	0x80

	mov 	esp, ebp 		; stack maintenance
	pop 	ecx				; stack maintenance
	pop 	ebx				; stack maintenance
	pop 	ebp 			; stack maintenance
	ret

; int close(int fd);
close:
	push 	ebp 			; stack maintenance
	push 	ebx				; stack maintenance
	mov 	ebp, esp 		; stack maintenance
	
	mov 	eax,6			; sys_close
	mov 	ebx, [ebp+12]	; int fd
	int 	0x80

	mov 	esp, ebp 		; stack maintenance
	pop 	ebx				; stack maintenance
	pop 	ebp 			; stack maintenance
	ret

; int strlen(char *s);	
strlen:
	push 	ebp 			; stack maintenance
	push 	ebx				; stack maintenance
	mov 	ebp, esp 		; stack maintenance

	mov 	eax, 0			; initialize counter
	mov 	ebx, [ebp+12]	; char *s

strlen_loop:
	cmp 	[ebx], BYTE 0	; check if reached end of string
	je 		return			; if so, return
	inc 	eax				; else, increment counter
	inc 	ebx				; increment pointer
	jmp 	strlen_loop
	
return:
	mov 	esp, ebp 		; stack maintenance
	pop 	ebx				; stack maintenance
	pop 	ebp 			; stack maintenance
	ret