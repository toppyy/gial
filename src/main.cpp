#include <iostream>
#include "./include/gial.h"

int main(int argc, char *argv[]) {



    // The first argument is the path to the file containing the sourcecode to compile
    char* filename;
    char* p_assembler;

    char default_assembler[5] = "NASM";
    p_assembler = default_assembler;

    bool do_optimisation = false;

    for (int i = 0; i < argc; ++i) {
        if (i == 1) {
            filename = argv[i];
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


    vector<string> instructions = compile(p_assembler, filename, do_optimisation);

    for (auto instr: instructions) {
        std::cout << instr << "\n";
    }

    // Finished
    std::cout << "\n\n";

    return 0;

}