#include <iostream>
#include <vector>
#include <fstream> 
#include <string>
#include "./include/GAST.h"
#include "./include/Parser.h"
#include "./include/NASM.h"
#include "./include/Javascript.h"
#include "./include/Assembler.h"
#include "./include/Scanner.h"
#include "./keywords.cpp"

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


void printTree(std::shared_ptr<GNODE> node, int depth, GAST& tree) {


    int tabs = depth;
    string tab = "";
    while (tabs > 0) {
        std::cout << "\t";
        tabs--;
    }
    
    std::clog << "type is " << node->getType() << " and id is " << node->id
        << " and left is " << node->getLeft() 
        << " and right is " << node->getRight() 
        << " and next is "  << node->getNext()
        << "\n";

    std::shared_ptr<GNODE> left   = tree.getNode(node->getLeft());
    std::shared_ptr<GNODE> right  = tree.getNode(node->getRight());
    std::shared_ptr<GNODE> next   = tree.getNode(node->getNext());

    if (left != nullptr) {
        printTree(left, depth + 1, tree);
    }

    if (right != nullptr) {
        printTree(right, depth + 1, tree);
    }

    if (next != nullptr) {
        printTree(next, depth, tree);
    }

    return;
}


int main(int argc, char *argv[]) {



    // The first argument is the path to the file containing the sourcecode to compile
    char* filename;
    char* p_assembler;

    char default_assembler[5] = "NASM"; 
    p_assembler = default_assembler;

    for (int i = 0; i < argc; ++i) {
        if (i == 1) {
            filename = argv[i];
        }
        if (i == 2) {
            p_assembler = argv[i];
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

    std::set<std::string> keywords = getKeywords();
    
    // Init scanner & parser and result data structure
    Scanner scnr = Scanner(content, keywords);
    scnr.init();

    
    GAST tree = GAST();
    
    //std::shared_ptr<GAST> p_tree = std::make_shared<GAST>( GAST(root) );
    
    Parser prsr = Parser(scnr.getTokens(), keywords, tree);
   

    prsr.init();


    printTree(tree.getRoot(), 0, tree);
    
    // tree.returnToRoot();
    
    // std::unique_ptr<Assembler> asmblr;
    // if ( strcmp(p_assembler,"JS") == 0) {
    //     asmblr = std::make_unique<Javascript>();
    // } else {
    //     asmblr = std::make_unique<NASM>();
    // }
    // asmblr->assemble(p_tree);

    std::shared_ptr<GAST> p_tree = std::make_shared<GAST>( tree );
    if ( strcmp(p_assembler,"JS") == 0) {
        Javascript asmblr = Javascript();
        asmblr.assemble(p_tree);
    } else {
        NASM asmblr = NASM();
        asmblr.assemble(p_tree);
    }

    // Finished  
    std::cout << "\n\n";
    
    return 0;

}