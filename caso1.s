section .data
	inputBuffer db 16 dup(0)
	overflow_msg db "Overflow detectado!", 0xA, 0
	newline db 0xA
	ten dd 10
	VARIABLE28 dd 2
	VARIABLE29 dd 0
	VARIABLE30 dd 0
	VARIABLE31 dd 0
section .bss
section .text
	global _start
INPUT:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	mov edx, [ebp+12]
	mov eax, 3
	mov ebx, 0
	int 80h
	pop ebp
	ret

STRING_TO_INT:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	mov ebx, [ebp+12]
	xor eax, eax
	xor edx, edx
	mov esi, 1
	movzx edx, byte [ecx]
	cmp edx, '-'
	jne check_positive
	mov esi, -1
	inc ecx
	jmp convert_loop
check_positive:
	cmp edx, '+'
	jne convert_loop
	inc ecx
convert_loop:
	movzx edx, byte [ecx]
	test edx, edx
	jz done
	cmp edx, '0'
	jb done
	cmp edx, '9'
	ja done
	sub edx, '0'
	imul eax, eax, 10
	add eax, edx
	inc ecx
	jmp convert_loop
 done:
	imul eax, esi
	mov [ebx], eax
	pop ebp
	ret

INT_TO_STRING:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov ebx, [ebp+12]
	mov edi, ebx
	add edi, 10
	mov byte [edi], 0
	dec edi
	cmp eax, 0
	je print_zero
	js handle_negative
convert_loop_its:
	xor edx, edx
	div dword [ten]
	add dl, '0'
	mov [edi], dl
	dec edi
	test eax, eax
	jnz convert_loop_its
	inc edi
	jmp done_its
handle_negative:
	neg eax
	jmp convert_loop_its
done_its:
	cmp byte [edi-1], '-'
	jne end_negative
	mov byte [edi], '-'
	dec edi
end_negative:
	inc edi
	mov eax, edi
	pop ebp
	ret
print_zero:
	mov byte [ebx], '0'
	mov byte [ebx+1], 0
	mov eax, ebx
	pop ebp
	ret

OUTPUT:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	mov edx, [ebp+12]
	mov eax, 4
	mov ebx, 1
	int 0x80
	mov eax, 4
	mov ebx, 1
	mov ecx, newline
	mov edx, 1
	int 0x80
	pop ebp
	ret

OVERFLOW:
	mov eax, 4
	mov ebx, 1
	lea ecx, [overflow_msg]
	mov edx, 18
	int 0x80

clear_buffer:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	mov edx, [ebp+12]
	xor eax, eax
	clear_loop:
	mov byte [ecx], al
	inc ecx
	dec edx
	jnz clear_loop
	pop ebp
	ret

_start:
	push 16
	push inputBuffer
	call INPUT
	add esp, 8
	push dword VARIABLE29
	push dword inputBuffer
	call STRING_TO_INT
	add esp, 8
	push 16
	push inputBuffer
	call clear_buffer
	mov eax, [VARIABLE29]
LABEL4: cdq
	idiv dword [VARIABLE28]
	mov [VARIABLE30], eax
	imul eax, [VARIABLE28]
	jo OVERFLOW
	mov [VARIABLE31], eax
	mov eax, [VARIABLE29]
	sub eax, [VARIABLE31]
	mov [VARIABLE31], eax
	push inputBuffer
	push dword [VARIABLE31]
	call INT_TO_STRING
	add esp, 8
	push dword 16
	push dword inputBuffer
	call OUTPUT
	add esp, 8
	push 16
	push inputBuffer
	call clear_buffer
	mov eax, [VARIABLE30]
	mov [VARIABLE29], eax
	mov eax, [VARIABLE29]
	cmp eax,0
	jg LABEL4
	mov eax, 1
	xor ebx, ebx
	int 0x80
				