IDIR = ./src/include
CC=g++
CFLAGS=-I$(IDIR)

ODIR=build
SRC=./src/


_DEPS = Parser.h NASMProgram.h Scanner.h Token.h GAST.h NASM.h Assembler.h Javascript.h Optimiser.h gial.h keywords.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o Parser.o NASMProgram.o Scanner.o Token.o GAST.o NASM.o Javascript.o Optimiser.o gial.o keywords.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: gl

gl: $(OBJ)
	$(CC)  $(OBJ) -o $(ODIR)/gl


$(ODIR)/%.o: $(SRC)%.cpp 
	$(CC) -g -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

tests:
	./test/bats/bin/bats ./test

memcheck:
	./test/bats/bin/bats ./test/memcheck