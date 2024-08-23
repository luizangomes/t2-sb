section .bss
    input_buffer resb 12        ; Reserve 12 bytes for the input string (including null terminator)

GetInt:
    ; Read the input string from the console
    mov eax, 3                  ; syscall: sys_read
    mov ebx, 0                  ; file descriptor 0 (STDIN)
    mov ecx, input_buffer       ; buffer to store input
    mov edx, 11                 ; number of bytes to read (10 digits + newline)
    int 0x80                    ; call kernel

    ; Convert the string to an integer
    mov esi, input_buffer       ; ESI points to the input string
    xor eax, eax                ; Clear EAX to store the result
    xor ebx, ebx                ; EBX will be used as the sign (0 = positive, 1 = negative)

    ; Check for a negative sign
    cmp byte [esi], '-'
    jne .convert                ; If not a negative sign, skip to conversion
    inc esi                     ; Move to the next character
    mov ebx, 1                  ; Set the sign to negative

.convert:
    xor ecx, ecx                ; Clear ECX to use as a digit holder

.convert_loop:
    mov cl, [esi]               ; Load the next character
    cmp cl, 10                  ; Check if it's a newline (end of input)
    je .done                    ; If newline, conversion is done
    sub cl, '0'                 ; Convert ASCII character to numeric value
    imul eax, eax, 10           ; Multiply EAX by 10 (shift left)
    add eax, ecx                ; Add the digit to EAX
    inc esi                     ; Move to the next character
    jmp .convert_loop           ; Repeat the loop

.done:
    cmp ebx, 0                  ; Check if the number was negative
    je .exit_getint             ; If positive, skip negation
    neg eax                     ; If negative, negate the result

.exit_getint:
    ret                         ; Return with result in EAX