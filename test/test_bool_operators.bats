#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/bool_operators.gl > ./test/build/bool_operators.asm
    nasm -f elf64 test/build/bool_operators.asm -o test/build/bool_operators.o
    ld -o ./test/build/bool_operators.out ./test/build/bool_operators.o ./build/ASCII.o
    
}

@test "bool_operators is correct" {
    run ./test/build/bool_operators.out

    [[ $"${lines[0]}" == "ok0" ]]
    [[ $"${lines[1]}" == "ok1" ]]
    [[ $"${lines[2]}" == "ok2" ]]
    [[ $"${lines[3]}" == "ok3" ]]
    [[ $"${lines[4]}" == "ok4" ]]
    [[ $"${lines[5]}" == "ok5" ]]
    [[ $"${lines[6]}" == "ok6" ]]
    [[ $"${lines[7]}" == "ok7" ]]
    
}


teardown_file() {
    rm ./test/build/bool_operators.*
}