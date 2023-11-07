#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include "./GAST.h"
#include "./Parser.h"
#include "./NASM.h"
#include "./Javascript.h"
#include "./Assembler.h"
#include "./Optimiser.h"
#include "./Scanner.h"
#include "./keywords.h"

using std::vector, std::string;

char* compile(char* p_assembler, char* content, int p_optimise);
char handleScandics(char cur, char prev);
