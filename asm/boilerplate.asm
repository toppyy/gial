;-------------------------------------------------------------------------------------
;
;   Count the number of lines (LF) in a file
;
;-------------------------------------------------------------------------------------

STDIN       equ 0           ; Code for standard input
STDOUT      equ 1           ; Code for standard output
SYS_READ    equ 0           ; System call for reading
SYS_OPEN    equ 2           ; System call for opening a file
SYS_LSEEK   equ 8           ; System call for repositioning read/write offset
O_RDONLY    equ 000000q     ; Read only -flag for reading files; zero, but base8 representation advised(?)
BUFFER_SIZE equ 100000      ; How many bytes are read from the file
LF          equ 10          ; ASCII-code for line feed (= newline)
NULL        equ 0           ; ASCII-code for NULL

%macro printBytes 3                   ; %1 = address of string to output, %2 = byte offset, %3 = number of bytes
    mov       rax, 1                  ; System call for write
    mov       rdi, 1                  ; File handle 1 is stdout
    mov       rsi, %1                 ; Address of string to output
    add       rsi, %2                 ; Byte offset
    mov       rdx, %3                 ; Number of bytes to write
    syscall                           ; Invoke operating system to do the write
%endmacro

extern PrintInteger
extern PrintASCII

section .bss
    
section .data

section .text
global _start

_start:


    mov r8, 8
    push r8
    mov r8, 2
    pop rax
    imul rax, r8
    mov r8, rax
    push r8
    mov r8, 2
    pop rax
    cdq
    idiv r8
    mov r8, rax
    push r8
    mov r8, 5
    pop r9
    add r8, r9
    push r8
    mov r8, 2
    pop r9
    sub r9, r8
    mov r8, r9



    mov rdi, r8
    call PrintInteger                   ; Print the count (appending it to the message)

    mov dil, LF                         ; Print new line to wrap up
    call PrintASCII                     ; Function call to print the character
Exit:
    mov       rax, 60               ; System call for exit
    xor       rdi, 0                ; Set exit code 0 for success
    syscall                         ; Make the call
