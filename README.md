# giäl

**giäl** is programming language imitating the [Rauma dialect](https://en.wikipedia.org/wiki/Rauma_dialect). In rauma the term for 'language' is 'giäl' - hence the name of language. Apologies in advance to native speakers of Rauma.

The compiler translates giäl to assembly for x86. The assembler targeted is [NASM](https://www.nasm.us). It is written in C++.

The project follows Jack Crenshaws's [Let's Build a Compiler](https://compilers.iecc.com/crenshaw/) article series. Jack writes in Turbo Pascal and targets 68000, so code's been re-written, but ideas remain. This is also why it's a top-down compiler in one monolithic class responsible for everything from lexing to code generation (atm I have not reached the parts in which a separate lexer is created).

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
| RETURN | TOIST           |

