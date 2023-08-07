#include "./include/NASM.h"
#include "./include/NASMProgram.h"

NASM::NASM() {
    tree    = nullptr;
    program = nullptr;
}

void NASM::assemble(shared_ptr<GAST> p_tree) {
    
    program = std::make_unique<NASMProgram>(std::cout);

    tree = p_tree;
    traverse(tree->getRoot());
    program->buildProgram();

}

void NASM::traverse(shared_ptr<GNODE> node) {

    handleNode(node);

    shared_ptr<GNODE> next  = node->getNext();

    if (next != nullptr) {
        traverse(next);
    }

    /* Note: left- and rightbranches are always traversed inside the node-handlers if need be.  */
} 

void NASM::handleNode(shared_ptr<GNODE> node) {

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

    if (type == "FOR") {
        handleFor(node);
        return;
    }

    if (type == "BOOLEXPR") {
        handleBoolExpression(node);
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

void NASM::handleBlock(shared_ptr<GNODE> node) {
    // Empty on purpose. Just a container for tree manipulations    
}

void NASM::handlePrintASCII(shared_ptr<GNODE> node) {
    emitInstruction("mov dil, " + node->value);
    emitInstruction("call PrintASCII");
}

void NASM::handleDeclare(shared_ptr<GNODE> node) {
    
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

void NASM::handleWhile(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    checkNullPtr(node->getRight(), node);

    // Left-child has the condition
    // Right-child has a linked list to the operations within the block
    string labelIn  = program->getNewLabel();
    string labelOut = program->getNewLabel();
    emitInstruction(labelIn + ":");
    traverse(node->getLeft());
    emitInstruction("cmp r8, 1");
    emitInstruction("jne " + labelOut);
    traverse(node->getRight());
    emitInstruction("jmp " + labelIn);
    emitInstruction(labelOut + ":");
  
}

void NASM::handleFor(shared_ptr<GNODE> node) {

    // Right-child has a linked list to the operations within the block
    checkNullPtr(node->getRight(), node);

    string loopStart    = program->getNewLabel();
    string variableRef  = "qword[" + node->name + "]";
    string step         = node->getFromInfo("step");
    string to           = node->getFromInfo("to");
    string toType       = node->getFromInfo("toType");

    if (toType == "variable") {
        to = "qword[" + to + "]";
    }

    emitInstruction("mov r8, " + variableRef);   // Init counter
    emitInstruction(loopStart + ":");            // Loop start label
    emitInstruction("push r8");                  // Push counter onto stack
    
    traverse(node->getRight());

    emitInstruction("pop r8");                      // Pop counter from stack
    emitInstruction("add r8, " + step);             // Increment counter from stack
    emitInstruction("mov " + variableRef + ", r8"); // Update loop variable
    emitInstruction("cmp r8, " + to);               // Compare counter to n
    emitInstruction("jl " + loopStart);             // If it's less than n, jmp to labelTrue

  
}

void NASM::handleIf(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    checkNullPtr(node->getRight(), node);

    emitComment("If started");

    // Left-child has the condition
    // Right-child has a linked list to the operations within the block
    string labelIn  = program->getNewLabel();
    string labelElse = program->getNewLabel();
    string labelOut = program->getNewLabel();
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

void NASM::handleExpression(shared_ptr<GNODE> node) {
    
    checkNullPtr(node->getLeft(), node);
    traverse(node->getLeft());

    if (node->hasMathOperator()) {
        checkNullPtr(node->getRight(), node);
        handleMathOperation(node->getRight(), node->op);
    }
}

void NASM::handleVariable(shared_ptr<GNODE> node) {
    // Moves a variable reference to r8
    // We need to pull the variable to know whether to read a byte or ..?
    Variable var = program->getVariable(node->name);

    
    // If there's a right branch, it's an indexed assignment
    // The right branch has an expr that will evaluate to the index (usually an int constant)
    shared_ptr<GNODE> right = node->getRight();

    if (right) {
        // Store the index into a register (R9) and offset
        // the MOV-operation with the value in this register
        traverse(right);            // The value is now in R8
        string offset = "r8";
        emitInstruction("mov " + var.getRegister8Size() + ", " + var.makeReferenceTo(offset));
        return;
    }
    

    // If it's a (non-indexed) string ref, move the address of the pointer to R8
    if (program->isStringVariable(var.identifier)) {
        emitInstruction("mov r8, " + var.identifier);
        return;
    }

    emitInstruction("mov " + var.getRegister8Size() + ", " + var.makeReferenceTo());

}

void NASM::handleBoolExpression(shared_ptr<GNODE> node) {

    // A wrapper for bool term(s)
    // If it's a single term, just execute it
    // If there's two terms, apply approriate boolean operator
    checkNullPtr(node->getLeft(),node);

    traverse(node->getLeft());

    if (!node->getRight()) {
        return;
    }
    emitInstruction("push r8");
    traverse(node->getRight());
    emitInstruction("pop r9");

    emitInstruction(node->op + " r8, r9");

}

void NASM::handleBoolTerm(shared_ptr<GNODE> node) {
    emitComment("bool-term started");

    string opInstruction = mapOperatorToInstruction(node->op);

    // Evaluate left expression; result is stored into r8
    traverse(node->getLeft());
    // Push result onto stack
    emitInstruction("push r8");
    // Eval right expression
    traverse(node->getRight());
    // Now we have values/pointers in R8 and R9
    emitInstruction("pop r9");
    
    // Check if we're comparing strings
    bool leftIsStr  = checkIfExpressionIsAString(node->getLeft());
    bool rightIsStr = checkIfExpressionIsAString(node->getRight());

    if (leftIsStr && rightIsStr) {
        // Strings need a bit of special interest
        // Reduce comparison to a single value with a helper function
        // R8 is a pointer to the beginning of *right* string of expr
        // R9 is a pointer to the beginnning of *left* string of expr
        emitComment("comparing strings!");
        doStringComparison(opInstruction);
        return;
    }

    string labelFalse = program->getNewLabel();
    emitInstruction("cmp r9, r8");
    emitInstruction("mov r8, 1");
    emitInstruction(opInstruction + " " + labelFalse);
    emitInstruction("mov r8, 0");
    emitInstruction(labelFalse + ":");
    emitComment("done with bool-term");
    // R8 = 1 if the expression is true, 0 if not
}

void NASM::handleConstant(shared_ptr<GNODE> node) {

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

void NASM::handleAssign(shared_ptr<GNODE> node) {
    string name = node->name;

    if (!program->inVariables(name)) {
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
        return;
    }

    traverse(left);
    Variable var = program->getVariable(name);

    string offset = "";
    if (right) { 
        // Indexed assignment. Pop the index from stack
        emitInstruction("pop r9");
        offset = "r9";
    }

    emitInstruction("mov " + var.makeReferenceTo(offset) + ", " + var.getRegister8Size());
}

void NASM::handleMathOperation(shared_ptr<GNODE> node, string op) {
    emitInstruction("push r8"); // Store result of previous node
    traverse(node);           // Content will be R8 after this
    
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

void NASM::handlePrintString(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    shared_ptr<GNODE> left = node->getLeft();

    if (left->getType() == "CONSTANT") {
                string value = left->value;
                string label = program->getNewLabel();
                emitInstruction("printBytes " + label + ", 0, " + to_string(left->value.length()));
                emitConstant(label, value, "str");
    }

    if (left->getType() == "VARIABLE") {

        Variable variable = program->getVariable(left->name);

        if (variable.type == "str") {

            string loopStart = program->getNewLabel();
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

void NASM::handlePrintInt(shared_ptr<GNODE> node) {
    // If 'name' is present, print a variable ref
    // if not, evaluate node on left
    if (node->name != "") {
        Variable var = program->getVariable(node->name);
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

void NASM::handleInput(shared_ptr<GNODE> node) {


    string bufferName = node->name;
    string labelInputLoop = program->getNewLabel();
    string labelLoopOut = program->getNewLabel();
    
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

void NASM::emitInstruction(string inst) {
    program->addInstruction(inst);
}

void NASM::emitConstant(string out, string value, string varType = "str") {
    program->addConstant(out, value, varType);
    
}

void NASM::emitComment(std::string comment) {
    emitInstruction("; " + comment);
}

void NASM::emitVariable(string name, string varType, string size, int length) {
    if (!program->inVariables(name)) {
            program->addVariable(name, varType, size, length);
    }
}

void NASM::error(string error_message) {
    throw std::runtime_error(error_message);
}

void NASM::checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from) {
    if (!node) {
        error("nullptr! Origin: " + from->getType());
    }
}

string NASM::mapOperatorToInstruction(string op) {

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

bool NASM::checkIfExpressionIsAString(shared_ptr<GNODE> node) {
    // Traverses the node to check if has child node type of 'str'
    if (node->getType() == "VARIABLE") {
        if (program->isStringVariable(node->name)) {
            return true;
        }
    }
    bool res = false;
    if (node->getLeft()) {
        res = checkIfExpressionIsAString(node->getLeft());
    }
    if (node->getRight()) {
        res = checkIfExpressionIsAString(node->getRight());
    }
    return res;
}

void NASM::doStringComparison(string op) {
    /*
        Loop through the strings until either reaches NULL or comparison operation is satisfied
        Operations:
            je  = Equal. Return false if any character is not equal
            jne = Not equal. Return true if any character is not equal
            jl  = A is before B in ASCII-order. Return true if any character of A is before B
            jg  = A is after B in ASCII-order. Return true if any character of B is before A
    
    */
    string loopStart    = program->getNewLabel();
    string loopEnd      = program->getNewLabel();
    string loopEndFalse = program->getNewLabel();
    string loopEndTrue  = program->getNewLabel();



    // Init loop counter
    emitInstruction("mov r12, 0");
    emitInstruction(loopStart + ":");

    // Use R11 and R12 as a temporary containers for operations
    // (could be done with just one of them though. both ops of cmp cannot be memory)

    emitInstruction("mov r10b, byte[r8 + r12]");
	emitInstruction("mov r11b, byte[r9 + r12]");
	emitInstruction("cmp r10b, r11b");
    // Operations start
    emitComment("op is " + op);
    if (op == "je") {  
        emitInstruction("jne " + loopEndFalse);
        emitInstruction("cmp r10b, 0");
	    emitInstruction("je " + loopEndTrue);

    } else if(op == "jne") {
        emitInstruction("jne " + loopEndTrue);
        emitInstruction("cmp r10b, 0");
	    emitInstruction("je " + loopEndFalse);

    } else if(op == "jl") {
        emitInstruction("jl " + loopEndFalse);
        emitInstruction("cmp r10b, r11b");
        emitInstruction("jg " + loopEndTrue);
        emitInstruction("cmp r10b, 0");
	    emitInstruction("je " + loopEndFalse);

    } else if(op == "jg") {
        emitInstruction("jg " + loopEndFalse);
        emitInstruction("cmp r10b, r11b");
        emitInstruction("jl " + loopEndTrue);
        emitInstruction("cmp r10b, 0");
	    emitInstruction("je " + loopEndFalse);

    }

    // Operations end
	emitInstruction("inc r12");
	emitInstruction("jmp " + loopStart);
	emitInstruction(loopEndTrue + ":");
	emitInstruction("mov r8,1");
	emitInstruction("jmp " + loopEnd);
	emitInstruction(loopEndFalse + ":");
	emitInstruction("mov r8, 0");
	emitInstruction(loopEnd + ":");


    /*
    emitInstruction("mov r10b, byte[r8 + r12]");
	emitInstruction("mov r11b, byte[r9 + r12]");
	emitInstruction("cmp r10b, r11b");
	emitInstruction("jne " + loopEndFalse);
	emitInstruction("cmp r10, 0");
	emitInstruction("je " + loopEndTrue);
	emitInstruction("inc r12");
	emitInstruction("jmp " + loopStart);
	emitInstruction(loopEndTrue + ":");
	emitInstruction("mov r8,1");
	emitInstruction("jmp " + loopEnd);
	emitInstruction(loopEndFalse + ":");
	emitInstruction("mov r8, 0");
	emitInstruction(loopEnd + ":");
    */

}