section .text
	global open
	
open:
	push ebp
	mov ebp, esp
	sub esp, 4
	pushad
	mov eax, 5
	mov ebx, [ebp+8]
	mov ecx, 2
	int 0x80
	mov [ebp-4], eax
	popad
	mov eax, [ebp-4]
	add esp, 4
	pop ebp
	ret

close:
	push ebp
	mov ebp, esp
	sub esp, 4
	pushad
	mov eax, 6
	mov ebx, [ebp+8]
	int 0x80
	mov [ebp-4], eax
	popad
	mov eax, [ebp-4]
	add esp, 4
	pop ebp
	ret