#include "../../include/compilers/Javascript/Optimiser.h"
#include <string>
#include <sstream>


namespace JSOptimise {

bool hasNumberInBrackets(const std::string& str) {

    bool foundLeftBracket = false;
    bool foundNumber = false;

    for (char c : str) {
        if (c == '(') {
            foundLeftBracket = true;
            continue;
        }
        
        if (std::isdigit(c) && foundLeftBracket) {
            foundNumber = true;
            continue;
        }

        if (c == ')') {
            if (foundLeftBracket && foundNumber) {
                return true;
            } else {
                return false;
            }
        }
        if (!std::isdigit(c) && foundLeftBracket) {
            return false;
        }
    }

    return false;
}


std::string getNumberBetweenBracketsAsString(const std::string& str) {
    std::stringstream ss(str);
    std::string token;

    // Skip the first token (e.g., "stack")
    std::getline(ss, token, '(');

    // Extract the number between brackets
    std::getline(ss, token, ')');
    std::string numberString = token;

    for (char c: numberString) {
        if (!std::isdigit(c)) {
            return "";
        }
    }

    return numberString;
}



vector<string> removeConstantsToStack(vector<string> instructions) {
    // Often a constant is pushed on the stack and then popped from the stack
    // Try to find such occasions
    // TODO: Simplify
    string numConstant, instr, stackPop = "stack.pop()";
    bool previousStackPushWasConstant = false;
    int indexOfStackPush;
    vector<int> instrToRemove;
    
    for (int i = 0; i < instructions.size(); i++) {
        instr = instructions[i];
        if (instr.find("stack.push(",0) == 0) {
            string num = getNumberBetweenBracketsAsString(instr);
            if (num.size() > 0) {
                previousStackPushWasConstant = true;
                indexOfStackPush = i;
                numConstant = getNumberBetweenBracketsAsString(instr);
                continue;
            } else {
                previousStackPushWasConstant = false;
            }

        }

        size_t stackPopLocation = instr.rfind(stackPop);
        if (
            (stackPopLocation  != std::string::npos ) &&
            previousStackPushWasConstant
        ) { 
            instr.replace(stackPopLocation, stackPop.size(), numConstant);
            instructions[i] = instr;
            instrToRemove.push_back(indexOfStackPush);
            indexOfStackPush = -1;


            previousStackPushWasConstant = false;
            numConstant = "";
            
        }


    }

    int removed = 0;
    for (auto i: instrToRemove) {
        instructions.erase(instructions.begin() + (i - removed));
        removed++;
    }
    return instructions;
}

// End of namespace
}


vector<string> JSOptimise::optimise(vector<string> instructions) {
    
    std::clog << "Optimising " << instructions.size() << " statements\n";

    instructions = removeConstantsToStack(instructions);


    std::clog << "Got it down to " << instructions.size() << " statements\n";    
    return instructions;
}