#include "./include/Parser.h"

Parser::Parser(std::string p_content, Program &p_program) : program(p_program)  {
    content = p_content;
    program = p_program;
    cursor = 0;
    cursor_max = p_content.length();
}

bool Parser::parsingToBeDone() {
    return cursor < cursor_max;
}

void Parser::incrementCursor() {
    cursor += 1;
}

void Parser::init() {
    getChar();
    while (parsingToBeDone()) {
        expression();
    }
}

void Parser::getChar() {
    // We need to allow going over the string buffer by one byte
    // The logic looks 'ahead' one character
    if (cursor >= cursor_max + 1) {
        std::string err_msg = "Unexpected end!\n";
        error(err_msg);
        
    }
    look = content[cursor];
    cursor++;    
}

void Parser::printLookInfo() {
    printf("look is %c. Content is %c, cursor is %d. Content length: %d.\n", look, content[cursor],cursor, cursor_max);
}

void Parser::error(std::string error_message) {
    throw std::runtime_error(error_message);
}

void Parser::expected(std::string expected_thing) {
    error("Expected: " + expected_thing + "\n");
}

void Parser::match(char x) {
    if (x == look) {
        getChar();
        return;
    }
    expected(std::string("" + look)); // TODO:
}

bool Parser::isAlpha(char x) {
    return 
        (x >= 65 & x <= 90) // A-Z
        |
        (x >= 97 & x <= 122); // a-z
}


bool Parser::isDigit(char x) {
    return x >= 48 & x <= 57; // 0-9
}

std::string Parser::getName() {

    std::string token = "";
    
    if (!isAlpha(look)) {
        expected("Name");
    }

    while (isAlphaNumeric(look)) {
        token.push_back(look);
        getChar();
    }

    return token;
}


std::string Parser::getNum() {
    std::string value = "";
    if (!isDigit(look)) {
        expected("Integer");
    }
    while (isDigit(look)) {
        value.push_back(look);
        getChar();
    }
    return value;
}

void Parser::emitVariable(std::string out) {

    std::string variableDeclaration = out + " resq 1"; // Every variable is 64 bits :)
    program.addVariable(variableDeclaration);
}


void Parser::emitInstruction(std::string out) {
    program.addInstruction(out);
}


void Parser::factor() {
    // If an opening parenthesis is met, start an another expression recursively
    // This works because nothing is emitted before the 'inner most' parenthesis have been met
    if (look == '(') {
        match('(');
        expression();
        match(')');
        return;
    }
    
    std::string instr;

    if (isAlpha(look)) {
        ident(); // Checks if it's a function or a variable name
        return;
    }
    instr = "mov r8, " + getNum();
    emitInstruction(instr);
}   

void Parser::term() {
    factor();
    while (look == '*' | look == '/') {
        emitInstruction("push r8");
        if (look == '*') {
            multiply();
        } else if (look == '/') {
            divide();
        }
    }
}

void Parser::expression() {
    // Trick: Add a zero to start calculations with
    // This way -3 + 3 becomes 0 - 3 + 3    
    if (isAddOp(look)) {
        emitInstruction("mov r8, 0");
    } else {
        term();
    }

    
    while (isAddOp(look)) {
        emitInstruction("push r8");
        if (look == '+') {
            add();
        } else if (look == '-') {
            minus();
        }
    }
}   


void Parser::ident() {
    // short for 'identity'?
    std::string name = getName();
    std::string instr;

    // check if the name is a name of a function (not a variable)
    if (look == '(') {
        match('(');
        match(')'); // Currently can match only empty argument lists
        instr = "call " + name;
        emitInstruction(instr);
        return;
    }

    // ATM variables are introduced by assignment only, but no error is thrown here if 
    // a non-declared variable is used
    if (look == '=') {
        assignment(name);
        return;
    }
    instr = "mov r8, qword[" + name + "]";
    emitInstruction(instr);
    return;
}

void Parser::assignment(std::string name) {

    match('=');
    expression();
    emitVariable(name);

    // Instruction to assign r8 to variable
    emitInstruction( "mov qword[" + name + "], r8" );
}

bool Parser::isAddOp(char x) {
    return x == '+' | x == '-';
}

bool Parser::isAlphaNumeric(char x) {
    return isAlpha(x) | isDigit(x);
}

void Parser::add() {
    match('+');
    term();
    emitInstruction("pop r9");
    emitInstruction("add r8, r9");
}   

void Parser::minus() {
    match('-');
    term();
    emitInstruction("pop r9");
    emitInstruction("sub r9, r8");
    emitInstruction("mov r8, r9");
}

void Parser::multiply() {
    match('*');
    factor();                 // Moves multiplier to register (r8)
    emitInstruction("pop rax");      // The other operand is now in rax
    emitInstruction("imul rax, r8"); // (signed) multiplication
    emitInstruction("mov r8, rax");  // Store result into r8 like all calculations
}

void Parser::divide() {
    match('/');             // Consume the division character. 'look' now has the divisor
    factor();               // Moves divisor to register (r8)
    emitInstruction("pop rax");    // Pop the value we want to divided from stack (stack uses 64 bits, so we must use rax)
    emitInstruction("cdq");        // convert qword in eax to qword in edx:eax
    emitInstruction("idiv r8");    // divisor. Now rax has the quotient. We forget the remainder
    emitInstruction("mov r8, rax");// Store quotient into r8 like all calculations
}





