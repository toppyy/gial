# Guessing game


A classic beginner programming exercise, the guessing game, implemented in giäl:

    SAN NY 'What year was Rauma founded?'
    SAN JOTTAI
    SAN SNÄÄ answer LUGU
    correctAnswer = 1442

    GUNNES (answer != correctAnswer)
        GUHA (answer < correctAnswer) 
            SAN NY 'Try a bigger number!'
        NYLOPPUS
        GUHA (answer > correctAnswer) 
            SAN NY 'Try a smaller number!'
        NYLOPPUS
        SAN JOTTAI
        SAN SNÄÄ answer LUGU
    NYLOPPUS
    SAN NY 'victory!'
    SAN JOTTAI
    SAN JOTTAI



Translates to this lengthy piece of assembly:

    STDIN       equ 0
    STDOUT      equ 1
    SYS_READ    equ 0
    LF equ 10
    %macro printBytes 3               ; %1 = address of string to output, %2 = byte offset, %3 = number of bytes
        mov       rax, 1                  ; system call for write
        mov       rdi, 1                  ; file handle 1 is stdout
        mov       rsi, %1                 ; address of string to output
        add       rsi, %2                 ; byte offset
        mov       rdx, %3                 ; number of bytes
        syscall                           ; invoke operating system to do the write
    %endmacro
    extern PrintInteger
    extern PrintASCII
    section .bss
        correctAnswer resq 8
        answer resq 8
    section .data
        LBL_30 db "victory!"
        LBL_24 db "Try a smaller number!"
        LBL_18 db "Try a bigger number!"
        LBL_2 db "What year was Rauma founded?"
    section .text
    global _start
    _start:
        printBytes LBL_2, 0, 28
        mov dil, LF
        call PrintASCII
        mov r8, 0
        push r8
        mov r9, 0
        LBL_4:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_6
        pop r8
        mov rax, r8
        mov r11, 10
        mul r11
        mov r8, rax
        mov r12, 0
        mov r12b, byte[answer + r9]
        sub r12b, 48
        add r8, r12
        push r8
        inc r9
        jmp LBL_4
        LBL_6:
        mov byte[answer + r9], 0
        mov qword[answer], r8
        mov r8, 1442
        mov qword[correctAnswer], r8
        mov r8, qword[correctAnswer]
        LBL_10:
        mov r8, qword[answer]
        push r8
        mov r8, qword[correctAnswer]
        pop r9
        cmp r9, r8
        mov r8, 1
        jne LBL_12
        mov r8, 0
        LBL_12:
        cmp r8, 1
        jne LBL_8
        mov r8, qword[answer]
        push r8
        mov r8, qword[correctAnswer]
        pop r9
        cmp r9, r8
        mov r8, 1
        jl LBL_16
        mov r8, 0
        LBL_16:
        cmp r8, 1
        jne LBL_14
        printBytes LBL_18, 0, 20
        LBL_14:
        mov r8, qword[answer]
        push r8
        mov r8, qword[correctAnswer]
        pop r9
        cmp r9, r8
        mov r8, 1
        jg LBL_22
        mov r8, 0
        LBL_22:
        cmp r8, 1
        jne LBL_20
        printBytes LBL_24, 0, 21
        LBL_20:
        mov dil, LF
        call PrintASCII
        mov r8, 0
        push r8
        mov r9, 0
        LBL_26:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_28
        pop r8
        mov rax, r8
        mov r11, 10
        mul r11
        mov r8, rax
        mov r12, 0
        mov r12b, byte[answer + r9]
        sub r12b, 48
        add r8, r12
        push r8
        inc r9
        jmp LBL_26
        LBL_28:
        mov byte[answer + r9], 0
        mov qword[answer], r8
        jmp LBL_10
        LBL_8:
        printBytes LBL_30, 0, 8
        mov dil, LF
        call PrintASCII


        mov rax, 60
        xor rdi, 0
        syscall




