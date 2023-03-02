
nasm -f elf64 ASCII.asm -o ASCII.o
nasm -f elf64 boilerplate.asm -o boilerplate.o
ld -o boilerplate.out ASCII.o boilerplate.o
./boilerplate.out