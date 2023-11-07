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


    
char* compile(char* p_assembler, string content, int p_optimise) {
    
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

    // Concat instructions to a string
    string instr_str = "";

    for (auto s: instructions) {
        instr_str += s;
        instr_str += "\n";
    }

    // string to char* (for wasm)
    char* instr_char_p = new char[instr_str.size() + 1]; 
    strcpy(instr_char_p, instr_str.c_str());

    return instr_char_p;
}
