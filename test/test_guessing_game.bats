#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl example/guessingGame.gl > ./test/build/guessingGame.asm
    nasm -f elf64 test/build/guessingGame.asm -o test/build/guessingGame.o
    ld -o ./test/build/guessingGame.out ./test/build/guessingGame.o ./build/ASCII.o
    echo "echo 1442 | ./test/build/guessingGame.out" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    
}

@test "correct answer returns victory" {
    
    echo "echo 1442 | ./test/build/guessingGame.out" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    run ./test/build/guessingGame.sh

    [[ $"${lines[0]}" == "What year was Rauma founded?" ]]
    [[ $"${lines[1]}" == "victory!" ]]
}


@test "too small guess means guidance to give a larger number" {
    
    echo "echo 1142 | ./test/build/guessingGame.out" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    run ./test/build/guessingGame.sh
    [[ $"${lines[1]}" == "Try a bigger number!" ]]
}


@test "too large guess means guidance to give a smaller number" {
    
    echo "echo 4123 | ./test/build/guessingGame.out" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    run ./test/build/guessingGame.sh

    [[ $"${lines[1]}" == "Try a smaller number!" ]]
}


teardown_file() {
   rm ./test/build/guessingGame.*
}