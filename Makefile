CC=g++
SRC=./src/
ODIR=build

__OBJ = $(wildcard -r src/*.cpp) $(wildcard -r src/**/*.cpp) $(wildcard -r src/**/**/*.cpp)
_OBJ = $(patsubst %.cpp, %.o, $(__OBJ))
OBJ = $(patsubst src/%, $(ODIR)/%, $(_OBJ))


# Define a target to compile all source files
all: ascii gl

gl: $(OBJ)
	$(CC)  $(OBJ) -o $(ODIR)/gl

$(ODIR)/%.o: $(SRC)%.cpp 
	$(CC) -g -c $< -o $@  $(CFLAGS)

# ASCII.o is needed for printing in gial
ascii:
	nasm -f elf64 asm/ASCII.asm -o build/ASCII.o


# Clean up
clean:
	rm -f $(ODIR)/*.o

# Run tests
tests:
	./test/bats/bin/bats ./test

# Make memcheck
memcheck:
	./test/bats/bin/bats ./test/memcheck