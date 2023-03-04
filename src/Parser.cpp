#include "./include/Parser.h"

Parser::Parser(std::string p_content, Program &p_program) : program(p_program), labelCount(0) {
    content = p_content;
    program = p_program;
    cursor = 0;
    cursor_max = p_content.length();
}


void Parser::init() {

    skipEOL();
    getChar();
    skipWhite();
    block();
}

void Parser::block() {

    std::string nextToken = "?";
    std::string endToken = "NYLOPPUS";

    while (
        !isNextToken(endToken) &
        look != EOF
    ) {
        nextToken = lookaheadToken();

        if (nextToken == "GUHA")  {
            matchString(nextToken);
            ifStatement();
            matchString(endToken);
            continue;
        }
        if (nextToken == "GUNNES")  {
            matchString(nextToken);
            whileStatement();
            matchString(endToken);
            continue;;
        }
        if (nextToken == "TOIST")  {
            matchString(nextToken);
            repeatStatement();
            matchString(endToken);
            continue;
        }
        if (nextToken == "SANS")  {
            matchString(nextToken);
            printStatement();
            continue;
        }
        if (nextToken == "PRINTINT")  {
            matchString(nextToken);
            printIntStatement();
            continue;
        }
        expression();
    }
}

bool Parser::isNextToken(std::string keyword) {
    return keyword == lookaheadToken();
}

void Parser::printLine() {
    emitInstruction("mov dil, LF");
    emitInstruction("call PrintASCII");
    return;
}

void Parser::printIntStatement() {
    
    if (isAlpha(look)) {
        // it's a variable. Interpret content as integer
        std::string varname = getName();
        emitInstruction("mov rdi, qword[" + varname + "]");
    } else {
        // it's an expression
        expression();
        emitInstruction("mov rdi, r8");
    }
    emitInstruction("call PrintInteger ");
    return;

}

void Parser::printStatement() {
    
    if (isEOL(look)) {
        // just 'PRINT' prints a new line
        getChar();
        printLine();
        return;
    }

    if (look == '\'') {
        match('\'');
        std::string instr;
        while(look != '\'') {
            instr = "mov rdi, '";
            instr.push_back(look);
            instr += "'";
            emitInstruction(instr);
            emitInstruction("call PrintASCII");
            getChar();
        }
        match('\'');
        return;
    }

    if (isAlpha(look)) {
        // it's a variable. Interpret content as integer
        std::string varname = getName();
        emitInstruction("mov rdi, qword[" + varname + "]");
        emitInstruction("call PrintASCII");
        return;
    }
    
    expected("SANS expected a variable, character literal (or nothing).");
}


void Parser::repeatStatement() {
    // implements
    //  REPEAT n
    //      [code to be repeated n times]
    //  ENDREPEAT
    //

    // n is either a integer or a variable ref
    std::string n;
    if (isDigit(look)) {
       n = getNum();
    }  else {
       n =  "qword[" + getName() + "]";
    }

    std::string labelRepetition  = getNewLabel();

    emitInstruction("mov r8, 0");   // Init counter
    emitInstruction(labelRepetition + ":");
    emitInstruction("push r8");     // Push counter onto stack
    
    block();

    emitInstruction("pop r8");                 // Pop counter from stack
    emitInstruction("inc r8");                 // Increment counter from stack
    emitInstruction("cmp r8, " + n);           // Compare counter to n
    emitInstruction("jl " + labelRepetition);  // If it's less than n, jmp to labelTrue

}



void Parser::ifStatement() {
    std::string labelFalse = getNewLabel();
    // Sets r8 to 0/1 depending on the evaluation
    boolExpression();
    // Compare it and determine whether to run 'block'
    emitInstruction("cmp r8, 1");
    emitInstruction("jne " + labelFalse); // jump to 'labelFalse' if evaluates to false
    block();
    emitInstruction(labelFalse + ":");
   
}

void Parser::whileStatement() {
    std::string labelFalse = getNewLabel();
    std::string labelTrue  = getNewLabel();
    
    emitInstruction(labelTrue + ":");
    // Sets r8 to 0/1 depending on the evaluation
    boolExpression();
    
    // Compare it and determine whether to run 'block'
    emitInstruction("cmp r8, 1");
    // jump to 'labelFalse' if evaluates to false
    emitInstruction("jne " + labelFalse); 
    block();
    emitInstruction("jmp " + labelTrue);
    emitInstruction(labelFalse + ":");
}


