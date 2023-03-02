;------------------------------------------------------------------------------
;
;   Utility functions to help with conversion to ASCII and printing to console
;
;   "PrintInteger":
;       Converts an integer (parameter through edi) into a set of ASCII-codes and prints said codes to STDOUT   
;
;   "PrintASCII":
;       Prints the character represented by the ASCII-code given as a parameter (through dil)
;
;-------------------------------------------------------------------------------

%macro printBytes 3                   ; %1 = address of string to output, %2 = byte offset, %3 = number of bytes
    mov       rax, 1                  ; system call for write
    mov       rdi, 1                  ; file handle 1 is stdout
    mov       rsi, %1                 ; address of string to output
    add       rsi, %2                 ; byte offset
    mov       rdx, %3                 ; number of bytes
    syscall                           ; invoke operating system to do the write
%endmacro

%macro divideInt 2
    mov eax, %1                 ; %1 = dividend
    cdq                         ; convert dword in eax to qword in edx:eax
    idiv %2                     ; %2 = divisor
    mov dword[remainder], edx;  ; store remainder
    mov dword[quotient],  eax;  ; store quotient
%endmacro

section     .data
    ASCIIcode: db "?"   ; Store ASCII-code to print
    remainder dd 0      ; Used to store the remainder of divisions 
    quotient dd 0       ; Used to store the quotient of divisions
    ten: dd 10          ; Constant for value 10
    negative: db 0      ; Flag used when printing negative integers
    
section     .bss
    numberCodes resb 10     ; Store ASCII-codes to print

section   .text
global PrintInteger 
; Function to convert an integer to a set ASCII-codes and print said ASCII-codes
; The algorithm repeatedly divides the int by 10 and stores the remainders in array
; The array (or a subset of it) is printed after there's nothing to divide
; Example: integer 25 is represented by ASCII-codes [50, 53]
PrintInteger:
    mov r9, 9                           ; to track where to store the current ASCII-code
    mov byte[negative], 1               ; a flag for negative values (so when can correctly calculate ASCII-code)
    divideInt edi, dword[ten]           ; divide by 10

    cmp edi, 0
    jg  CollectRemainders               ; if non-negative skip flagging step
    mov dil, 45                         ; pass ASCII-code for minus sign
    call PrintASCII                     ; print minus sign before the number
    mov byte[negative], -1              ; flag as negative

CollectRemainders:
    ; Check if the remainder or quotient is not zero. If not proceed to PrintRemainder-routine.
    ; If so, print the codes in the numberCodes-array
    cmp dword[remainder], 0     ; Is the remainder non-zero?
    jne StoreRemainder
    cmp dword[quotient], 0      ; Is the quotient non-zero?
    jne StoreRemainder
    jmp printASCIIcodes         ; Move on to printing the collected remainders

StoreRemainder:
    ; Store the remainder in the array numberCodes
    mov r8b, 0
    ; Use the negative flag to flip sign of negative values
    ; multiply the remainder by 1 or -1
    mov al , byte[remainder]
    mov bl , byte[negative]        
    mul bl    
    mov r8b, al
    add r8b, 48                         ; Add 48 as it is the code for zero
    mov byte[numberCodes + r9], r8b     ; Store result into array    
    dec r9                              ; Update counter for ASCII-codes in array

    ; Next iteration of algorithm: divide by ten
    divideInt dword[quotient], dword[ten]       ;  Divide by ten
    jmp CollectRemainders                       ;  Return to collecting remainders

printASCIIcodes:
    ; calculate the number of bytes to print (= number of ASCII-codes)
    mov r10, 9
    sub r10, r9
    ; offset
    add r9, 1

    printBytes numberCodes, r9, r10
    ret

global PrintASCII
PrintASCII:
    mov byte[ASCIIcode],  dil    ; Store parameter into memory
    printBytes ASCIIcode, 0, 1  ; Use macro to print the byte
    ret