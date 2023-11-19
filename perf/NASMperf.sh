mkdir -p ./perf/build

# Translate, assemble and link programs to be tested

## Unoptimised translation
./build/gl perf/gl/loop.gl > ./perf/build/loop.asm
nasm -f elf64 perf/build/loop.asm -o perf/build/loop.o
ld -o ./perf/build/gial_loop.out ./perf/build/loop.o ./build/ASCII.o


## Optimised translation
./build/gl perf/gl/loop.gl -o > ./perf/build/loop_optim.asm
nasm -f elf64 perf/build/loop_optim.asm -o perf/build/loop_optim.o
ld -o ./perf/build/gial_loop_optim.out ./perf/build/loop_optim.o ./build/ASCII.o


## Build c-program(s) for benchmarking
gcc ./perf/benchmark/loop.c -o ./perf/build/c_loop.out


# Run 'em
echo
echo "GIAL-loop:"
time ./perf/build/gial_loop.out

echo
echo "GIAL-loop optimised:"
time ./perf/build/gial_loop_optim.out


echo
echo "C-loop:"
time ./perf/build/c_loop.out