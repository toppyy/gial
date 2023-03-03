LF          equ 10          ; ASCII-code for line feed (= newline)

extern PrintInteger
extern PrintASCII

section .bss
    
section .data

section .text
global _start

_start:

	mov r8, 1
	mov r9, 1

	;// Are r8 and r9 the same?
	cmp r8, r9
	mov r8, 0
	jne false
	mov r8, 1
false:
	push r8 ;// Store result on stack

	mov r8, 3
	mov r9, 3

	;// Is r8 greater than r9?
	cmp r8, r9
	mov r8, 0
	jne false2
	mov r8, 1
false2:
	push r8

	pop r9
	pop r8

    ; mov rdi, r8
    ; call PrintInteger                   ; Print the count (appending it to the message)
    ; mov dil, LF                         ; Print new line to wrap up
    ; call PrintASCII                     ; Function call to print the character

    ; mov rdi, r9
    ; call PrintInteger                   ; Print the count (appending it to the message)
    ; mov dil, LF                         ; Print new line to wrap up
    ; call PrintASCII                     ; Function call to print the character


	and r8, r9

    mov rdi, r8
    call PrintInteger                   ; Print the count (appending it to the message)

    mov dil, LF                         ; Print new line to wrap up
    call PrintASCII                     ; Function call to print the character
Exit:
    mov       rax, 60               ; System call for exit
    xor       rdi, 0                ; Set exit code 0 for success
    syscall                         ; Make the call
