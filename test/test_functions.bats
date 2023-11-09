#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/functions.gl > ./test/build/functions.asm
    nasm -f elf64 test/build/functions.asm -o test/build/functions.o
    ld -o ./test/build/functions.out ./test/build/functions.o ./build/ASCII.o
    
    ./build/gl test/test_gl/functions.gl -a JS > ./test/build/functions.js

}

@test "ASM: functions is correct" {
    run ./test/build/functions.out

    [[ $"${lines[0]}" == "2 ^ 6 = 64" ]]
}

@test "JS: functions is correct" {
    run node ./test/build/functions.js

    [[ $"${lines[0]}" == "2 ^ 6 = 64" ]]
}




teardown_file() {
    rm ./test/build/functions.*
}