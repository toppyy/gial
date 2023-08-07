#include "./include/Javascript.h"

Javascript::Javascript() {
   tree = nullptr;
}

void Javascript::assemble(shared_ptr<GAST> p_tree) {


    // Init instruction set with a stack
    emitInstruction("const stack = [];");

    tree = p_tree;
    traverse(tree->getRoot());
    //buildProgram();

    for (auto instr: instructions) {
        std::cout << instr << "\n";
    }

}

void Javascript::traverse(shared_ptr<GNODE> node) {

    handleNode(node);

    shared_ptr<GNODE> next  = node->getNext();

    if (next != nullptr) {
        traverse(next);
    }

    /* Note: left- and rightbranches are always traversed inside the node-handlers if need be.  */
} 

void Javascript::handleNode(shared_ptr<GNODE> node) {

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

void Javascript::handleBlock(shared_ptr<GNODE> node) {
    // Empty on purpose. Just a container for tree manipulations    
}

void Javascript::handlePrintASCII(shared_ptr<GNODE> node) {
    if (node->value == "LF") {
        // console.log includes LF
        return;
    }
    consoleLog(node->value,true);
}

void Javascript::handleDeclare(shared_ptr<GNODE> node) {
    
    emitInstruction("let " + node->name + ";");

    shared_ptr<GNODE> left = node->getLeft();
    if (left) {
        
        if (left->getType() != "ASSIGN") {
            error("Declare expected assignment, got " + left->getType());
        }
        checkNullPtr(node->getLeft(), node);
        handleAssign(node->getLeft());
    }
}

void Javascript::handleWhile(shared_ptr<GNODE> node) {


  
}

void Javascript::handleFor(shared_ptr<GNODE> node) {

  
}

void Javascript::handleIf(shared_ptr<GNODE> node) {

}

void Javascript::handleExpression(shared_ptr<GNODE> node) {
    
    checkNullPtr(node->getLeft(), node);
    traverse(node->getLeft());

    if (node->hasMathOperator()) {
        checkNullPtr(node->getRight(), node);
        handleMathOperation(node->getRight(), node->op);
    }
}

void Javascript::handleVariable(shared_ptr<GNODE> node) {
    
}

void Javascript::handleBoolExpression(shared_ptr<GNODE> node) {


}

void Javascript::handleBoolTerm(shared_ptr<GNODE> node) {
    
}

void Javascript::handleConstant(shared_ptr<GNODE> node) {

    if (node->datatype == "str") {
        error("Dont know how to handle str-constant atm");
        return;
    }

   
    emitInstruction("stack.push(" + node->value + ");");

}

void Javascript::handleAssign(shared_ptr<GNODE> node) {
    string name = node->name;

    // if (!program->inVariables(name)) {
    //     error("Assignment to an undeclared variable (" + name + ")");
    // }
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
        emitInstruction(name + " = '" + left->value + "';");
        return;
    }
   
    traverse(left);
    emitInstruction(name + " = stack.pop();");

    // string offset = "";
    // if (right) { 
    //     // Indexed assignment. Pop the index from stack
    //     emitInstruction("pop r9");
    //     offset = "r9";
    // }

    
}

void Javascript::handleMathOperation(shared_ptr<GNODE> node, string op) {

    
    traverse(node);           // Content will be on the stack after this
    
    // Result of previous node is also on the stack so:
    emitInstruction("stack.push( stack.pop() "  + op + " stack.pop() );");
}

void Javascript::handlePrintString(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    shared_ptr<GNODE> left = node->getLeft();

    if (left->getType() == "CONSTANT") {
        handlePrintASCII(left);
        return;
    }

    if (left->getType() == "VARIABLE") {
        consoleLog(left->name);
    }

}

void Javascript::handlePrintInt(shared_ptr<GNODE> node) {
    // If 'name' is present, print a variable ref
    // if not, evaluate node on left
    if (node->name != "") {
        consoleLog(node->name);
        return;
    } 
    
    
    shared_ptr<GNODE> left = node->getLeft();

    if (left->getType() == "VARIABLE") {
        consoleLog(left->name);
        return;
    }
    
    if (!node) {
        error("Expected PrintInt to have a left node!\n");
    }
    handleNode(left);
    consoleLog("stack.pop()");
}

void Javascript::handleInput(shared_ptr<GNODE> node) {



}

void Javascript::emitInstruction(string inst) {
    instructions.push_back(inst);
}

void Javascript::emitConstant(string out, string value, string varType = "str") {
    
    
}

void Javascript::emitComment(std::string comment) {
    emitInstruction("// " + comment);
}

void Javascript::emitVariable(string name, string varType, string size, int length) {

}

void Javascript::error(string error_message) {
    throw std::runtime_error(error_message);
}

void Javascript::checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from) {
    if (!node) {
        error("nullptr! Origin: " + from->getType());
    }
}

string Javascript::mapOperatorToInstruction(string op) {

    return "";
}

bool Javascript::checkIfExpressionIsAString(shared_ptr<GNODE> node) {
   
    return true;
}

void Javascript::doStringComparison(string op) {
    

}

void Javascript::consoleLog(string tolog, bool quote) {

    if (quote) {
        emitInstruction("console.log('" + tolog + "');");
        return;
    }
    emitInstruction("console.log(" + tolog + ");");
}