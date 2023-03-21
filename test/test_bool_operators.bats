#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/bool_operators.gl > ./test/build/bool_operators.asm
    nasm -f elf64 test/build/bool_operators.asm -o test/build/bool_operators.o
    ld -o ./test/build/bool_operators.out ./test/build/bool_operators.o ./build/ASCII.o
    
}

@test "bool_operators is correct" {
    run ./test/build/bool_operators.out

    [[ $"${lines[0]}" == "ok" ]]
    [[ $"${lines[1]}" == "ok" ]]
    [[ $"${lines[2]}" == "ok" ]]
    [[ $"${lines[3]}" == "ok" ]]
    [[ $"${lines[4]}" == "ok" ]]
    [[ $"${lines[5]}" == "ok" ]]
    [[ $"${lines[6]}" == "ok" ]]
    [[ $"${lines[7]}" == "ok" ]]
    
}


#teardown_file() {
#    rm ./test/build/bool_operators.*
#}