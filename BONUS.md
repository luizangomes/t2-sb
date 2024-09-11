# Funções bônus do T2
## Luiza de Araújo Nunes Gomes  - 190112794

### S_INPUT utilizada no projeto
Para utilizá-la no projeto fiz algumas alterações para que possamos escrever a quantidade de bytes lidos ao utilizar o INPUT.
* **Primeiro argumento ([ebp+8]):** O ponteiro para o buffer onde os dados lidos serão armazenados.
* **Segundo argumento ([ebp+12]):** O número máximo de bytes a serem lidos.
```asm
S_INPUT:
    push ebp                ;salva o atual registrador ebp na pilha
    mov ebp, esp            ; configura o quadro de pilha para a função
    mov ecx, [ebp+8]        ; carrega o primeiro argumento, buffer de entrada
    mov edx, [ebp+12]       ; carrega o segundo argumento, tamanho do buffer de entrada
    mov eax, 3              ; eax = 3, sys_read
    mov ebx, 0              ; ebx = 0, descritor do arquivo para stdin
    int 80h                 ; Realiza a chamada de sistema "read"
    push eax                ; salva o retorno da chamada de sistema
    call bytes_read_written ; chama a bytes_read_written, que irá imprimir quantos bytes foram lidos
    mov eax, 4              ; eax = 4, sys_write
    mov ebx, 1              ; eax = 1, descritor do arquivo stdout
    mov ecx, newline        ; carrega o endereço da nova linha
    mov edx, 1              ; número de bytes em newline
    int 0x80                ; Realiza a chamada de sistema "write"
    pop eax                 ; recupera eax, para ser escrito no final como bytes lidos
    pop ebp                 ; restaura o valor anterior do reg ebp
    ret                     ; retorna a função
```

### S_OUTPUT utilizada no projeto
Para utilizá-la no projeto fiz algumas alterações para que possamos escrever a quantidade de bytes lidos ao utilizar o OUTPUT.
* **Primeiro argumento ([ebp+8]):** O ponteiro para o buffer contendo os dados a serem impressos.
* **Segundo argumento ([ebp+12]):** O número de bytes a serem escritos
```asm
S_OUTPUT:
	push ebp                ; salva o atual registrador ebp na pilha
	mov ebp, esp            ; configura o quadro de pilha para a função
	; Imprime a saída requisitada
	mov ecx, [ebp+8]        ; carrega o ponteiro para o buffer contendo os dados a serem impressos
	mov edx, [ebp+12]       ; carrega o número de bytes a serem escritos
	mov eax, 4              ; eax = 4, sys_write
	mov ebx, 1              ; eax = 1, descritor do arquivo stdout
	int 0x80                ; Realiza a chamada de sistema "write"
	mov eax, 4              ; eax = 4, sys_write
	mov ebx, 1              ; eax = 1, descritor do arquivo stdout
	mov ecx, newline        ; carrega o endereço da nova linha
	mov edx, 1              ; número de bytes em newline
	int 0x80                ; Realiza a chamada de sistema "write"
	push eax                ; salva o retorno da chamada de sistema
	call bytes_read_written ; chama a bytes_read_written, que irá imprimir quantos bytes foram lidos
	pop eax                 ; recupera eax, para ser escrito no final como bytes lidos
	mov eax, 4              ; eax = 4, sys_write
	mov ebx, 1              ; eax = 1, descritor do arquivo stdout
	mov ecx, newline        ; carrega o endereço da nova linha
	mov edx, 1              ; número de bytes em newline
	int 0x80                ; Realiza a chamada de sistema "write"
	pop ebp                 ; restaura o valor anterior do reg ebp
	ret                     ; retorna a função
```