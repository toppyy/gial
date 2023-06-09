#include "./include/Program.h"

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


Program::Program(std::ostream& p_output_stream) :
        instructions(std::vector<string> {}),
        icount(0),
        labelCount(0),
        variables(std::unordered_map<string, Variable > {}),
        output_stream(p_output_stream)
        {
        
        int icount = 0;
        instructions = std::vector<string> {};
}

std::vector<string> Program::getInstructions() {
    return instructions;
}

void Program::addInstruction(string instruction) {
    instructions.push_back(instruction);
    icount++;
}

void Program::addConstant(string identifier, string value,string type) {
    constants.insert({ identifier, Constant(identifier, value, type) });
}


void Program::addVariable(string identifier, string type, string size, int length) {
    variables.insert({ identifier, Variable(identifier, type, size, length) });
}

bool Program::inVariables(string variable) {
    return variables.count(variable) > 0;
}

void Program::outputLine(string s) {
    output_stream << s << "\n";
}

Variable Program::getVariable(string identifier) {
    if (auto search = variables.find(identifier); search != variables.end()) {
        return search->second;
    } else {
        throw std::runtime_error("Variable " + identifier + " has not been declared");
    }
    return Variable("","","",0); // Unreachable
}

bool Program::isStringVariable(string variable) {
    if (!inVariables(variable))  {
        return false;
    }
    if (auto search = variables.find(variable); search != variables.end())
        return search->second.type == "str";
    

    return false;
 
}

string Program::getNewLabel() {
    labelCount += 1;
    return "LBL_" + std::to_string(labelCount);
}

void Program::buildProgram() {
    // Responsible for putting instructions and variable declarations in their place
    // to create a complete program

    // Constants and libraries
    outputLine("STDIN       equ 0");
    outputLine("STDOUT      equ 1");
    outputLine("SYS_READ    equ 0");
    outputLine("LF equ 10");

    // Standard macro to print n bytes to stdout
    outputLine("%macro printBytes 3");
    outputLine("\tmov       rax, 1");
    outputLine("\tmov       rdi, 1");
    outputLine("\tmov       rsi, %1");
    outputLine("\tadd       rsi, %2");
    outputLine("\tmov       rdx, %3");
    outputLine("\tsyscall");
    outputLine("%endmacro");


    outputLine("extern PrintInteger");
    outputLine("extern PrintASCII");

    outputLine("section .bss");

    // Variable declarations
    for (auto& variable: variables) {

        string sizeReserveUnit = "resq";
        if (variable.second.size == "byte") {
            sizeReserveUnit = "resb";
        }

        string variableDeclaration = variable.second.identifier + " " + sizeReserveUnit + " ";
        variableDeclaration  += std::to_string(variable.second.length);

        output_stream << "\t" + variableDeclaration + "\n";
    }

    outputLine("section .data");
    // Variable declarations
    for (auto& constant: constants) {

        string constantDeclaration = constant.second.identifier + " db " + "\"" + constant.second.value +  "\""   ;

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
