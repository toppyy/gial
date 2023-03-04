# giäl

**giäl** is programming language imitating the [Rauma dialect](https://en.wikipedia.org/wiki/Rauma_dialect). In rauma the term for 'language' is 'giäl' - hence the name of language. Apologies in advance to native speakers of Rauma.

The compiler translates giäl to assembly for x86. The assembler targeted is [NASM](https://www.nasm.us). It is written in C++.

The project follows Jack Crenshaws's [Let's Build a Compiler](https://compilers.iecc.com/crenshaw/) article series. Jack writes in Turbo Pascal and targets 68000, so code's been re-written, but I've stolen the ideas. This is also why it's a top-down compiler in one monolithic class responsible for everything from lexing to code generation (atm I have not reached the parts in which a separate lexer is created).

# Hello, world! / Terveissi!

The canonical [hello world! -example](https://en.wikipedia.org/wiki/%22Hello,_World!%22_program) in *giäl* is simply:

    SAN SNÄÄ 'hello world!'

To illustrate the use giäl, let's extend the example by translating "hello world" into Rauma (it's *terveissi!*) and print it five times using a while-loop: That would look something like this in pseudocode:

    i = 0
    WHILE (i < 5)
        PRINT 'Terveissi!
        PRINT
        i = i + 1
    END WHILE

The output being (of course) the string 'Terveissi!' printed out five times.

In *giäl* the same can be achieved with:

    i = 0
    GUNNES (i < 5)
        SAN SNÄÄ 'Terveissi!'
        SAN SNÄÄ
        i = i + 1    
    NYLOPPUS

Alternatively one can use a construct for repeating a block of code *N*-times:

    TOIST 5 
        SAN SNÄÄ 'Terveissi!'
        SAN SNÄÄ
    NYLOPPUS


## Syntax

In giäl, keywords are written in CAPS. The following table lists keywords of giäl and the corresponding term in "conventional programming wisdom" (whatever that means). If there's no corresponding term, an english translation is provided.

| conventional term | giäl |
| ----- |----------------- |
| TRUE  | GYL              |       
| FALSE | FALSKI           |
| BREAK | PORI             |
| PRINT | SAN SNÄÄ         |
| IF    | GUHA             |
| END   | NYLOPPUS         |  
| WHILE | GUNNES           |
| STRING | SANAMBÄTK       |
| REPEAT | TOIST           |

(If you have trouble with the encoding you can replace 'Ä' with 'A' in the keywords. This is what the compiler does anyways. So SAN SNÄÄ is equal to SAN SNAA.)


## Compiling

Assuming you have g++ and nasm at your disposal, follow these steps.

First, build the compiler under build/

    mkdir -p build
    g++ src/main.cpp src/Parser.cpp src/Program.cpp -o ./build/gl
    
Compile the program written in *giäl* and pipe the result into an .asm file. Compiling the example from above:

    ./build/gl example/terveissi.gl > ./build/terveissi.asm

Assemble and link with ASCII:

    nasm -f elf64 asm/ASCII.asm -o build/ASCII.o
    nasm -f elf64 build/terveissi.asm -o build/terveissi.o
    ld -o ./build/terveissi.out ./build/terveissi.o ./asm/ASCII.o

Run the program:

    ./build/terveissi.out

