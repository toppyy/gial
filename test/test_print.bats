#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/print.gl > ./test/build/print.asm
    nasm -f elf64 test/build/print.asm -o test/build/print.o
    ld -o ./test/build/print.out ./test/build/print.o ./build/ASCII.o
    
}

@test "print is correct" {
    run ./test/build/print.out

    [[ $"${lines[0]}" == "str constant" ]]
    [[ $"${lines[1]}" == "terveissi" ]]
    [[ $"${lines[2]}" == "9876" ]]
    [[ $"${lines[3]}" == "123456" ]]
    
}


teardown_file() {
    rm ./test/build/print.*
}