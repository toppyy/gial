#include "./include/Parser.h"


Parser::Parser(Scanner & p_scanner, Program &p_program) : 
                                        program(p_program),
                                        labelCount(0),
                                        look(Token("EOF")),
                                        lookChar(0),
                                        tokens(std::vector<Token> {}),
                                        cursor(0),
                                        token_count(0)
                                        {
    tokens = p_scanner.getTokens();
    program = p_program;
    cursor = 0;
    token_count = tokens.size();

    keywords.insert("GUNNES");
    keywords.insert("GUHA");
    keywords.insert("OLGO");
    keywords.insert("TOIST");
    keywords.insert("SAN NY");
    keywords.insert("SAN JOTTAI");
    keywords.insert("SAN SNAA");
    keywords.insert("SAN LUGU");
}


void Parser::init() {

    getToken();
    block();
}

void Parser::mapStatementToFunction(std::string statement ) {
    if (statement == "SAN NY")  {
        printStatement();
        return;
    }

    if (statement == "SAN NY")  {
        printStatement();            
        return;
    }

    if (statement == "SAN JOTTAI") {
        emitLine();
        return;
    }

    if (statement == "SAN LUGU")  {
        printIntStatement();
        return;
    }

    if (statement == "SAN SNAA")  {
        inputStatement();
        return;
    }

    if (statement == "GUHA")  {
        ifStatement();
        return;
    }

    if (statement == "GUNNES")  {        
        whileStatement();
        return;
    }
        
    if (statement == "TOIST")  {
        repeatStatement();
        return;
    }

    if (statement == "OLGO") {
        letStatement();
        return;
    }


}


void Parser::block() {
    std::string nextToken = "?";
    std::string endToken = "NYLOPPUS";


    while (
        look.getContent() != "NYLOPPUS" &
        look.getContent() != "EOF" &
        cursor < (token_count-1)
    ) {
        nextToken = look.getContent();
        
        if (lookChar == 0) {
            getToken();
            continue;
        }

        if (keywords.count(nextToken) > 0) {
            matchToken(nextToken);
            mapStatementToFunction(nextToken);
            continue;
        }

        expression();
    }
    // std::cout << "exited block with " + look.getContent() + "\n"; // REMOVE
}

/* ---------- STATEMENTS ----------------------------------------------------------------------------------------------------------------  */
void Parser::printStatement() {
        

    if (lookChar == '\'') {
        
        matchToken("\'");
        std::string instr;
        
        std::string strToPrint = look.getContent();
        for (auto c: strToPrint) {
            instr = "mov rdi, '";
            instr.push_back(c);
            instr += "'";
            emitInstruction(instr);
            emitInstruction("call PrintASCII");            
        }
        getToken();
        matchToken("\'");
        return;
    }

    if (look.isName) {
        // it's a variable. Interpret content as ASCII-string
        // iterate until NULL
        
        std::string varname     = look.getContent();
        std::string loopStart   = getNewLabel();


        emitComment("Printing variable;");
        emitInstruction("mov r11, 0");
        
        emitInstruction(loopStart + ":");
        emitInstruction("push r11");
        emitInstruction("mov rdi, qword[" + varname + " + r11]");
        emitInstruction("call PrintASCII");
        emitInstruction("pop r11");
        emitInstruction("inc r11");
        emitInstruction("mov r8, qword[" + varname + " + r11]");
        emitInstruction("cmp r8, 0");
        emitInstruction("jne " + loopStart);

        getToken();
        return;
    }
    
    error("SANS NY expected a variable, character literal (or nothing). Got: " + look.getContent());
}


void Parser::printIntStatement() {
    
    if (look.isName) {
        // it's a variable. Interpret content as integer
        Name var = getName();
        emitInstruction("mov rdi, qword[" + var.getContent() + "]");
    } else {
        // it's an expression
        expression();
        emitInstruction("mov rdi, r8");
    }
    emitInstruction("call PrintInteger ");
    return;

}

void Parser::emitLine() {
    emitInstruction("mov dil, LF");
    emitInstruction("call PrintASCII");
    return;
}

void Parser::inputStatement() {
    
    Name varName = getName();
    std::string bufferName = varName.getContent();
    std::string labelInputLoop = getNewLabel();
    std::string labelLoopOut = getNewLabel();
    
    std::string bufferRef = "byte[" + bufferName + " + r9]";

    emitVariable(bufferName, 100);                  // Reserve space for input buffer (100 bytes)
    
    emitInstruction("mov r9, 0");                   // Init character count -loop
    emitInstruction(labelInputLoop + ":");          // Loop to read characters one-by-one from input
    emitInstruction("mov rax, SYS_READ");           // Specify system call
    emitInstruction("mov rdi, STDIN");              // Reading from standard input
    emitInstruction("lea rsi, " + bufferRef);       // Specify address of the space reserved for input
    emitInstruction("mov rdx, 1");                  // Read one byte (=character) at a time
    emitInstruction("syscall"); 

    emitInstruction("cmp " + bufferRef + ", LF");   // Is the input character LF?
    emitInstruction("je " + labelLoopOut);          //  If yes, stop reading by jumping out loop
    emitInstruction("inc r9");                      //  If no, increment character count
    emitInstruction("jmp " + labelInputLoop);       // And jump back to reading another character

    emitInstruction(labelLoopOut + ":");
    emitInstruction("inc r9");
    emitInstruction("mov " + bufferRef + ", 0");     // Null-terminate the string


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
    matchEndStatement();
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
    matchEndStatement();
}

