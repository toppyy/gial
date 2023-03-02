#include <iostream>
#include <vector>
#include <fstream> 
#include <string>
#include "./include/Parser.h"
#include "./include/Program.h"


void construct_program(std::vector<std::string> &ir) {
    // Prepare 
}


int main(int argc, char *argv[]) {

    // The first argument is the path to the file containing the sourcecode to compile
    std::string filename;

    for (int i = 0; i < argc; ++i) {
        if (i == 1) {
            filename = argv[i];
        }
    }

    // Read from the text file
    std::ifstream sourceFile(filename);

    std::string content = "", tmp;
        
    // Read the file line by line into a single string
    while (getline(sourceFile, tmp)) {
        content += tmp;
    }

    // Close the file
    sourceFile.close();

    // Init parser and result data structure
    Program pr;
    Parser prsr = Parser(content, pr);
 
    // Do the parsing
    prsr.init();
    prsr.assignment();

    // Build the program
    pr.buildProgram();

    // Finished  
    std::cout << "\n\n";
    
    return 0;

}