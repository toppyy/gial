#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl test/test_gl/expressions.gl JS > ./test/build/expressions.js
}

@test "expressions is correct" {
    run node ./test/build/expressions.js

    [[ $"${lines[0]}" == "18046" ]]
    [[ $"${lines[1]}" == "18002" ]]
    [[ $"${lines[2]}" == "1" ]]
    
}


teardown_file() {
    rm -f ./test/build/expression.js
}