void Parser::letStatement() {
    Name varName = getName();
    emitVariable(varName.getContent(), 100);

}


void Parser::repeatStatement() {
    // implements
    //  REPEAT n
    //      [code to be repeated n times]
    //  ENDREPEAT
    //

    // n is either a integer or a variable ref
    std::string n;
    if (look.isNumber) {
       n = look.getContent();
    }  else if (look.isName) {
       n =  "qword[" + look.getContent() + "]";
    } else {
        error("Expected a number or a variable name after TOIST. Got: " + look.getContent());
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
    matchEndStatement();
}



/* ------- END STATEMENTS ----------------------------------------------------------------------------------------------------------------  */


std::string Parser::getNewLabel() {
    labelCount += 2;
    return "LBL_" + std::to_string(labelCount);

}

void Parser::emitInstruction(std::string out) {
    program.addInstruction(out);
}

void Parser::emitVariable(std::string out, int bytes = 1) {

    std::string variableDeclaration = out + " resq "; // Every variable is 64 bits :)
    variableDeclaration  += std::to_string(bytes);
    
    // Terrible way to check if variable has been declared;
    // TODO: This does not protect agaings declaring the same variable with different length
    if (!program.inVariables(variableDeclaration)) {
        program.addVariable(variableDeclaration);
    }
    
}




void Parser::emitComment(std::string comment) {
    emitInstruction("; " + comment);
}

void Parser::error(std::string error_message) {
    throw std::runtime_error(error_message);
}

void Parser::expected(std::string expected_thing) {
    error("Expected: " + expected_thing + "\n");
    return;
}


void Parser::getToken() {
    if (cursor >= token_count) {
        std::string err_msg = "No more tokens!\n";
        error(err_msg);
    }
    look = tokens[cursor];
    lookChar = look.getCharFromContent(0);
    cursor++;
}

Name Parser::getName() {
    if (look.isName) {
        Name rtrn = Name(look.getContent()); // Stupid conversion
        getToken();
        return rtrn;
        
    }
    std::string err_msg = "Expected to find a 'Name'!\n";
    error(err_msg);
    return Name("");
}


void Parser::matchToken(std::string expected_content) {

    if (expected_content == look.getContent()) {        
        getToken();
        return;
    }
    std::string msg = "character ";
    msg += expected_content;
    msg += ", got : " + look.getContent();
    expected(msg);
}

void Parser::matchEndStatement() {
    matchToken("NYLOPPUS");
}


/* ----------------------- PARSING EXPRESSION ---------------------------- */

void Parser::expression() {
    // Trick: Add a zero to start calculations with
    // This way -3 + 3 becomes 0 - 3 + 3    
    
    if ( isAddOp(lookChar)) {
        emitInstruction("mov r8, 0");
    } else {
        term();
    }

    
    while (isAddOp(lookChar)) {
        emitInstruction("push r8");
        if (lookChar == '+') {
            add();
        } else if (lookChar == '-') {
            minus();
        }
    }
    // std::cout << "Returning expression \\w content " + look.getContent() + ", ";
    // printf(" lookchar %d, %c\n", lookChar, lookChar); // REMOVE

}   



void Parser::term() {
    factor();
    while (lookChar == '*' | lookChar == '/') {
        emitInstruction("push r8");
        if (lookChar == '*') {
            multiply();
        } else if (lookChar == '/') {
            divide();
        }
    }
}

void Parser::factor() {
    // printf("Factor %d, %c\n", lookChar, lookChar); // REMOVE
    
    // If an opening parenthesis is met, start an another expression recursively
    // This works because nothing is emitted before the 'inner most' parenthesis have been met
    if (lookChar == '(') {
        matchToken("(");
        expression();
        matchToken(")");
        return;
    }
    
    std::string instr;

    if (isAlpha(lookChar)) {
        ident(); // Checks if it's a function or a variable name
        return;
    }

    
    Token nextToken = look;
    if (isDigit(lookChar)) {
        instr = "mov r8, " + nextToken.getContent();
        emitInstruction(instr);
        getToken();
        return;
    }

    if (lookChar == 0) {
        return;
    }


    error("Should have not reached token \\w content " + nextToken.getContent());
    return;
}   




void Parser::ident() {

    Name name = getName();
    std::string instr;

    // std::cout << "ident: " + name.getContent() + ", "; // REMOVE
    // printf("look is %d, %c\n", lookChar, lookChar);    // REMOVE

    // check if the name is a name of a function (not a variable)
    if (lookChar == '(') {
        matchToken("(");
        matchToken(")"); // Currently can match only empty argument lists
        instr = "call " + name.getContent();
        emitInstruction(instr);
        return;
    }

    // ATM variables are introduced by assignment only, but no error is thrown here if 
    // a non-declared variable is used
    if (lookChar == '=') {
        // Check if this is actually a condition, not an assignment. If a condition, do nothing.
        if ( look.getContent() == "==") {
            return;
        }
        assignment(name);
        return;
    }
    instr = "mov r8, qword[" + name.getContent() + "]";
    emitInstruction(instr);
    return;
}

void Parser::assignment(Name name) {

    matchToken("=");


    expression();

    // std::cout << "assignemnt: " + name.getContent() + ", "; // REMOVE
    // printf("look is %d, %c\n", lookChar, lookChar);    // REMOVE


    emitVariable(name.getContent());

    // Instruction to assign r8 to variable
    emitInstruction( "mov qword[" + name.getContent() + "], r8" );
}

bool Parser::isAddOp(char x) {
    return x == '+' | x == '-';
}

bool Parser::isAlphaNumeric(char x) {
    return isAlpha(x) | isDigit(x);
}


void Parser::add() {
    matchToken("+");
    term();
    emitInstruction("pop r9");
    emitInstruction("add r8, r9");
}   

void Parser::minus() {
    matchToken("-");
    term();
    emitInstruction("pop r9");
    emitInstruction("sub r9, r8");
    emitInstruction("mov r8, r9");
}

void Parser::multiply() {
    matchToken("*");
    factor();                 // Moves multiplier to register (r8)
    emitInstruction("pop rax");      // The other operand is now in rax
    emitInstruction("imul rax, r8"); // (signed) multiplication
    emitInstruction("mov r8, rax");  // Store result into r8 like all calculations
}

void Parser::divide() {
    matchToken("/");                // Consume the division character. 'look' now has the divisor
    factor();                       // Moves divisor to register (r8)
    emitInstruction("pop rax");    // Pop the value we want to divided from stack (stack uses 64 bits, so we must use rax)
    emitInstruction("cdq");        // convert qword in eax to qword in edx:eax
    emitInstruction("idiv r8");    // divisor. Now rax has the quotient. We forget the remainder
    emitInstruction("mov r8, rax");// Store quotient into r8 like all calculations
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

/* ------------------------ Parse boolean expressions -------------------------------------------------------- */

void Parser::boolExpression() {
    // Try to parse a boolean expression
    // boolExpression = (boolTerm operator boolTerm)
    // but recursive: so (boolExpression operator boolTerm) is also valid
    if (lookChar == '(') {
        matchToken("(");
        boolExpression();
        matchToken(")");
        
    } else {
        boolTerm();
    }
    std::string nextTokenContent = look.getContent();
    while (
        nextTokenContent == "AND" |
        nextTokenContent == "OR"
    ) {
        
        matchToken(nextTokenContent); // eat the token
        emitInstruction("push r8");
        boolExpression();
        emitInstruction("pop r9");
        
        if (nextTokenContent == "AND") {
            emitInstruction("and r8, r9");
        }
        if (nextTokenContent == "OR") {
            emitInstruction("or r8, r9");
        }
        nextTokenContent = look.getContent();
    }
}

void Parser::boolTerm() {   
    // std::cout << "boolterm: " + look.getContent() + ", "; // REMOVE
    // printf("look is %d, %c\n", lookChar, lookChar);    // REMOVE

    expression();
    emitInstruction("push r8");
    std::string op = mapOperatorToInstruction();
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




std::string Parser::mapOperatorToInstruction() {
    // TODO: all fails should throw with list of accepted operators

    std::string lookOp = look.getContent();
    // ==
    if (lookOp == "==") {
        matchToken("==");
        return "jne";
    }

    // != 
    if (lookOp == "!=") {
        matchToken("!=");
        return "je";
    }

    // these are sort of backwards, but we use the comparison
    // to jump in case of FALSE. So we want an operator
    // that is a negation of the operator specified in the program.

    // >= or >
    if (lookOp == ">") {
        matchToken(">");
        return "jle"; 
    }


    if (lookOp == ">=") {
        matchToken(">=");
        return "jl";
    }
    
    // <= or <
    if (lookOp == "<") {
        matchToken("<");
        return "jge"; 
    }


    if (lookOp == "<=") {
        matchToken("<=");
        return "jg";
    }


    expected(" ==, !=, >=, <=, <, <, got: " + look.getContent());
    return ""; // Unreachable
}
