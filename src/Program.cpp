#include "./include/Program.h"

Variable::Variable(std::string p_identifier, std::string p_type, std::string p_size, int p_length) :
    identifier(""),
    type(""),
    length(),
    size() {
    identifier = p_identifier;
    size = p_size;
    type = p_type;
    length = p_length;
}

Constant::Constant(std::string p_identifier, std::string p_value, std::string p_type) : 
    identifier(""),
    type(""),
    value("")
    {
    identifier = p_identifier;
    type = p_type;
    value = p_value;
}


Program::Program(std::ostream& p_output_stream) :
        instructions(std::vector<std::string> {}),
        icount(0),
        variables(std::unordered_map<std::string, Variable > {}),
        output_stream(p_output_stream)
        {
        
        int icount = 0;
        instructions = std::vector<std::string> {};
}

std::vector<std::string> Program::getInstructions() {
    return instructions;
}

void Program::addInstruction(std::string instruction) {
    instructions.push_back(instruction);
    icount++;
}

void Program::addConstant(std::string identifier, std::string value,std::string type) {
    constants.insert({ identifier, Constant(identifier, value, type) });
}


void Program::addVariable(std::string identifier, std::string type, std::string size, int length) {
    variables.insert({ identifier, Variable(identifier, type, size, length) });
}

bool Program::inVariables(std::string variable) {
    return variables.count(variable) > 0;
}

void Program::outputLine(std::string s) {
    output_stream << s << "\n";
}

Variable Program::getVariable(std::string identifier) {
    if (auto search = variables.find(identifier); search != variables.end()) {
        return search->second;
    } else {
        throw std::runtime_error("Variable " + identifier + " has not been declared");
    }
    return Variable("","","",0); // Unreachable
}

bool Program::isStringVariable(std::string variable) {
    if (!inVariables(variable))  {
        return false;
    }
    if (auto search = variables.find(variable); search != variables.end())
        return search->second.type == "str";
    

    return false;
 
}

void Program::buildProgram() {
    // Responsible for putting instructions and variable declarations in their place
    // to create a complete program

    // Constants and libraries
    outputLine("STDIN       equ 0");
    outputLine("STDOUT      equ 1");
    outputLine("SYS_READ    equ 0");
    outputLine("LF equ 10");

    outputLine("%macro printBytes 3               ; %1 = address of string to output, %2 = byte offset, %3 = number of bytes");
    outputLine("\tmov       rax, 1                  ; system call for write");
    outputLine("\tmov       rdi, 1                  ; file handle 1 is stdout");
    outputLine("\tmov       rsi, %1                 ; address of string to output");
    outputLine("\tadd       rsi, %2                 ; byte offset");
    outputLine("\tmov       rdx, %3                 ; number of bytes");
    outputLine("\tsyscall                           ; invoke operating system to do the write");
    outputLine("%endmacro");


    outputLine("extern PrintInteger");
    outputLine("extern PrintASCII");

    outputLine("section .bss");

    // Variable declarations
    for (auto& variable: variables) {

        std::string sizeReserveUnit = "resq";
        if (variable.second.size == "byte") {
            sizeReserveUnit = "resb";
        }

        std::string variableDeclaration = variable.second.identifier + " " + sizeReserveUnit + " ";
        variableDeclaration  += std::to_string(variable.second.length);

        output_stream << "\t" + variableDeclaration + "\n";
    }

    outputLine("section .data");
    // Variable declarations
    for (auto& constant: constants) {

        std::string constantDeclaration = constant.second.identifier + " db " + "\"" + constant.second.value +  "\""   ;

        outputLine("\t" + constantDeclaration);
    }


    outputLine("section .text");
    outputLine("global _start");
    outputLine("_start:");
    

    // Print instructions
    for (auto instruction: instructions) {
        output_stream << "\t" + instruction + "\n";
    }
    outputLine("\n");
    outputLine("\tmov rax, 60");
    outputLine("\txor rdi, 0");
    outputLine("\tsyscall");
}
