section .data
	inputBuffer db 16 dup(0)
	overflow_msg db "Overflow detectado!", 0xA, 0
	countBytesBuffer1 db 16 dup(0)
	msg1 db "<Foram lidos/escritos ", 0
	msg2 db " bytes>",0
	newline db 0xA
	ten dd 10
	VARIABLE12 dd 2
	VARIABLE37 dd 1
	VARIABLE38 dd 0
	VARIABLE39 dd 2
section .bss
	VARIABLE10 resd 1
	VARIABLE11 resd 1
	VARIABLE36 resd 1
section .text
	global _start
S_INPUT:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	mov edx, [ebp+12]
	mov eax, 3
	mov ebx, 0
	int 80h
	push eax
	call bytes_read_written
	mov eax, 4
	mov ebx, 1
	mov ecx, newline
	mov edx, 1
	int 0x80
	pop eax
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

INPUT:
	push ebp
	mov ebp, esp
	push dword 16
	push dword [ebp+8]
	call S_INPUT
	add esp, 8
	push dword [ebp+12]
	push dword [ebp+8]
	call STRING_TO_INT
	add esp, 8
	pop ebp
	ret

OUTPUT:
	push ebp
	mov ebp, esp
	push dword [ebp+12]
	push dword [ebp+8]
	call INT_TO_STRING
	add esp, 8
	push dword 16
	push dword [ebp+12]
	call S_OUTPUT
	add esp, 8
	pop ebp
	ret

INT_TO_STRING:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov ebx, [ebp+12]
	mov edi, ebx
	add edi, 14
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

S_OUTPUT:
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
	push eax
	call bytes_read_written
	pop eax
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
	mov eax, 1
	xor ebx, ebx
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

bytes_read_written:
	push ebp
	mov ebp, esp
	mov ecx, [ebp+8]
	push countBytesBuffer1
	push dword ecx
	call INT_TO_STRING
	mov eax, 4
	mov ebx, 1
	mov ecx, msg1
	mov edx, 22
	int 0x80
	mov eax, 4
	mov ebx, 1
	mov ecx, countBytesBuffer1
	mov edx, 14
	int 0x80
	mov eax, 4
	mov ebx, 1
	mov ecx, msg2
	mov edx, 7
	int 0x80
	push 16
	push countBytesBuffer1
	call clear_buffer
	mov esp, ebp
	pop ebp
	ret

_start:
	push dword VARIABLE10
	push dword inputBuffer
	call INPUT
	push 16
	push inputBuffer
	call clear_buffer
	mov eax, [VARIABLE12]
	imul eax, [VARIABLE10]
	jo OVERFLOW
	mov [VARIABLE11], eax
	jmp LABEL13
			LABEL13: push dword VARIABLE36
	push dword inputBuffer
	call INPUT
	push 16
	push inputBuffer
	call clear_buffer
	sub eax, [VARIABLE11]
	cmp eax,0
	jg LABEL23
	cmp eax,0
	jl LABEL27
	cmp eax,0
	je LABEL31
LABEL23: push inputBuffer
	push dword [VARIABLE37]
	call OUTPUT
	push 16
	push inputBuffer
	call clear_buffer
	jmp LABEL35
LABEL27: push inputBuffer
	push dword [VARIABLE39]
	call OUTPUT
	push 16
	push inputBuffer
	call clear_buffer
	jmp LABEL35
LABEL31: push inputBuffer
	push dword [VARIABLE38]
	call OUTPUT
	push 16
	push inputBuffer
	call clear_buffer
	jmp LABEL35
LABEL35: mov eax, 1
	xor ebx, ebx
	int 0x80
				