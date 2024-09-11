# Trabalho 2 - Diassembler
### Aluna: Luiza de Araújo Nunes Gomes
### Matrícula: 190112794
### Repositório do projeto: https://github.com/luizangomes/t2-sb.git

## Como rodar o projeto?
Rode o seguinte no terminal:

``make``

Agora, você terá acesso à "./tradutor", que te permitirá passar códigos objetos em assembly inventado para IA32. Para fazer isso você precisa, para um programa na **_mesma pasta do projeto_**:

``
./tradutor codobjai.obj
``

Assim, na mesma pasta, o arquivo com o código em assembly ia32 será gerado.
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
Sendo o primeiro parâmetro o tamanho da variável à ser recebida e o segundo parâmetro é o endereço da variável onde será guardada o valor recebido.
```asm
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
Se chama da seguinte forma, exemplo:
```asm
push 1024      ; Tamanho do buffer
push buffer    ; Endereço do buffer
call S_INPUT   ; Chama a função S_INPUT
```
### STRING_TO_INT: função que converte a string para integer
Sendo o primeiro parâmetro é o endereço da variável que será guardada a variável após a conversão e o segundo parâmetro é o endereço da variável que será convertida.
```asm
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
```
Exemplo de como é utilizada:
```asm
push result          ; Endereço onde o valor convertido será armazenado
push str             ; Endereço da string contendo o número a ser convertido
call STRING_TO_INT   ; Chama a função STRING_TO_INT
```
### INPUT: Função que recebe a string do terminal, passa pra integer e retorna
Sendo o primeiro parâmetro é o ponteiro da variável que será guardada a variável após a conversão para integer e o segundo parâmetro é o ponteiro para variável em que o valor em string será guardado inicialmente, pode ser um _inputBuffer_.
```asm
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
Exemplo de utilização:
```asm
push result    ; Endereço onde o valor convertido será armazenado
push buffer    ; Endereço do buffer para armazenar a string
call INPUT     ; Chama a função INPUT

```
### S_OUTPUT: Função que escreve no terminal uma string
O primeiro parâmetro é o ponteiro para a variável à ser impressa, e o segundo é o número de bytes à serem escritos.
```asm
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
Exemplo de utilização:
```asm
push length    ; Número de bytes a serem escritos
push buffer    ; Endereço do buffer com os dados a serem impressos
call S_OUTPUT  ; Chama a função S_OUTPUT
```
### INT_TO_STRING: Função que converte int para string
O primeiro parâmetro é o número inteiro a ser convertido para string, o segundo parâmetyro é o buffer onde a string convertida será guardada.
```asm
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
Exemplo:
```asm
push buffer    ; Endereço do buffer para armazenar a string
push value     ; Valor inteiro a ser convertido
call INT_TO_STRING  ; Chama a função INT_TO_STRING
```
### OUTPUT: Imprime no terminal uma integer e um '\n'
O primeiro parâmetro é o valor inteiro a ser convertido para string, e o segundo é o ponteiro para o buffer onde a string será armazenada.
```asm
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
Exemplo:
```asm
push buffer    ; Endereço do buffer para armazenar a string
push value     ; Valor inteiro a ser convertido
call OUTPUT    ; Chama a função OUTPUT

```
### bytes_read_written: retorna a quantidade de bytes lidos/escritos
Recebe a variável que necessita ser lida/escrita para contar a quantidade bytes. Imprime a quantidade de bytes lidos ou escritos.
```asm
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
Exemplo, lembrando que deve ser utilizada sempre a variável **countBytesBuffer1**':
```asm
push countBytesBuffer1  ; Endereço do buffer para armazenar a string convertida
push byteCount          ; Quantidade de bytes a serem exibidos
call bytes_read_written ; Chama a função bytes_read_written
```
### OVERFLOW: Detecta o overflow e encerra o programa
``` asm
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
Normalmente utilizado para limpar os buffers utilizados no projeto. O primeiro parâmetro é o ponteiro do buffer a ser limpo, o segundo é o número de bytes a serem lidos.
```asm
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
Exemplo:
```asm
push numBytes      ; Número de bytes a serem limpos no buffer
push bufferPointer ; Endereço do buffer a ser limpo
call clear_buffer  ; Chama a função clear_buffer
```
## Parte do Section .data obrigatória
```asm
inputBuffer db 16 dup(0)                      ; Buffer utilizado no decorrer do código
overflow_msg db "Overflow detectado!", 0xA, 0 ; mensagem de overflow
countBytesBuffer1 db 16 dup(0)                ; bytes lidos, temporária
msg1 db "<Foram lidos/escritos ", 0           ; mensagem de bytes lidos
msg2 db " bytes>",0                           ; mensagem de bytes lidos
newline db 0xA                                ; Nova linha
ten dd 10                                     ; Número 10
```