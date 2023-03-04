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

bool Program::inVariables(std::string variable) {
    for (auto var: variables) {
        if (var == variable) {
            return true;
        }
    }
    return false;
}

void Program::outputLine(std::string s) {
    std::cout << s << "\n";
}

void Program::buildProgram() {
    // Responsible for putting instructions and variable declarations in their place
    // to create a complete program

    // Constants and libraries
    outputLine("STDIN       equ 0");
    outputLine("STDOUT      equ 1");
    outputLine("SYS_READ    equ 0");
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
    

    // Print instructions
    for (auto instruction: instructions) {
        std::cout << "\t" + instruction + "\n";
    }
    outputLine("\n");
    outputLine("\tmov rax, 60");
    outputLine("\txor rdi, 0");
    outputLine("\tsyscall");
}
