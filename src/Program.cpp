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

