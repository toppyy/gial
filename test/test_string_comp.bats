#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/string_comp.gl > ./test/build/string_comp.asm
    nasm -f elf64 test/build/string_comp.asm -o test/build/string_comp.o
    ld -o ./test/build/string_comp.out ./test/build/string_comp.o ./build/ASCII.o
    
}

@test "string_comp is correct" {
    run ./test/build/string_comp.out

    echo ${lines[2]}    

    [[ $"${lines[0]}" == "ok" ]]
    [[ $"${lines[1]}" == "ok" ]]
    [[ $"${lines[2]}" == "ok" ]]
    [[ $"${lines[3]}" == "ok" ]]
}


teardown_file() {
    rm ./test/build/string_comp.*
}