# giäl

**giäl** is a BASIC-inspired programming language imitating the [Rauma dialect](https://en.wikipedia.org/wiki/Rauma_dialect). In rauma the term for 'language' is 'giäl' - hence the name of language. Apologies in advance to native speakers of Rauma.

The project is actually a translator: it translates giäl to assembly for x86. The assembler targeted is [NASM](https://www.nasm.us). It is written in C++.

The project started with Jack Crenshaws's [Let's Build a Compiler](https://compilers.iecc.com/crenshaw/) article series. Jack writes in Turbo Pascal and targets 68000, so code's been re-written, but I've stolen some of the ideas. Unlike Jack's compiler, giäl represents the parsed program as a tree-like structure before translation.

# Online compiler

You can try gial using the online compiler [here](https://toppyy.github.io/gial/). The online compiler is possible with WASM (see [`./wasm/`](./wasm/)).


# Hello, world! / Terveissi!

The canonical [hello world! -example](https://en.wikipedia.org/wiki/%22Hello,_World!%22_program) in *giäl* is simply:

    SAN NY 'hello world!'

To illustrate the use giäl, let's extend the example by translating "hello world" into Rauma (it's *terveissi!*) and print it five times using a while-loop: That would look something like this in pseudocode:

    i = 0
    WHILE (i < 5)
        PRINT 'Terveissi!
        PRINT
        i = i + 1
    END WHILE

The output being (of course) the string 'Terveissi!' printed out five times.

In *giäl* the same can be achieved with:

    LUGU i = 0
    GUHA (i < 5)
        SAN NY 'Terveissi!'
        SIKSES
        i = i + 1    
    NYLOPPUS

Alternatively one can use a construct for repeating a block of code *N*-times:

    TOIST 5 
        SAN NY 'Terveissi!'
        SIKSES
    NYLOPPUS


## Features

Giäl can do:

- Integer arithmetic with operators +, -, * and /
- Complex expressions, eg. ((1*3) + 2)/5
- Boolean expressions with operators ==, !=, <, >, <= and >=, eg. ( ((1==1) AND 1 > 5) OR 8 < 9)
- Control structures like IF, IF ELSE, WHILE
- String indexing (see examples, `RaumTranslator.gl`)
- Arrays (see examples `arrays.gl`)
- I/O, atm. only through stdio and stdout

## Syntax

In giäl, keywords are written in CAPS. The following table lists keywords of giäl and the corresponding term in "conventional programming wisdom" (whatever that means). If there's no corresponding term, an english translation is provided.

| conventional term/expression | giäl |
| ----- |----------------- |
| PRINT \n | SIKSES        |
| PRINT | SAN NY           |
| INPUT | SAN SNÄÄ         |
| AND   | JA               |
| OR    | TAI              |
| IF    | JOS              |
| END   | NYLOPPUS         |  
| WHILE | GUHA             |
| STRING | SANAMBÄTK       |
| INTEGER | LUGU           |
| REPEAT | TOIST           |
| FOR x = 0 TO 10 STEP 1 | TOST x = 0 TOHO 10 HYBYL 1 |

(If you have trouble with the encoding you can replace 'Ä' with 'A' in the keywords. This is what the compiler does anyways. So SAN SNÄÄ is equal to SAN SNAA.)


## Compiling

Assuming you have g++ and nasm at your disposal, follow these steps.

First, create a folder 'build' and run make:

    mkdir -p build && make

Next, translate the example program written in *giäl* and pipe the result into an .asm file:

    ./build/gl example/terveissi.gl > ./build/terveissi.asm

Assemble and link with ASCII:

    nasm -f elf64 asm/ASCII.asm -o build/ASCII.o
    nasm -f elf64 build/terveissi.asm -o build/terveissi.o
    ld -o ./build/terveissi.out ./build/terveissi.o ./build/ASCII.o

And you're done. Run the program:

    ./build/terveissi.out

## Testing

Testing is implemented with [BATS](https://github.com/bats-core/bats-core) (included as a submodule). 

Get submodule

    git submodule update --init --recursive

To run all tests:

    make tests

To test for memory leaks, use:

    make memcheck
