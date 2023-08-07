#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/arithmetic.gl JS > ./test/build/arithmetic.js
}

@test "arithmetic is correct" {
    run node ./test/build/arithmetic.js

    [[ $"${lines[0]}" == "9023" ]]
    [[ $"${lines[1]}" == "8977" ]]
    [[ $"${lines[2]}" == "207000" ]]
    [[ $"${lines[3]}" == "391.30434782608694" ]]
    
}


teardown_file() {
    rm -f ./test/build/arithmetic.js
}