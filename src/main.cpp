#include <iostream>
#include <fstream> 
#include <string>
#include "./include/Parser.h"


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

    Parser prsr = Parser(content);
 
    // Do the parsing
    prsr.init();
    prsr.expression();


    // Finished
  
    std::cout << "\n\n";
    
    return 0;

}