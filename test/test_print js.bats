#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/print.gl -a JS > ./test/build/print.js    
}

@test "print is correct" {
    run node ./test/build/print.js

    [[ $"${lines[0]}" == "str constant" ]]
    [[ $"${lines[1]}" == "terveissi" ]]
    [[ $"${lines[2]}" == "9876" ]]
    [[ $"${lines[3]}" == "123456" ]]
    
}


teardown_file() {
    rm -f ./test/build/print.js
}