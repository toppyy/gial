#include "./include/Parser.h"

Parser::Parser(std::string p_content, Program &p_program) : program(p_program)  {
    content = p_content;
    program = p_program;
    cursor = 0;
    cursor_max = p_content.length();
}

void Parser::init() {
    getChar();
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
    printf("look is %c. Content is %c, cursor is %d.\n", look, content[cursor],cursor);
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

char Parser::getName() {
    if (!isAlpha(look)) {
        expected("Name");
    }
    char rtrn = look;
    getChar();
    return rtrn;
}


char Parser::getNum() {
    if (!isDigit(look)) {
        expected("Integer");
    }
    char rtrn = look;
    getChar();
    return rtrn;
}

void Parser::emitVariable(std::string out) {
    program.addVariable(out);
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
        instr = "mov ";
        instr.push_back(getName());
        instr += "0, r8";
        emitInstruction(instr);
        return;
    }
    instr = "mov r8, ";
    instr.push_back(getNum());
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
        } else {
            expected("Mulop");
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
        } else {
            expected("Addop");
        }
    }
}   

bool Parser::isAddOp(char x) {
    return x == '+' | x == '-';
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
    emitInstruction("cdq");        // convert dword in eax to qword in edx:eax
    emitInstruction("idiv r8");    // divisor. Now rax has the quotient. We forget the remainder
    emitInstruction("mov r8, rax");// Store quotient into r8 like all calculations
}





