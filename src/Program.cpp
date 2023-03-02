#include "./include/Program.h"

Program::Program()  {
}

std::vector<std::string> Program::getInstructions() {
    return instructions;
}

void Program::addInstruction(std::string instruction) {
    instructions.push_back(instruction);
}

void Program::addVariable(std::string variable) {
    variables.push_back(variable);
}


void Program::outputLine(std::string s) {
    std::cout << s << "\n";
}

void Program::buildProgram() {
    // Responsible for putting instructions and variable declarations in their place
    // to create a complete program

    outputLine("LF equ 10");
    outputLine("extern PrintInteger");
    outputLine("extern PrintASCII");

    outputLine("section .bss");
    // Variable declarations
    for (auto variable: variables) {
        std::cout << "\t" + variable + "\n";
    }

    outputLine("section .text");
    outputLine("global _start");
    outputLine("_start:");
    
    instructions.push_back("mov rdi, qword[a]");
    instructions.push_back("call PrintInteger ");
    instructions.push_back("mov dil, LF");
    instructions.push_back("call PrintASCII");

    // Print instructions
    for (auto instruction: instructions) {
        std::cout << "\t" + instruction + "\n";
    }
    outputLine("\n");
    outputLine("\tmov rax, 60");
    outputLine("\txor rdi, 0");
    outputLine("\tsyscall");
}
