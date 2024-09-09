# Trabalho 2 - Diassembler
### Aluna: Luiza de Araújo Nunes Gomes
### Matrícula: 190112794

## Como rodar o projeto?
``make``
## Arquivos presentes no projeto
* **diassembler.c**: arquivo em c com o código do projeto
* **diassembler.h**: arquivo em c com o header do projeto
* **makefile**: makefile para gerar o ./tradutor
* **README.md**: guia do projeto
* **pasta de exemplos**
  * _caso1.obj_: caso teste do enunciado do projeto
  * _caso2.obj_: soma de dois números
  * _caso3.obj_, _caso4.obj_, _caso5.obj_, _caso6.obj_: casos aleatórios
## Especificações
### Versão GCC
gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0
### Sistema Operacional
WSl Ubuntu 22.04
## Sobre as funções utilizadas no projeto
### S_INPUT: função de input que recebe string
Sendo o primeiro parâmetro o tamanho da variável à ser recebida e o segundo parâmetro é a variável onde será guardada o valor recebido.
```
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
```
### STRING_TO_INT: função que converte a string para integer
Sendo o primeiro parâmetro é variável que será guardada a variável após a conversão e o segundo parâmetro é a variável que será convertida.
````
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
````
### INPUT: Função que recebe a string do terminal, passa pra integer e retorna
Sendo o primeiro parâmetro é variável que será guardada a variável após a conversão para integere o segundo parâmetro é a variável em que o valor em string será guardado inicialmente, pode ser um _inputBuffer_.
```
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
```
### S_OUTPUT: Função que escreve no terminal uma string
O primeiro parâmetro é a variável à ser impressa, e o segundo é o tamanho dessa variável.
```
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
```
### INT_TO_STRING: Função que converte int para string
O primeiro parâmetro é a variável em que ficará guardada a variável convertida, e o segundo parâmetro é a string à ser convertida.
```
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
```
### OUTPUT: Imprime no terminal uma integer e um '\n'
O primeiro parâmetro é o número à ser impresso, e o segundo parâmetro é a string em que será guardado o número a ser impresso.
```
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
```
### bytes_read_written: retorna a quantidade de bytes lidos/escritos
Recebe a variável que necessita ser lida/escrita para contar a quantidade bytes.
```
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
```
### OVERFLOW: Detecta o overflow e encerra o programa
```
OVERFLOW:
	mov eax, 4
	mov ebx, 1
	lea ecx, [overflow_msg]
	mov edx, 18
	int 0x80
    mov eax, 1
	xor ebx, ebx
	int 0x80
```
### clear_buffer: Limpa o buffer
Normalmente utilizado para limpar os buffers utilizados no projeto. O primeiro parâmetro é o tamanho do buffer, o segundo é o buffer em si.
```
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
```
## Parte do Section .data obrigatória
```
	inputBuffer db 16 dup(0)
	overflow_msg db "Overflow detectado!", 0xA, 0
	countBytesBuffer1 db 16 dup(0)
	msg1 db "<Foram lidos/escritos ", 0
	msg2 db " bytes>",0
	newline db 0xA
	ten dd 10
```