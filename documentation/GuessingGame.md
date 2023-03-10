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
    extern PrintInteger
    extern PrintASCII
    section .bss
        answer resq 8
        correctAnswer resq 8
    section .text
    global _start
    _start:
        mov rdi, 'W'
        call PrintASCII
        mov rdi, 'h'
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, 't'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'y'
        call PrintASCII
        mov rdi, 'e'
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'w'
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, 's'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'R'
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, 'u'
        call PrintASCII
        mov rdi, 'm'
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'f'
        call PrintASCII
        mov rdi, 'o'
        call PrintASCII
        mov rdi, 'u'
        call PrintASCII
        mov rdi, 'n'
        call PrintASCII
        mov rdi, 'd'
        call PrintASCII
        mov rdi, 'e'
        call PrintASCII
        mov rdi, 'd'
        call PrintASCII
        mov rdi, '?'
        call PrintASCII
        mov dil, LF
        call PrintASCII
        mov r8, 1442
        ; assignig value!
        mov qword[correctAnswer], r8
        mov r8, qword[correctAnswer]
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
        je LBL_4
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
        LBL_4:
        mov byte[answer + r9], 0
        mov qword[answer], r8
        LBL_8:
        mov r8, qword[answer]
        push r8
        mov r8, 1442
        pop r9
        cmp r9, r8
        mov r8, 1
        jne LBL_10
        mov r8, 0
        LBL_10:
        cmp r8, 1
        jne LBL_6
        mov r8, qword[answer]
        push r8
        mov r8, 1442
        pop r9
        cmp r9, r8
        mov r8, 1
        jl LBL_14
        mov r8, 0
        LBL_14:
        cmp r8, 1
        jne LBL_12
        mov rdi, 'T'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, 'y'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'b'
        call PrintASCII
        mov rdi, 'i'
        call PrintASCII
        mov rdi, 'g'
        call PrintASCII
        mov rdi, 'g'
        call PrintASCII
        mov rdi, 'e'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'n'
        call PrintASCII
        mov rdi, 'u'
        call PrintASCII
        mov rdi, 'm'
        call PrintASCII
        mov rdi, 'b'
        call PrintASCII
        mov rdi, 'e'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, '!'
        call PrintASCII
        LBL_12:
        mov r8, qword[answer]
        push r8
        mov r8, 1442
        pop r9
        cmp r9, r8
        mov r8, 1
        jg LBL_18
        mov r8, 0
        LBL_18:
        cmp r8, 1
        jne LBL_16
        mov rdi, 'T'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, 'y'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 's'
        call PrintASCII
        mov rdi, 'm'
        call PrintASCII
        mov rdi, 'a'
        call PrintASCII
        mov rdi, 'l'
        call PrintASCII
        mov rdi, 'l'
        call PrintASCII
        mov rdi, 'e'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, ' '
        call PrintASCII
        mov rdi, 'n'
        call PrintASCII
        mov rdi, 'u'
        call PrintASCII
        mov rdi, 'm'
        call PrintASCII
        mov rdi, 'b'
        call PrintASCII
        mov rdi, 'e'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, '!'
        call PrintASCII
        LBL_16:
        mov dil, LF
        call PrintASCII
        mov r8, 0
        push r8
        mov r9, 0
        LBL_20:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_22
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
        jmp LBL_20
        LBL_22:
        mov byte[answer + r9], 0
        mov qword[answer], r8
        jmp LBL_8
        LBL_6:
        mov rdi, 'v'
        call PrintASCII
        mov rdi, 'i'
        call PrintASCII
        mov rdi, 'c'
        call PrintASCII
        mov rdi, 't'
        call PrintASCII
        mov rdi, 'o'
        call PrintASCII
        mov rdi, 'r'
        call PrintASCII
        mov rdi, 'y'
        call PrintASCII
        mov rdi, '!'
        call PrintASCII
        mov dil, LF
        call PrintASCII


        mov rax, 60
        xor rdi, 0
        syscall


