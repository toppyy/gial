#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/readfile.gl > ./test/build/readfile.asm
    nasm -f elf64 test/build/readfile.asm -o test/build/readfile.o
    ld -o ./test/build/readfile.out ./test/build/readfile.o ./build/ASCII.o
    
}

@test "readfile is correct" {
    run ./test/build/readfile.out

     [[ $"${lines[0]}" == "raum > pori" ]]
    
    
}


