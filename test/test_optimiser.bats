#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/optimiser_loop.gl -o > ./test/build/optimiser_loop.asm
    nasm -f elf64 test/build/optimiser_loop.asm -o test/build/optimiser_loop.o
    ld -o ./test/build/optimiser_loop.out ./test/build/optimiser_loop.o ./build/ASCII.o
    
    ./build/gl test/test_gl/optimiser_loop.gl -a JS -o > ./test/build/optimiser_loop.js

}

@test "ASM: optimised loop result correct" {
    run ./test/build/optimiser_loop.out

    [[ $"${lines[0]}" == "54745088" ]]
}

@test "JS: optimised loop result correct" {
    run node ./test/build/optimiser_loop.js

    [[ $"${lines[0]}" == "54745088" ]]
}


teardown_file() {
    rm ./test/build/optimiser_loop.*
}