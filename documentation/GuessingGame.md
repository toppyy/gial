# Guessing game


A classic beginner programming exercise, the guessing game, implemented in giäl:

    SAN NY 'What year was Rauma founded?'
    SAN JOTTAI
    SANAMBÄTK correctAnswer = '1442'
    SAN SNÄÄ answer

    GUHA (answer != correctAnswer)
        JOS (answer < correctAnswer) 
            SAN NY 'Try a bigger number!'
        NYLOPPUS
        JOS (answer > correctAnswer) 
            SAN NY 'Try a smaller number!'
        NYLOPPUS
        SAN JOTTAI
        SAN SNÄÄ answer
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
        answer resb 100
        correctAnswer resb 5
    section .data
        LBL_26 db "Try a smaller number!"
        LBL_18 db "Try a bigger number!"
        LBL_29 db "victory!"
        LBL_1 db "What year was Rauma founded?"
    section .text
    global _start
    _start:
        printBytes LBL_1, 0, 28
        mov dil, LF
        call PrintASCII
        mov byte[correctAnswer + 0], 49
        mov byte[correctAnswer + 1], 52
        mov byte[correctAnswer + 2], 52
        mov byte[correctAnswer + 3], 50
        mov byte[correctAnswer + 4], 0
        mov r9, 0
        LBL_2:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_3
        inc r9
        jmp LBL_2
        LBL_3:
        mov byte[answer + r9], 0
        LBL_5:
        mov r12, 0
        LBL_7:
        mov r8b, byte[answer + r12]
        mov r9b, byte[correctAnswer + r12]
        cmp r8b, r9b
        jne LBL_9
        mov r8b, byte[answer + r12]
        cmp r8b, 0
        je LBL_10
        inc r12
        jmp LBL_7
        LBL_9:
        mov r8, 1
        jmp LBL_8
        LBL_10:
        mov r8, 0
        LBL_8:
        cmp r8, 1
        jne LBL_4
        mov r12, 0
        LBL_14:
        mov r8b, byte[answer + r12]
        mov r9b, byte[correctAnswer + r12]
        cmp r8b, r9b
        jl LBL_17
        mov r8b, byte[answer + r12]
        cmp r8b, 0
        je LBL_16
        inc r12
        jmp LBL_14
        LBL_16:
        mov r8, 0
        jmp LBL_15
        LBL_17:
        mov r8, 1
        LBL_15:
        cmp r8, 1
        jne LBL_11
        printBytes LBL_18, 0, 20
        jmp LBL_12
        LBL_11:
        LBL_12:
        mov r12, 0
        LBL_22:
        mov r8b, byte[answer + r12]
        mov r9b, byte[correctAnswer + r12]
        cmp r8b, r9b
        jl LBL_25
        mov r8b, byte[answer + r12]
        cmp r8b, 0
        je LBL_24
        inc r12
        jmp LBL_22
        LBL_24:
        mov r8, 1
        jmp LBL_23
        LBL_25:
        mov r8, 0
        LBL_23:
        cmp r8, 1
        jne LBL_19
        printBytes LBL_26, 0, 21
        jmp LBL_20
        LBL_19:
        LBL_20:
        mov dil, LF
        call PrintASCII
        mov r9, 0
        LBL_27:
        mov rax, SYS_READ
        mov rdi, STDIN
        lea rsi, byte[answer + r9]
        mov rdx, 1
        syscall
        cmp byte[answer + r9], LF
        je LBL_28
        inc r9
        jmp LBL_27
        LBL_28:
        mov byte[answer + r9], 0
        
        jmp LBL_5
        LBL_4:
        printBytes LBL_29, 0, 8
        mov dil, LF
        call PrintASCII


        mov rax, 60
        xor rdi, 0
        syscall


