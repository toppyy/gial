#!/usr/bin/env bats

setup_file() {

    mkdir -p ./test/build
    ./build/gl example/arrays.gl JS > ./test/build/arrays.js    
}

@test "assigning an iterating works on arrays" {
    run node ./test/build/arrays.js

    [[ $"${lines[0]}" == "0 + 1000 + 400 + 42 = 1442" ]]
}


teardown_file() {
    rm -f ./test/build/arrayz.*
}