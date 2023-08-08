#include "./include/Javascript.h"

Javascript::Javascript() {
   tree = nullptr;
   wrapToAsyncFunction = false;
}

void Javascript::assemble(shared_ptr<GAST> p_tree) {


  
    // Init instruction set with a stack
    emitInstruction("const stack = [];");

    tree = p_tree;
    traverse(tree->getRoot());
    //buildProgram();

    if (wrapToAsyncFunction) {
        std::cout << "res = async function() {";
    }


    if (wrapToAsyncFunction) {
        emitInstruction("}();");
    }
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
        emitInstruction("console.log('')");
        return;
    }

    writeToStdout(node->value,true);
}

void Javascript::handleDeclare(shared_ptr<GNODE> node) {

    if (node->datatype == "int" && node->size > 1) {
        emitInstruction("var " + node->name + " = Array("+std::to_string(node->size)+");");
    } else {
        emitInstruction("var " + node->name + ";");
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

void Javascript::handleWhile(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    checkNullPtr(node->getRight(), node);

    // Left-child has the condition
    // Right-child has a linked list to the operations within the block
    emitInstruction("var whileR8;");
    traverse(node->getLeft());
    emitInstruction("whileR8 = stack.pop()");
    emitInstruction("while (whileR8)");
    
    emitInstruction("{");
        traverse(node->getRight());
        traverse(node->getLeft());
        emitInstruction("whileR8 = stack.pop()");
    emitInstruction("};");
  
}

void Javascript::handleFor(shared_ptr<GNODE> node) {

    // Right-child has a linked list to the operations within the block
    checkNullPtr(node->getRight(), node);

    string variableRef  = node->name;
    string step         = node->getFromInfo("step");
    string to           = node->getFromInfo("to");
    string toType       = node->getFromInfo("toType");


    emitInstruction("var tmp = " + node->name + ";");
    emitInstruction("for (let "+node->name+" =tmp; "+node->name+" < "+to+"; "+node->name+"+="+step+") {");
        traverse(node->getRight());
    emitInstruction("};"); 
  
}

void Javascript::handleIf(shared_ptr<GNODE> node) {
    checkNullPtr(node->getLeft(), node);
    checkNullPtr(node->getRight(), node);

    // Left-child has the condition. Traverse it to get the result onto stack
    traverse(node->getLeft());
    emitInstruction("if (stack.pop()) {");
    traverse(node->getRight()->getRight());
    // The code to be executed if the condition evaluates TRUE is 
    // wrapped in a nested GNODE. The right branch is executed if TRUE.
    // The left branch is taken if FALSE and IF-ELSE exists
    emitInstruction("} else {");
    if (node->getRight()->getLeft()) {
        traverse(node->getRight()->getLeft());
    }
    emitInstruction("}");
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
    // Moves a variable reference onto the stack

    // If there's a right branch, it's an indexed reference
    // The right branch has an expr that will evaluate to the index (usually an int constant)
    shared_ptr<GNODE> right = node->getRight();

    if (right) {
        // Store the index into a register (R9) and offset
        // the MOV-operation with the value in this register
        traverse(right);            // The value is now on the stack
        emitInstruction("stack.push(" + node->name +"[stack.pop()]);");
        return;
    }
    

    // Otherwise, push it onto the stack
    emitInstruction("stack.push(" + node->name + ")");

}

void Javascript::handleBoolExpression(shared_ptr<GNODE> node) {
    
    // A wrapper for bool term(s)
    // If it's a single term, just execute it
    // If there's two terms, apply approriate boolean operator
    checkNullPtr(node->getLeft(),node);

    traverse(node->getLeft());

    if (!node->getRight()) {
        return;
    }
    emitInstruction("var r9 = stack.pop();");
    traverse(node->getRight());
    emitInstruction("var r8 = stack.pop();");

    emitInstruction("stack.push(r8 " + node->op + " r9);");
}

void Javascript::handleBoolTerm(shared_ptr<GNODE> node) {
    emitComment("bool-term started");

    // Evaluate left expression; result is stored on the stack
    traverse(node->getLeft());
    // Eval right expression
    traverse(node->getRight());
    // Now we have values in R8 and R9
    emitInstruction("var r9 = stack.pop();");
    emitInstruction("var r8 = stack.pop();");


    emitInstruction("stack.push(r8 " + node->op + " r9);");
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

    // If it's an int:
    //  Evaluate the expression that is beign assigned, the result will be in r8
    // If it's a constant str:
    //  Handle pointer locally (do not traverse)
    shared_ptr<GNODE> left = node->getLeft();  

    if (left->getType() == "CONSTANT" & left->datatype == "str") {
        emitInstruction(name + " = '" + left->value + "';");
        return;
    }
   

    if (right) {
        // We store the index into a register (R9). Later on (see below)
        // Well offset the MOV-operation with the value in this register
        traverse(right);            // The value is now on the stack
        emitInstruction("var tmp = stack.pop();");
        traverse(left);
        emitInstruction(name + "[tmp] = stack.pop();");
        return;
    }

    traverse(left);
    emitInstruction(name + " = stack.pop();");
    
}

void Javascript::handleMathOperation(shared_ptr<GNODE> node, string op) {

    // Content of traversing will be on the stack after this
    traverse(node);           
    
    // Result of previous node is also on the stack so:

    if (op == "+" || op == "*") {
        // these are commutative so
        emitInstruction("stack.push( stack.pop() "  + op + " stack.pop() );");
        return;
    }

    if (op == "-" || op == "/") {
        emitInstruction("var r9 = stack.pop();");
        emitInstruction("var r8 = stack.pop();");
        emitInstruction("stack.push( r8 "  + op + " r9 );");
        return;
    }

    error("Don't know how to deal with operator: "+op+"\n");
    
}

void Javascript::handlePrintString(shared_ptr<GNODE> node) {

    checkNullPtr(node->getLeft(), node);
    shared_ptr<GNODE> left = node->getLeft();

    if (left->getType() == "CONSTANT") {
        handlePrintASCII(left);
        return;
    }

    if (left->getType() == "VARIABLE") {
        writeToStdout(left->name);
    }

}

void Javascript::handlePrintInt(shared_ptr<GNODE> node) {
    // If 'name' is present, print a variable ref
    // if not, evaluate node on left
    if (node->name != "") {
        writeToStdout(node->name);
        return;
    } 

    shared_ptr<GNODE> left = node->getLeft();
    
    if (!node) {
        error("Expected PrintInt to have a left node!\n");
    }
    handleNode(left);
    writeToStdout("stack.pop()");
}

void Javascript::handleInput(shared_ptr<GNODE> node) {

    wrapToAsyncFunction = true; // Need to wrap as async to utilize await

    string bufferName = node->name;

    emitInstruction("const readline = require('node:readline/promises').createInterface({ input: process.stdin, output: process.stdout, });");
    emitInstruction(node->name  + " = await readline.question('');");

}

void Javascript::emitInstruction(string inst) {
    instructions.push_back(inst);
}

void Javascript::emitComment(std::string comment) {
    emitInstruction("// " + comment);
}


void Javascript::error(string error_message) {
    throw std::runtime_error(error_message);
}

void Javascript::checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from) {
    if (!node) {
        error("nullptr! Origin: " + from->getType());
    }
}

void Javascript::writeToStdout(string tolog, bool quote) {

    if (quote) {
        emitInstruction("process.stdout.write('" + tolog + "');");
        return;
    }
    emitInstruction("process.stdout.write(String(" + tolog + "));");
}