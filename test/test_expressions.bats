#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/expressions.gl > ./test/build/expressions.asm
    nasm -f elf64 test/build/expressions.asm -o test/build/expressions.o
    ld -o ./test/build/expressions.out ./test/build/expressions.o ./build/ASCII.o
    
}

@test "expressions is correct" {
    run ./test/build/expressions.out

    [[ $"${lines[0]}" == "18046" ]]
    [[ $"${lines[1]}" == "18002" ]]
    [[ $"${lines[2]}" == "1" ]]
    
}


teardown_file() {
    rm ./test/build/expressions.*
}