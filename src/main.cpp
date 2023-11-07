#include "./include/gial.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>

int main(int argc, char *argv[]) {



    // The first argument is the path to the file containing the sourcecode to compile
    char* p_filename;
    char* p_assembler;

    char default_assembler[5] = "NASM";
    p_assembler = default_assembler;

    bool do_optimisation = false;

    for (int i = 0; i < argc; ++i) {
        if (i == 1) {
            p_filename = argv[i];
        }
        // It's a flag
        if (argv[i][0] == '-') {
            if (strcmp(argv[i], "-o") == 0) {
                do_optimisation = true;
                continue;
            }

            if (strcmp(argv[i], "-a") == 0) {
                if (argc < i + 1) {
                    // Should not happen
                    printf("Flag '-a' needs an assembler\n");
                    continue;
                }
                i++;
                p_assembler =  argv[i];
                continue;
            }

        }
    }
    // Manipulate the content and the pass it to the compiler
    // Read from the text file
    FILE* pFile;
    pFile = fopen(p_filename,"r");
    if (pFile==NULL) perror("Error opening file");

    std::string content = "";
    char cur, prev, curManipulated;

    // Read the file line by line into a single string

    // Here's a dirty trick to change (some) scandics into ASCII with the following mapping:
    //  Ä -> A
    // (Ö,ö,å have no role in giäl, so forget about them atm)
    // Might not work for all encodings so use with caution.
    // Also the program cannot start with a scandic.

    cur = getc(pFile);
    content.push_back(cur);
    do {

        prev = cur;
        cur = getc(pFile);
        curManipulated = handleScandics(cur, prev);
        if (curManipulated > 0) {
            content.push_back(curManipulated);
        }

    } while (cur != EOF);
    content.push_back(-1); // We want EOF
    fclose(pFile);

   

    // Actual compilation
    vector<string> instructions = compile(p_assembler, content, do_optimisation);

    for (auto instr: instructions) {
        std::cout << instr << "\n";
    }

    // Finished
    std::cout << "\n\n";

    return 0;

}