void Parser::boolExpression() {
    // Try to parse a boolean expression
    // boolExpression = (boolTerm operator boolTerm)
    // but recursive: so (boolExpression operator boolTerm) is also valid
    if (look == '(') {
        match('(');
        boolExpression();
        match(')');
        
    } else {
        boolTerm();
    }
    std::string nextToken = lookaheadToken();
    while (
        nextToken == "AND" |
        nextToken == "OR"
    ) {
        
        matchString(nextToken); // eat the token
        emitInstruction("push r8");
        boolExpression();
        emitInstruction("pop r9");
        
        if (nextToken == "AND") {
            emitInstruction("and r8, r9");
        }
        if (nextToken == "OR") {
            emitInstruction("or r8, r9");
        }
        nextToken = lookaheadToken();
    }
}

void Parser::skipEOL() {
    while(isEOL(look)) {
        getChar();
    }
}

void Parser::boolTerm() {   
    expression();
    emitInstruction("push r8");
    std::string op = logoperator();
    expression();
    emitInstruction("pop r9");
    emitInstruction("cmp r9, r8");
    std::string labelFalse = getNewLabel();
    emitInstruction("mov r8, 0");
    emitInstruction(op + " " + labelFalse);
    emitInstruction("mov r8, 1");
    emitInstruction(labelFalse + ":");
    // R8 is now 0/1 depending on the comparison
}




std::string Parser::logoperator() {
    // TODO: all fails should throw with list of accepted operators

    // ==
    if (look == '=') {
        match('=');
        match('=');
        return "jne";
    }

    // != 
    if (look == '!') {
        match('!');
        match('=');
        return "je";
    }

    // these are sort of backwards, but we use the comparison
    // to jump in case of FALSE. So we want an operator
    // that is a negation of the operator specified in the program.

    // >= or >
    if (look == '>') {
        match('>');
        if (look == '=') {
            match('=');
            return "jl";
        }
        return "jle"; 
    }
    
    // <= or <
    if (look == '<') {
        match('<');
        if (look == '=') {
            match('=');
            return "jg";
        }
        return "jge";
    } 
    

    expected(" ==, !=, >=, <=");
    return ""; // Unreachable
}

std::string Parser::lookaheadToken() {
    // Looks ahead until a white space or end of cursor is met
    std::string token = "";
    int lookCursor = cursor - 1;
    while (
        isAlphaNumeric(content[lookCursor])
        //lookCursor < cursor_max &
        //!isWhite(content[lookCursor]) &
        //content[lookCursor] != 10       // 10 = LF
        //content[lookCursor] >= 65     &
        //content[lookCursor] >= 90
    ) {
        token.push_back(content[lookCursor]);
        lookCursor++;
    }
    return token;
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

void Parser::printLookInfo(char info) {

    char contentAtm =  content[cursor];
    if (contentAtm ==10 | contentAtm == 13 ) {
        contentAtm = '?'; // Don't want to put out newline
    }
    char lookAtm = look;
    if (lookAtm ==10 | lookAtm == 13 | lookAtm == 32) {
        lookAtm= '?'; // Don't want to put out newline
    }
    printf("; %d -> look is %c (%d). Content is %c (%d), cursor is %d. Content length: %d.\n", info, lookAtm, look, contentAtm, content[cursor] ,cursor, cursor_max);
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
        skipWhite();
        return;
    }
    std::string msg = "character ";
    msg.push_back(x);
    msg += ", got : ";
    msg.push_back(look);
    expected(msg);
}

void Parser::matchString(std::string str) {
    for (auto c: str) {
        match(c);
    }
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
    skipWhite();

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
    skipWhite();
    return value;
}

void Parser::emitVariable(std::string out) {

    // Terrible way to check if variable has been declared;
    std::string variableDeclaration = out + " resq 1"; // Every variable is 64 bits :)
    if (!program.inVariables(variableDeclaration)) {
        program.addVariable(variableDeclaration);
    }
    
}


void Parser::emitInstruction(std::string out) {
    program.addInstruction(out);
}


bool Parser::isEOL(char x) {
    return x == 10;
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

    if (isEOL(look)) {
        match(10);
        return;
    }
    if (isDigit(look)) {
        instr = "mov r8, " + getNum();
        emitInstruction(instr);
        return;
    }
    getChar(); // ?
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
        skipWhite(); // All terms start with a non-white character
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
        // Check if this is actually a condition, not an assignment. If a condition, do nothing.
        // We don't want to consume anything, so don't use match, but look-a-head next token
        if (isNextToken("==")) {
            return;
        }
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

bool Parser::isWhite(char x) {
    return x == ' ' | x == 9 | x == 32; // 9 = TAB
}
void Parser::skipWhite() {
    while (isWhite(look)) {
        getChar();
    }
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

std::string Parser::getNewLabel() {
    labelCount += 2;
    return "LBL_" + std::to_string(labelCount);

}


void Parser::asmComment(std::string comment) {
    emitInstruction("; " + comment);
}
