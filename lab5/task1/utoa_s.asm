%define UTOA_BUFLEN 16	; for utoa_s

section .bss
str: RESB UTOA_BUFLEN	; for utoa_s

section .text
	global 	utoa_s
    global 	atou_s 

;char *utoa_s(int i)
utoa_s:
	; stack maintenance
    push 	ebp 
	push 	ebx
	push 	ecx
	push	edx
	mov 	ebp, esp
    
	mov 	eax, dword [ebp+20] 	; ebx = i
	mov 	ecx, str				; pointer to str start position
	add		ecx, UTOA_BUFLEN		; pointer to str end+1
	dec		ecx						; pointer to str end
    mov 	[ecx], BYTE 0			; str[UTOA_BUFLEN-1] = 0;

	cmp 	eax, 0					; i == 0
	je 		isZero					; edge case

	mov 	ebx, 10	
	
whileNotZero:	
	dec 	ecx			; add char to string from left by decrementing pointer 
	mov 	edx, 0		; clear edx

	div 	ebx			; after division: eax = i/10, dl = i%10
	add 	edx, '0' 	; convert from number to ASCII
	mov 	[ecx], dl	; save converted number to str

checkZero:	
	cmp 	eax, 0			; check if (i == 0)
	jne 	whileNotZero	; jump if (i != 0)
	jmp 	FINISH

isZero: ; edge case
	dec 	ecx
	mov 	[ecx], BYTE '0'
	jmp 	FINISH

FINISH:
	mov 	eax, ecx	; save result pointer to start of string into eax
	; stack maintenance
	mov 	esp, ebp
	pop		edx
	pop 	ecx
	pop 	ebx
	pop 	ebp 
	ret