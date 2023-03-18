#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl example/terveissi.gl > ./test/build/terveissi.asm
    nasm -f elf64 test/build/terveissi.asm -o test/build/terveissi.o
    ld -o ./test/build/terveissi.out ./test/build/terveissi.o ./build/ASCII.o
    
}

@test "terveissia said on multiple lines" {
    run ./test/build/terveissi.out

    [[ $"${lines[0]}" == "Terveissi!" ]]
    [[ $"${lines[4]}" == "Terveissi!" ]]
}


teardown_file() {
    rm ./test/build/terveissi.*
}