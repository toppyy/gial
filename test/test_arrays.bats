#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl example/arrays.gl > ./test/build/arrays.asm
    nasm -f elf64 test/build/arrays.asm -o test/build/arrays.o
    ld -o ./test/build/arrays.out ./test/build/arrays.o ./build/ASCII.o
    
}

@test "assigning an iterating works on arrays" {
    run ./test/build/arrays.out

    [[ $"${lines[0]}" == "1442" ]]
}


teardown_file() {
    rm ./test/build/arrays.*
}