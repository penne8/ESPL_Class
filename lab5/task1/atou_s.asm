section .data
	position_mul: dd 1
	mul_inc: dd 10

section .text
    global atou_s 

;int atou_s(char* c)
atou_s:
	; stack maintenance
    push 	ebp 
	push 	ebx
	push 	ecx
	push	edx
	mov 	ebp, esp

	mov		ebx, dword [ebp+20]	; char *c at place 0
	mov		ecx, 0				; returned value

get_least_significant:
	cmp		[ebx], BYTE 0 
	je		dec_pointer
	inc		ebx
	jmp		get_least_significant

dec_pointer:
	dec		ebx	; pointer to the least significant digit

whileNotFinished:
	movzx	eax, BYTE[ebx]
	sub		eax, '0'
	mul		dword [position_mul]
	add		ecx, eax

	mov		eax, dword [position_mul]
	mul		dword [mul_inc]
	mov		[position_mul], eax

	dec		ebx

checkFinished:
	cmp		ebx, dword [ebp+20]	; if we arrived to the begining of the str minus 1, finish
	jae		whileNotFinished	; jump if ebx >= char* c

FINISH:
	mov 	eax, ecx	; save result pointer to start of string into eax
	; stack maintenance
	mov 	esp, ebp
	pop		edx
	pop 	ecx
	pop 	ebx
	pop 	ebp 
	ret