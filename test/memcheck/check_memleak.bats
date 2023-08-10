

setup() {

    load '../test_helper/bats-support/load'
    load '../test_helper/bats-assert/load'

}

@test "Valgrind compiler arrays" {
    run valgrind -s   ./build/gl ./example/arrays.gl
    assert_output --partial 'All heap blocks were freed -- no leaks are possible'
}
@test "Valgrind compiler guessingGame" {
    run valgrind -s   ./build/gl ./example/guessingGame.gl
    assert_output --partial 'All heap blocks were freed -- no leaks are possible'
}


@test "Valgrind compiler RaumTranslator" {
    run valgrind -s   ./build/gl ./test/test_gl/RaumTranslator.gl
    assert_output --partial 'All heap blocks were freed -- no leaks are possible'
}

@test "Valgrind compiler bool_operators" {
    run valgrind -s   ./build/gl ./test/test_gl/bool_operators.gl
    assert_output --partial 'All heap blocks were freed -- no leaks are possible'
}

@test "Valgrind compiler expressions" {
    run valgrind -s   ./build/gl ./test/test_gl/expressions.gl
    assert_output --partial 'All heap blocks were freed -- no leaks are possible'
}

@test "Valgrind compiler string_comp" {
    run valgrind -s   ./build/gl ./test/test_gl/string_comp.gl
    assert_output --partial 'All heap blocks were freed -- no leaks are possible'
}

