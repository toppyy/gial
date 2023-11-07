emcc -O2 ./src/Parser.cpp -c -o wasm/build/Parser.o
emcc -O2 ./src/NASMProgram.cpp -c -o wasm/build/NASMProgram.o
emcc -O2 ./src/Scanner.cpp -c -o wasm/build/Scanner.o
emcc -O2 ./src/Token.cpp -c -o wasm/build/Token.o
emcc -O2 ./src/GAST.cpp -c -o wasm/build/GAST.o
emcc -O2 ./src/NASM.cpp -c -o wasm/build/NASM.o
emcc -O2 ./src/Javascript.cpp -c -o wasm/build/Javascript.o
emcc -O2 ./src/Optimiser.cpp -c -o wasm/build/Optimiser.o
emcc -O2 ./src/gial.cpp -c -o wasm/build/gial.o 
emcc -O2 ./src/keywords.cpp -c -o wasm/build/keywords.o

emcc -O2 ./wasm/gialwasm.cpp -c -o ./wasm/build/gialwasm.o 

emcc ./wasm/build/gialwasm.o ./wasm/build/Parser.o ./wasm/build/NASMProgram.o ./wasm/build/Scanner.o ./wasm/build/Token.o ./wasm/build/GAST.o ./wasm/build/NASM.o ./wasm/build/Javascript.o ./wasm/build/Optimiser.o ./wasm/build/gial.o ./wasm/build/keywords.o -o wasm/build/gial.js -s "EXPORTED_FUNCTIONS=_compile_to_gial" -s "EXPORTED_RUNTIME_METHODS=ccall,cwrap" -sNO_DISABLE_EXCEPTION_CATCHING