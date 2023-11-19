mkdir -p ./perf/build

# Translate, assemble and link programs to be tested

## Unoptimised translation
./build/gl perf/gl/loop.gl -a JS > ./perf/build/loop.js

## Optimised translation
./build/gl perf/gl/loop.gl -a JS -o > ./perf/build/loop_optim.js


## Build c-program(s) for benchmarking
gcc ./perf/benchmark/loop.c -o ./perf/build/c_loop.out


# Run 'em
echo
echo "GIAL/JS-loop:"
time node ./perf/build/loop.js

echo
echo "GIAL/JS-loop optimised:"
time node ./perf/build/loop_optim.js

echo
echo "C-loop:"
time ./perf/build/c_loop.out