#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl example/guessingGame.gl -a JS > ./test/build/guessingGame.js
    
}

@test "correct answer returns victory" {
    echo "echo 1442 | node test/build/guessingGame.js" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    run ./test/build/guessingGame.sh

    [[ $"${lines[0]}" == "What year was Rauma founded?" ]]
    [[ $"${lines[1]}" == "victory!" ]]
}


@test "too small guess means guidance to give a larger number" {
    
    echo "echo 1142 | node test/build/guessingGame.js" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    run ./test/build/guessingGame.sh
    [[ $"${lines[1]}" == "Try a bigger number!" ]]
}


@test "too large guess means guidance to give a smaller number" {
    
    echo "echo 4123 | node test/build/guessingGame.js" > ./test/build/guessingGame.sh
    chmod +x ./test/build/guessingGame.sh
    run ./test/build/guessingGame.sh

    [[ $"${lines[1]}" == "Try a smaller number!" ]]
}



teardown_file() {
   rm -f ./test/build/guessingGame.*
}