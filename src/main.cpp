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
    char* filename;

    for (int i = 0; i < argc; ++i) {
        if (i == 1) {
            filename = argv[i];
        }
    }

    // Read from the text file
    FILE* pFile;
    pFile = fopen(filename,"r");
    if (pFile==NULL) perror("Error opening file");

    std::string content = "";
    char tmp;

    // Read the file line by line into a single string
    do {
        tmp = getc(pFile);
        content.push_back(tmp);
    } while (tmp != EOF);

    // Close the file
    fclose(pFile);

    // Init parser and result data structure
    Program pr;
    Parser prsr = Parser(content, pr);
 
    // Do the parsing
    prsr.init();
    
    // Build the program
    pr.buildProgram();

    // Finished  
    std::cout << "\n\n";
    
    return 0;

}