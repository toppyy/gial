# Guessing game


A classic beginner programming exercise, the guessing game, implemented in giäl:

    SAN NY 'What year was Rauma founded?'
    SAN JOTTAI
    LUGU correctAnswer = 1442
    SAN SNÄÄ answer LUGU

    GUHA (answer != correctAnswer)
        JOS (answer < correctAnswer) 
            SAN NY 'Try a bigger number!'
        NYLOPPUS
        JOS (answer > correctAnswer) 
            SAN NY 'Try a smaller number!'
        NYLOPPUS
        SAN JOTTAI
        SAN SNÄÄ answer LUGU
    NYLOPPUS
    SAN NY 'victory!'
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
        answer resq 1
        correctAnswer resq 1
    section .data
        LBL_17 db "victory!"
        LBL_14 db "Try a smaller number!"
        LBL_10 db "Try a bigger number!"
        LBL_1 db "What year was Rauma founded?"
    section .text
    global _start
    _start:
        printBytes LBL_1, 0, 28
        mov dil, LF
        call PrintASCII
        mov r8, 1442
        mov qword[ correctAnswer], r8
        mov r8, 0
        push r8
        mov r9, 0
        LBL_2:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_3
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
        jmp LBL_2
        LBL_3:
        mov byte[answer + r9], 0
        mov qword[answer], r8
        LBL_5:
        mov r8, qword[answer]
        push r8
        mov r8, qword[correctAnswer]
        pop r9
        cmp r9, r8
        mov r8, 1
        jne LBL_6
        mov r8, 0
        LBL_6:
        cmp r8, 1
        jne LBL_4
        mov r8, qword[answer]
        push r8
        mov r8, qword[correctAnswer]
        pop r9
        cmp r9, r8
        mov r8, 1
        jl LBL_9
        mov r8, 0
        LBL_9:
        cmp r8, 1
        jne LBL_7
        printBytes LBL_10, 0, 20
        jmp LBL_8
        LBL_7:
        LBL_8:
        mov r8, qword[answer]
        push r8
        mov r8, qword[correctAnswer]
        pop r9
        cmp r9, r8
        mov r8, 1
        jg LBL_13
        mov r8, 0
        LBL_13:
        cmp r8, 1
        jne LBL_11
        printBytes LBL_14, 0, 21
        jmp LBL_12
        LBL_11:
        LBL_12:
        mov dil, LF
        call PrintASCII
        mov r8, 0
        push r8
        mov r9, 0
        LBL_15:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_16
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
        jmp LBL_15
        LBL_16:
        mov byte[answer + r9], 0
        mov qword[answer], r8
        jmp LBL_5
        LBL_4:
        printBytes LBL_17, 0, 8
        mov dil, LF
        call PrintASCII


        mov rax, 60
        xor rdi, 0
        syscall


