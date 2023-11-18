#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include "./GAST.h"
#include "./Parser.h"
#include "./compilers/NASM.h"
#include "./compilers/Javascript.h"
#include "./Assembler.h"
#include "./Optimiser.h"
#include "./Scanner.h"
#include "./keywords.h"

using std::vector, std::string;

string compile(char* p_assembler, string content, int p_optimise);
char handleScandics(char cur, char prev);

