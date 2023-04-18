IDIR = ./src/include
CC=g++
CFLAGS=-I$(IDIR)

ODIR=build
SRC=./src/


_DEPS = Parser.h Program.h Scanner.h Token.h TreeParser.h GAST.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o Parser.o Program.o Scanner.o Token.o TreeParser.o GAST.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: gl

gl: $(OBJ)
	$(CC) $(OBJ) -o $(ODIR)/gl


$(ODIR)/%.o: $(SRC)%.cpp 
	$(CC) -c -o $@ $< $(CFLAGS)


.PHONY: clean

clean:
	rm -f $(ODIR)/*.o

tests:
	./test/bats/bin/bats ./test