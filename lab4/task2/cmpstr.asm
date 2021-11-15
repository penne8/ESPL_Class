section .text
	global cmpstr
	
cmpstr:
	push ebp
	push ebx ; 1st array curr char
	;push ecx ; 2nd array curr char --- why is this happening??
	mov	ebp, esp
	mov	eax,-1 ; curr element index in array

equal:
	inc eax
	mov ebx, eax
	add ebx, [ebp+12]
	movzx ebx, BYTE [ebx]

	mov ecx, eax
	add ecx, [ebp+16]
	movzx ecx, BYTE [ecx]
	
	test bl,bl
	je ended

	test cl,cl
	je ended

	sub ebx, ecx
	je equal

notEqual:
	mov eax, ebx
	jmp FINISH

ended:
	sub ebx, ecx
	mov eax, ebx
	jmp FINISH

FINISH:
	mov esp, ebp ;stack maintenance
	;pop ecx ----  why is this happening??
	pop ebx
	pop ebp ;stack maintenance
	ret ;stack maintenance