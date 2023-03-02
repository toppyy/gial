#include "./include/Parser.h"

Parser::Parser(std::string p_content) {
    content = p_content;
    cursor = 0;
    cursor_max = p_content.length();
}

void Parser::init() {
    getChar();
}

void Parser::getChar() {
    // We need to allow going over the string buffer
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


void Parser::emit(std::string out) {
    std::cout << out;
}

void Parser::emitLine(std::string out) {
    emit(out);
    std::cout << "\n";
}


void Parser::factor() {
    std::string instr = "mov r8, ";
    instr.push_back(getNum());
    emitLine(instr);
}   

void Parser::term() {
    factor();
    while (look == '*' | look == '/') {
        emitLine("push r8");
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
   term();
    while (look == '+' | look == '-') {
        emitLine("push r8");
        if (look == '+') {
            add();
        } else if (look == '-') {
            minus();
        } else {
            expected("Addop");
        }
    }
}   

void Parser::add() {
    match('+');
    term();
    emitLine("pop r9");
    emitLine("add r8, r9");
}   

void Parser::minus() {
    match('-');
    term();
    emitLine("pop r9");
    emitLine("sub r9, r8");
    emitLine("mov r8, r9");
}

void Parser::multiply() {
    match('*');
    factor();                 // Moves multiplier to register (r8)
    emitLine("pop rax");      // The other operand is now in rax
    emitLine("imul rax, r8"); // (signed) multiplication
    emitLine("mov r8, rax");  // Store result into r8 like all calculations
}

void Parser::divide() {

    match('/');             // Consume the division character. 'look' now has the divisor
    factor();               // Moves divisor to register (r8)
    emitLine("pop rax");    // Pop the value we want to divided from stack (stack uses 64 bits, so we must use rax)
    emitLine("cdq");        // convert dword in eax to qword in edx:eax
    emitLine("idiv r8");    // divisor. Now rax has the quotient. We forget the remainder
    emitLine("mov r8, rax");// Store quotient into r8 like all calculations

}





