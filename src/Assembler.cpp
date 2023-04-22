#include "./include/Assembler.h"


Assembler::Assembler(Program p_program) :
    tree(nullptr),
    program(p_program) {
}



void Assembler::Assemble(shared_ptr<GAST> p_tree) {
    tree = p_tree;

    
    traverse(tree->getRoot());

    program.buildProgram();
}


void Assembler::traverse(shared_ptr<GNODE> node, bool traverseRight) {

    handleNode(node);

    shared_ptr<GNODE> right  = node->getRight();

    if (right != nullptr && traverseRight) {
        traverse(right);
    }

    /* Note: left-branches are always traversed inside the node-handlers if need be.  */
}

void Assembler::handleNode(shared_ptr<GNODE> node) {

    if (!node) {
        error("nullptr in handle node!"); // TODO raise exception
    }

    string type = node->getType();

    if (type == "root") {
        return;
    }

    if (type == "CONSTANT") {
        handleConstant(node);
        return;
    }

    if (type == "DECLARE") {
        handleDeclare(node);
        return;
    }
    
    if (type == "WHILE") {
        handleWhile(node);
        return;
    }

    if (type == "BOOLTERM") {
        handleBoolTerm(node);
        return;
    }

    if (type == "VARIABLE") {
        handleVariable(node);
        return;
    }

    if (type == "EXPRESSION") {
        handleExpression(node);
        return;
    }

    if (type == "BLOCK") {
        handleBlock(node);
        return;
    }

    if (type == "PRINTSTRCONST") {
        handlePrintStrConst(node);
        return;
    }

    if (type == "PRINTASCII") {
        handlePrintASCII(node);
        return;
    }

    if (type == "PRINTINT") {
        handlePrintInt(node);
        return;
    }


    if (type == "ASSIGN") {
        handleAssign(node);
        return;
    }

    std::cout << "dont know how to handle type " << type << "\n";

}

void Assembler::handleBlock(shared_ptr<GNODE> node) {
    shared_ptr<GNODE> left  = node->getLeft();

    if (left) {
        traverse(left, false); // Right is not automatically traversed
    }
    emitComment("done with block left");
}

void Assembler::handlePrintStrConst(shared_ptr<GNODE> node) {

    string value = node->value;
    string label = program.getNewLabel();
    emitInstruction("printBytes " + label + ", 0, " + std::to_string(value.length()));
    emitConstant(label, value, "str");
}

void Assembler::handlePrintASCII(shared_ptr<GNODE> node) {
    emitInstruction("mov dil, " + node->value);
    emitInstruction("call PrintASCII");
}

void Assembler::handleDeclare(shared_ptr<GNODE> node) {
    
    if (node->datatype == "str") {
        emitVariable(node->name, "str", "byte", node->value.length());
    }

    if (node->datatype == "int") {
        emitVariable(node->name, "int", "qword", 1);
    }

    shared_ptr<GNODE> left = node->getLeft();
    if (left) {
        
        if (left->getType() != "ASSIGN") {
            error("Declare expected assignment, got " + left->getType());
        }
        handleAssign(node->getLeft());
    }

}

void Assembler::handleWhile(shared_ptr<GNODE> node) {

    // Left-child has the condition
    // Right-child has a linked list to the operations within the block
    string labelIn  = program.getNewLabel();
    string labelOut = program.getNewLabel();
    emitInstruction(labelIn + ":");
    traverse(node->getLeft(), false);
    emitInstruction("cmp r8, 1");
    emitInstruction("jne " + labelOut);
    traverse(node->getRight());
    emitInstruction("jmp " + labelIn);
    emitInstruction(labelOut + ":");
    emitComment("done with while");
  
}

void Assembler::handleExpression(shared_ptr<GNODE> node) {
    traverse(node->getLeft());
}

void Assembler::handleVariable(shared_ptr<GNODE> node) {
    // Moves a variable reference to r8
    // We need to pull the variable to know whether to read a byte or ..?
    Variable var = program.getVariable(node->name);

    emitInstruction("mov " + var.getRegister8Size() + ", " + var.makeReferenceTo());

    if (node->hasMathOperator()) {
        // Need to do math with child
        handleAddOperation(node->getRight(), node->op);
        node->makeRightNull(); // We've now consumed right
    }

}

void Assembler::handleBoolTerm(shared_ptr<GNODE> node) {

    // Evaluate left expression; result is stored into r8
    traverse(node->getLeft());
    // Push result onto stack
    emitInstruction("push r8");
    // Eval right expression
    traverse(node->getRight());

    string labelFalse = program.getNewLabel();
    string opInstruction = mapOperatorToInstruction(node->op);

    emitInstruction("pop r9");
    emitInstruction("cmp r9, r8");
    emitInstruction("mov r8, 1");
    emitInstruction(opInstruction + " " + labelFalse);
    emitInstruction("mov r8, 0");
    emitInstruction(labelFalse + ":");
    emitComment("done with bool-term");
    // R8 = 1 if the expression is true, 0 if not
}

void Assembler::handleConstant(shared_ptr<GNODE> node) {
    emitInstruction("mov r8, " + node->value);
}

void Assembler::handleAssign(shared_ptr<GNODE> node) {
    emitComment("assign");
    string name = node->name;

    if (!program.inVariables(name)) {
        error("Assignment to an undeclared variable (" + name + ")");
    }
    // Evaluate the expression that is beign assigned, the result will be in r8
    traverse(node->getLeft());

    Variable var = program.getVariable(name);
    
    emitInstruction("mov " + var.makeReferenceTo() + ", " + var.getRegister8Size());
}

void Assembler::handleAddOperation(shared_ptr<GNODE> node, string op) {
    emitInstruction("push r8"); // Store result of previous node
    handleNode(node);
    emitInstruction("mov r9, r8");
    emitInstruction("pop r8");
    if (op == "+") {
        emitInstruction("add r8, r9");
    }
    if (op == "-") {
        emitInstruction("sub r8, r9");
        
    }
}

void Assembler::handlePrintInt(shared_ptr<GNODE> node) {
    // If 'name' is present, print a variable ref
    // if not, evaluate node on left
    if (node->name != "") {
        Variable var = program.getVariable(node->name);
        emitInstruction("mov rdi, " + var.makeReferenceTo());
    } else {
        shared_ptr<GNODE> left = node->getLeft();
        if (!node) {
            error("Expected PrintInt to have a left node!\n");
        }
        handleNode(left);
    }
    emitInstruction("call PrintInteger");
}

void Assembler::emitInstruction(string inst) {
    program.addInstruction(inst);
}

void Assembler::emitConstant(string out, string value, string varType = "str") {
    program.addConstant(out, value, varType);
    
}

void Assembler::emitComment(std::string comment) {
    emitInstruction("; " + comment);
}

void Assembler::emitVariable(string name, string varType, string size, int length) {
    if (!program.inVariables(name)) {
            program.addVariable(name, varType, size, length);
    }
    
}


void Assembler::error(string error_message) {
    throw std::runtime_error(error_message);
}

string Assembler::mapOperatorToInstruction(string op) {

    std::unordered_map<string,string> operatorToInstruction {
        {"==", "je"},
        {"!=", "jne"},
        {">",  "jg"},
        {"<",  "jl"},
        {">=", "jge"},
        {"<=", "jle"}
    };

    if (operatorToInstruction.count(op) == 0) {
            error("expected: ==, !=, >=, <=, <, <, got: " + op);
    }

    auto search = operatorToInstruction.find(op);
    return search->second;
}