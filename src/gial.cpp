#include "./include/gial.h"

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



vector<string> compile(char* p_assembler, char* p_filename, bool p_optimise) {
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

    // Close the file
    fclose(pFile);

    std::set<std::string> keywords = getKeywords();

    // Init scanner & parser and result data structure
    Scanner scnr = Scanner(content, keywords);
    scnr.init();


    GAST tree = GAST();
    Parser prsr = Parser(scnr.getTokens(), keywords, tree);
    prsr.init();

    vector<string> instructions;
    if ( strcmp(p_assembler,"JS") == 0) {
        Javascript asmblr = Javascript(tree);
        instructions = asmblr.assemble();
    } else {
        NASM asmblr = NASM(tree);
        instructions = asmblr.assemble();
    }

    if (p_optimise) {
        instructions = optimise(instructions);
    }

    return instructions;
}
