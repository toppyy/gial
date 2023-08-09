IDIR = ./src/include
CC=g++
CFLAGS=-I$(IDIR)

ODIR=build
SRC=./src/


_DEPS = Parser.h NASMProgram.h Scanner.h Token.h GAST.h NASM.h Assembler.h Javascript.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o Parser.o NASMProgram.o Scanner.o Token.o GAST.o NASM.o Javascript.o
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