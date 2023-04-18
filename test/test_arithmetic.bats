#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/arithmetic.gl > ./test/build/arithmetic.asm
    nasm -f elf64 test/build/arithmetic.asm -o test/build/arithmetic.o
    ld -o ./test/build/arithmetic.out ./test/build/arithmetic.o ./build/ASCII.o
    
}

@test "arithmetic is correct" {
    run ./test/build/arithmetic.out

    [[ $"${lines[0]}" == "9023" ]]
    [[ $"${lines[1]}" == "8977" ]]
    [[ $"${lines[2]}" == "207000" ]]
    [[ $"${lines[3]}" == "391" ]]
    
}


teardown_file() {
    rm ./test/build/arithmetic.*
}