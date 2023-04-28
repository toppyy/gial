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

    if (type == "IF") {
        handleIf(node);
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

    if (type == "PRINTASCII") {
        handlePrintASCII(node);
        return;
    }

    if (type == "PRINTINT") {
        handlePrintInt(node);
        return;
    }

    if (type == "PRINTSTR") {
        handlePrintString(node);
        return;
    }

    if (type == "ASSIGN") {
        handleAssign(node);
        return;
    }

    if (type == "INPUT") {
        handleInput(node);
        return;
    }

    error("dont know how to handle type " + type);

}

void Assembler::handleBlock(shared_ptr<GNODE> node) {
    // Empty on purpose. Just a container for tree manipulations    
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
        emitVariable(node->name, "int", "qword", node->size);
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

void Assembler::handleIf(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    checkNullPtr(node->getRight(), node);

    emitComment("If started");

    // Left-child has the condition
    // Right-child has a linked list to the operations within the block
    string labelIn  = program.getNewLabel();
    string labelElse = program.getNewLabel();
    string labelOut = program.getNewLabel();
    emitInstruction(labelIn + ":");
    traverse(node->getLeft());
    emitInstruction("cmp r8, 1");
    emitInstruction("jne " + labelElse);
    // The code to be executed if the condition evaluates TRUE is 
    // wrapped in a nested GNODE. The right branch is executed if TRUE.
    // The left branch is taken if FALSE and IF-ELSE exists
    traverse(node->getRight()->getRight());
    emitInstruction("jmp " + labelOut);
    emitInstruction(labelElse + ":");
    if (node->getRight()->getLeft()) {
        traverse(node->getRight()->getLeft());
    }
    emitInstruction(labelOut + ":");
    emitComment("If done");
}

void Assembler::handleExpression(shared_ptr<GNODE> node) {
    checkNullPtr(node->getLeft(), node);    
    traverse(node->getLeft());

    if (node->hasMathOperator()) {
        checkNullPtr(node->getRight(), node);
        handleMathOperation(node->getRight(), node->op);
    }

}

void Assembler::handleVariable(shared_ptr<GNODE> node) {
    // Moves a variable reference to r8
    // We need to pull the variable to know whether to read a byte or ..?
    Variable var = program.getVariable(node->name);

    // If there's a right branch, it's an indexed assignment
    // The right branch has an expr that will evaluate to the index (usually an int constant)
    shared_ptr<GNODE> right = node->getRight();

    string offset = "";
    if (right) {
        // Store the index into a register (R9) and offset
        // the MOV-operation with the value in this register
        traverse(right);            // The value is now in R8
        offset = "r8";
    }
    emitInstruction("mov " + var.getRegister8Size() + ", " + var.makeReferenceTo(offset));
}

void Assembler::handleBoolTerm(shared_ptr<GNODE> node) {
    emitComment("bool-term started");
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

    if (node->datatype == "int") {
        emitInstruction("mov r8, " + node->value);
        return;
    }

    if (node->datatype == "chr") {
        emitInstruction("mov r8, '" + node->value + "'");
        return;
    }

    if (node->datatype == "str") {
        error("Dont know how to handle str-constant atm");
        return;
    }
}

void Assembler::handleAssign(shared_ptr<GNODE> node) {
    emitComment("assign from " + node->getParent()->getType());
    string name = node->name;

    if (!program.inVariables(name)) {
        error("Assignment to an undeclared variable (" + name + ")");
    }
    checkNullPtr(node->getLeft(), node);

    // If there's a right branch, it's an indexed assignment
    // The right branch has an expr that will evaluate to the index (usually an int constant)
    shared_ptr<GNODE> right = node->getRight();

    if (right) {
        // We store the index into a register (R9). Later on (see below)
        // Well offset the MOV-operation with the value in this register
        traverse(right);            // The value is now in R8
        emitInstruction("push r8"); // Onto stack
    }

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
        emitComment("assign done ");
        return;
    }

    traverse(left);
    Variable var = program.getVariable(name);

    string offset = "";
    if (right) { 
        // Indexed assignment. Pop the index from stack
        emitInstruction("pop r9");
        offset = "r9";
    }

    emitInstruction("mov " + var.makeReferenceTo(offset) + ", " + var.getRegister8Size());
    emitComment("assign done ");
}

