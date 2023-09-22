#include "./include/NASMProgram.h"

Variable::Variable(string p_identifier, string p_type, string p_size, int p_length) :
    identifier(""),
    type(""),
    length(),
    size() {
    identifier = p_identifier;
    size = p_size;
    type = p_type;
    length = p_length;
}


int Variable::sizeInBytes() {
    if (size == "qword") {
        return 8;
    }
    if (size == "byte") {
        return 1;
    }
    throw std::runtime_error("Don't know the size of type: " + size);
    return -1;
}

string Variable::strSizeInBytes() {
    return std::to_string(sizeInBytes());
}

string Variable::getRegister8Size() {
    if (size == "qword") {
        return "r8";
    }
    if (size == "byte") {
        return "r8b";
    }
    throw std::runtime_error("Don't know the R8-size for type: " + size);
    return "";
}


string Variable::makeReferenceTo() {
    // Returns example: qword[varName]
    return size + "[" + identifier + "]";
}

string Variable::makeReferenceTo(string offsetRegister) {
    if (offsetRegister=="") {
        return makeReferenceTo();
    }

    // Returns example: qword[varName + (r9 * 8)]
    return size + "[" + identifier + " + (" + offsetRegister + " * " + strSizeInBytes() + ")]";
}


Constant::Constant(string p_identifier, string p_value, string p_type) : 
    identifier(""),
    type(""),
    value("")
    {
    identifier = p_identifier;
    type = p_type;
    value = p_value;
}


NASMProgram::NASMProgram(std::ostream& p_output_stream) :
        instructions(std::vector<string> {}),
        icount(0),
        labelCount(0),
        variables(std::unordered_map<string, Variable > {}),
        output_stream(p_output_stream)
        {
        
        int icount = 0;
        instructions = std::vector<string> {};
}

std::vector<string> NASMProgram::getInstructions() {
    return instructions;
}

void NASMProgram::addInstruction(string instruction) {
    instructions.push_back(instruction);
    icount++;
}

void NASMProgram::addConstant(string identifier, string value,string type) {
    constants.insert({ identifier, Constant(identifier, value, type) });
}


void NASMProgram::addVariable(string identifier, string type, string size, int length) {
    variables.insert({ identifier, Variable(identifier, type, size, length) });
}

bool NASMProgram::inVariables(string variable) {
    return variables.count(variable) > 0;
}


Variable NASMProgram::getVariable(string identifier) {
    if (auto search = variables.find(identifier); search != variables.end()) {
        return search->second;
    } else {
        throw std::runtime_error("Variable " + identifier + " has not been declared");
    }
    return Variable("","","",0); // Unreachable
}

bool NASMProgram::isStringVariable(string variable) {
    if (!inVariables(variable))  {
        return false;
    }
    if (auto search = variables.find(variable); search != variables.end())
        return search->second.type == "str";
    

    return false;
 
}

string NASMProgram::getNewLabel() {
    labelCount += 1;
    return "LBL_" + std::to_string(labelCount);
}

std::vector<string> NASMProgram::buildProgram() {
    // Responsible for putting instructions and variable declarations in their place
    // to create a complete program

    std::vector<std::string> program;

    // Constants and libraries
    program.push_back("STDIN       equ 0");
    program.push_back("STDOUT      equ 1");
    program.push_back("SYS_READ    equ 0");
    program.push_back("LF equ 10");

    // Standard macro to print n bytes to stdout
    program.push_back("%macro printBytes 3");
    program.push_back("\tmov       rax, 1");
    program.push_back("\tmov       rdi, 1");
    program.push_back("\tmov       rsi, %1");
    program.push_back("\tadd       rsi, %2");
    program.push_back("\tmov       rdx, %3");
    program.push_back("\tsyscall");
    program.push_back("%endmacro");


    program.push_back("extern PrintInteger");
    program.push_back("extern PrintASCII");

    program.push_back("section .bss");

    // Variable declarations
    for (auto& variable: variables) {

        string sizeReserveUnit = "resq";
        if (variable.second.size == "byte") {
            sizeReserveUnit = "resb";
        }

        string variableDeclaration = variable.second.identifier + " " + sizeReserveUnit + " ";
        variableDeclaration  += std::to_string(variable.second.length);

        program.push_back("\t" + variableDeclaration);
    }

    program.push_back("section .data");
    // Variable declarations
    for (auto& constant: constants) {

        string constantDeclaration = constant.second.identifier + " db " + "\"" + constant.second.value +  "\""   ;

        program.push_back("\t" + constantDeclaration);
    }


    program.push_back("section .text");
    program.push_back("global _start");
    program.push_back("_start:");
    

    // Print instructions
    for (auto instruction: instructions) {
        program.push_back("\t" + instruction);
    }
    program.push_back("\n");
    program.push_back("\tmov rax, 60");
    program.push_back("\txor rdi, 0");
    program.push_back("\tsyscall");
    return program;
}
