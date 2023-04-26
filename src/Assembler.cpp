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

void Assembler::traverse(shared_ptr<GNODE> node) {

    handleNode(node);

    shared_ptr<GNODE> next  = node->getNext();

    if (next != nullptr) {
        traverse(next);
    }

    /* Note: left- and rightbranches are always traversed inside the node-handlers if need be.  */
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
    if (node->getNext()) {
        traverse(node->getNext());
    }
    
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
        emitVariable(node->name, "str", "byte", node->size + 1); // +1 for NULL-termination
    }

    if (node->datatype == "int") {
        emitVariable(node->name, "int", "qword", 1);
    }

    shared_ptr<GNODE> left = node->getLeft();
    if (left) {
        
        if (left->getType() != "ASSIGN") {
            error("Declare expected assignment, got " + left->getType());
        }
        checkNullPtr(node->getLeft(), node);
        handleAssign(node->getLeft());
    }

}

void Assembler::handleWhile(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    checkNullPtr(node->getRight(), node);

    // Left-child has the condition
    // Right-child has a linked list to the operations within the block
    string labelIn  = program.getNewLabel();
    string labelOut = program.getNewLabel();
    emitInstruction(labelIn + ":");
    traverse(node->getLeft());
    emitInstruction("cmp r8, 1");
    emitInstruction("jne " + labelOut);
    traverse(node->getRight());
    emitInstruction("jmp " + labelIn);
    emitInstruction(labelOut + ":");
  
}

void Assembler::handleExpression(shared_ptr<GNODE> node) {
    checkNullPtr(node->getLeft(), node);
    traverse(node->getLeft());
}

void Assembler::handleVariable(shared_ptr<GNODE> node) {
    // Moves a variable reference to r8
    // We need to pull the variable to know whether to read a byte or ..?
    Variable var = program.getVariable(node->name);

    emitInstruction("mov " + var.getRegister8Size() + ", " + var.makeReferenceTo());

    checkForMathOps(node);
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
    checkForMathOps(node);
}

void Assembler::handleAssign(shared_ptr<GNODE> node) {
    emitComment("assign");
    string name = node->name;

    if (!program.inVariables(name)) {
        error("Assignment to an undeclared variable (" + name + ")");
    }
    checkNullPtr(node->getLeft(), node);
    // If it's an int:
    //  Evaluate the expression that is beign assigned, the result will be in r8
    // If it's a constant str:
    //  Handle pointer locally (do not traverse)
    shared_ptr<GNODE> left = node->getLeft();     

    if (left->getType() == "CONSTANT" & left->datatype == "str") {
        int byteIdx = 0;
        for (auto c: left->value) {
            emitInstruction("mov byte[" + name + " + " + to_string(byteIdx) + "], " + to_string(int(c)));
            byteIdx++;
        }
        emitInstruction("mov byte[" + name + " + " + to_string(byteIdx) + "], 0" ); // NULL-termination
        return;
    }

    traverse(left);
    Variable var = program.getVariable(name);
    emitInstruction("mov " + var.makeReferenceTo() + ", " + var.getRegister8Size());
}

void Assembler::handleAddOperation(shared_ptr<GNODE> node, string op) {
    emitInstruction("push r8"); // Store result of previous node
    handleNode(node);           // Content will be R8 after this
    
    if (op == "+") {
        emitInstruction("mov r9, r8");
        emitInstruction("pop r8");
        emitInstruction("add r8, r9");
    }
    if (op == "-") {
        emitInstruction("mov r9, r8");
        emitInstruction("pop r8");
        emitInstruction("sub r8, r9");
        
    }
    if (op == "*") {
        emitInstruction("pop rax");      // The other operand is now in rax
        emitInstruction("imul rax, r8"); // (signed) multiplication
        emitInstruction("mov r8, rax");  // Store result into r8 like all calculations
    }
    if (op == "/") {
        emitInstruction("pop rax");    // Pop the value we want to divided from stack (stack uses 64 bits, so we must use rax)
        emitInstruction("cdq");        // convert qword in eax to qword in edx:eax
        emitInstruction("idiv r8");    // divisor. Now rax has the quotient. We forget the remainder
        emitInstruction("mov r8, rax");// Store quotient into r8 like all calculations
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

void Assembler::checkForMathOps(shared_ptr<GNODE> node) {
    if (node->hasMathOperator()) {
        checkNullPtr(node->getLeft(), node);
        // Need to do math with child
        handleAddOperation(node->getLeft(), node->op);
        node->makeRightNull(); // We've now consumed right
    }
}


void Assembler::checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from) {
    if (!node) {
        error("nullptr! Origin: " + from->getType());
    }
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