void Assembler::handleMathOperation(shared_ptr<GNODE> node, string op) {
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

void Assembler::handlePrintString(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    shared_ptr<GNODE> left = node->getLeft();

    if (left->getType() == "CONSTANT") {
                string value = left->value;
                string label = program.getNewLabel();
                emitInstruction("printBytes " + label + ", 0, " + to_string(left->value.length()));
                emitConstant(label, value, "str");
    }

    if (left->getType() == "VARIABLE") {

        Variable variable = program.getVariable(left->name);

        if (variable.type == "str") {

            string loopStart = program.getNewLabel();
            string varName   = variable.identifier;
            
            // Loop until NULL
            emitInstruction("mov r11, 0");            
            emitInstruction(loopStart + ":");
            emitInstruction("push r11");
            emitInstruction("mov dil, byte[" + varName + " + r11]");
            emitInstruction("call PrintASCII");
            emitInstruction("pop r11");
            emitInstruction("inc r11");
            emitInstruction("mov r8b, byte[" + varName + " + r11]");
            emitInstruction("cmp r8b, 0");
            emitInstruction("jne " + loopStart);
            return;
        }

        error("Expected 'str', got '" + variable.type + "'");

    }

    
}

void Assembler::handlePrintInt(shared_ptr<GNODE> node) {
    // If 'name' is present, print a variable ref
    // if not, evaluate node on left
    if (node->name != "") {
        Variable var = program.getVariable(node->name);
        emitInstruction("mov rdi, " + var.makeReferenceTo());
        emitComment("printing variable " + node->name);
    } else {
        shared_ptr<GNODE> left = node->getLeft();
        if (!node) {
            error("Expected PrintInt to have a left node!\n");
        }
        handleNode(left);
        emitInstruction("mov rdi, r8");
    }
    emitInstruction("call PrintInteger");
}

void Assembler::handleInput(shared_ptr<GNODE> node) {


    string bufferName = node->name;
    string labelInputLoop = program.getNewLabel();
    string labelLoopOut = program.getNewLabel();
    
    string bufferRef = "byte[" + bufferName + " + r9]";

    bool integerInput = node->datatype == "int";
    string varType = node->datatype;

    string varSize = "byte";
    int varLength = 100;

    if (integerInput) {
        varSize = "qword";
        varLength = 1;
    }

    if (integerInput) {
        // For converting the ASCII-representation to int
        emitInstruction("mov r8, 0");   // Intermediate result
        emitInstruction("push r8");
    }
    
    emitInstruction("mov r9, 0");                   // Init character count -loop
    emitInstruction(labelInputLoop + ":");          // Loop to read characters one-by-one from input
    emitInstruction("mov rax, SYS_READ");           // Specify system call
    emitInstruction("mov rdi, STDIN");              // Reading from standard input
    emitInstruction("lea rsi, " + bufferRef);       // Specify address of the space reserved for input
    emitInstruction("mov rdx, 1");                  // Read one byte (=character) at a time
    emitInstruction("syscall"); 



    emitInstruction("cmp " + bufferRef + ", LF");   // Is the input character LF?
    emitInstruction("je " + labelLoopOut);          //  If yes, stop reading by jumping out loop

    if (integerInput) {
        // Convert to integer by adding to previous number and multiplying by 10
        emitInstruction("pop r8");
        emitInstruction("mov rax, r8");             // The other operand is now in rax
        emitInstruction("mov r11, 10");
        emitInstruction("mul r11");                 // (signed) multiplication
        emitInstruction("mov r8, rax");   

        emitInstruction("mov r12, 0");
        emitInstruction("mov r12b, " + bufferRef);
        emitInstruction("sub r12b, 48");            // 48 = 0 in ASCIi

        emitInstruction("add r8, r12");
        emitInstruction("push r8");
        
    }

    emitInstruction("inc r9");                      //  If no, increment character count
    emitInstruction("jmp " + labelInputLoop);       // And jump back to reading another character

    emitInstruction(labelLoopOut + ":");
    emitInstruction("mov " + bufferRef + ", 0");     // Null-terminate the string

    if (!integerInput) {
        // We're done with a string
        return;
    }
    emitInstruction("mov qword[" + bufferName + "], r8");

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
        handleMathOperation(node->getLeft(), node->op);
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