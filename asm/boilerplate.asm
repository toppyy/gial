LF          equ 10          ; ASCII-code for line feed (= newline)

extern PrintInteger
extern PrintASCII

section .bss
    
section .data

section .text
global _start

_start:

    mov r8, 0
	push r8
	mov r8, 1
	pop r9
	sub r9, r8
	mov r8, r9
	push r8
	mov r8, 3
	pop r9
	add r8, r9





    mov rdi, r8
    call PrintInteger                   ; Print the count (appending it to the message)

    mov dil, LF                         ; Print new line to wrap up
    call PrintASCII                     ; Function call to print the character
Exit:
    mov       rax, 60               ; System call for exit
    xor       rdi, 0                ; Set exit code 0 for success
    syscall                         ; Make the call
