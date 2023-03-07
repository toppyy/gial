#include <iostream>
#include <vector>
#include <fstream> 
#include <string>
#include "./include/Parser.h"
#include "./include/Scanner.h"
#include "./include/Program.h"


char handleScandics(char cur, char prev) {
    if (cur == -124 & prev == -61) { // Letter 'Ä'
        return 65; // Letter A
    }
    // -1 is EOF
    if (cur < -1 & prev >= 0) {
        return 0;
    }
    return cur;
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

    // Close the file
    fclose(pFile);

    // Init parser and result data structure
    Program pr = Program();
    Scanner scnr = Scanner(content);
    scnr.init();

    // for (auto t: scnr.getTokens()) { // REMOVE
    //     std::cout << "token " << t.getContent() << ".\n";
    // }

    Parser prsr = Parser(scnr.getTokens());
 
    // // // Do the parsing
    prsr.init();
    
    // // // Build the program
    prsr.buildProgram();

    // Finished  
    std::cout << "\n\n";
    
    return 0;

}