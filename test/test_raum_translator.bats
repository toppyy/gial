#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/RaumTranslator.gl > ./test/build/RaumTranslator.asm
    nasm -f elf64 test/build/RaumTranslator.asm -o test/build/RaumTranslator.o
    ld -o ./test/build/RaumTranslator.out ./test/build/RaumTranslator.o ./build/ASCII.o
    
}

@test "Raum is translated correctly" {
    
    echo "echo rauma on aina rauma | ./test/build/RaumTranslator.out" > ./test/build/RaumTranslator.sh
    chmod +x ./test/build/RaumTranslator.sh
    run ./test/build/RaumTranslator.sh

    [[ $"${lines[0]}" == "raum  o  ain  raum " ]]
}


teardown_file() {
    rm ./test/build/RaumTranslator.*
}