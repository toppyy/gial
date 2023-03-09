#include "./include/Parser.h"


Parser::Parser(std::vector<Token> p_tokens, std::set<std::string> p_keywords) : 
                                        labelCount(0),
                                        look(Token("EOF")),
                                        lookChar(0),
                                        tokens(std::vector<Token> {}),
                                        cursor(0),
                                        token_count(0)
                                        {
    tokens = p_tokens;
    program = Program();
    cursor = 0;
    token_count = tokens.size();
    keywords = p_keywords;

}

void Parser::buildProgram() {
    program.buildProgram();
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

    if (statement == "SANAMBATK") {
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
        

    if (look.isString) {
        // Is a string constant
        std::string instr;
        for (auto c: look.getContent()) {
            // 
            instr = "mov rdi, '";
            instr.push_back(c);
            instr += "'";
            emitInstruction(instr);
            emitInstruction("call PrintASCII");            
        }

        getToken();
        return;
    }

    if (look.isName) {
        // it's a variable. Interpret content as ASCII-string
        // iterate until NULL
        
        std::string varname     = look.getContent();
        std::string loopStart   = getNewLabel();

        emitInstruction("mov r11, 0");
        
        emitInstruction(loopStart + ":");
        emitInstruction("push r11");
        emitInstruction("mov dil, byte[" + varname + " + r11]");
        emitInstruction("call PrintASCII");
        emitInstruction("pop r11");
        emitInstruction("inc r11");
        emitInstruction("mov r8b, byte[" + varname + " + r11]");
        emitInstruction("cmp r8b, 0");
        emitInstruction("jne " + loopStart);

        getToken();
        return;
    }
    
    error("SANS NY expected a variable, character literal (or nothing). Got: " + look.getContent());
}


void Parser::printIntStatement() {

    
    if (look.isName) {
        // it's a variable. 
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

    bool integerInput = false;
    std::string varType = "str";
    int varSize = 100;

    if (look.getContent() == "LUGU") {
        integerInput = true;
        varType = "int";
        varSize = 8;
        getToken();
    }

    if (integerInput) {
        // For converting the ASCII-representation to int
        emitInstruction("mov r8, 0");   // Intermediate result
        emitInstruction("push r8");
    }

    emitVariable(bufferName, varSize, varType);     // Reserve space for input buffer (100 bytes)
    
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
        emitInstruction("mov rax, r8");     // The other operand is now in rax
        emitInstruction("mov r11, 10");
        emitInstruction("mul r11");    // (signed) multiplication
        emitInstruction("mov r8, rax");   

        emitInstruction("mov r12, 0");
        emitInstruction("mov r12b, " + bufferRef);
        emitInstruction("sub r12b, 48");     // 48 = 0 in ASCIi

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
    std::string varName = getName().getContent();
    
    if (look.getContent() != "=") {
        // Just a declaration without an assignment
        return;
    }
    getToken(); // Eat '='
    if (!look.isString) {
        error("Expected a string!");
    }

    int bytesNeeded = 0;
    for (auto c: look.getContent()) {
        //  Brutal
        emitInstruction("mov byte[" + varName + " + " + std::to_string(bytesNeeded) + "], " + std::to_string(int(c)));
        bytesNeeded++;
    }
    emitInstruction("mov byte[" + varName + " + " + std::to_string(bytesNeeded) + "], 0" ); // NULL-termination
    getToken();
    emitVariable(varName, bytesNeeded + 1, "str");
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

void Parser::emitVariable(std::string out, int bytes = 1, std::string varType = "str") {
    if (!program.inVariables(out)) {
            program.addVariable(out, bytes, varType);
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
        if ( look.getContent() != "==") {
            assignment(name);
        }
        
    }
    instr = "mov r8, qword[" + name.getContent() + "]";
    emitInstruction(instr);
    return;
}

void Parser::assignment(Name name) {

    matchToken("=");

    std::string varType = "str";

    if (look.isNumber) {
        varType = "int";
    }

    expression();

    emitVariable(name.getContent(), 8, varType);
    emitComment("assignig value!");
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
    
    bool comparingStrings = false;
    std::string A, B;

    
    if (look.isName & program.isStringVariable(look.getContent())) {
        comparingStrings = true;
        A = look.getContent();
        getToken();
    } else {
        expression();
        emitInstruction("push r8");
    
    }
    
    
    std::string op = mapOperatorToInstruction();
    
    if (!look.isName) {
        emitComment("parsing value!");
        expression();
        emitComment("entering cmp value!");
        std::string labelFalse = getNewLabel();
        emitInstruction("pop r9");
        emitInstruction("cmp r9, r8");
        emitInstruction("mov r8, 1");
        emitInstruction(op + " " + labelFalse);
        emitInstruction("mov r8, 0");
        emitInstruction(labelFalse + ":");

    } else {
        B = look.getContent();
        getToken();
    }
    
    if (comparingStrings) {
            std::string loopStart = getNewLabel();
            std::string loopEnd = getNewLabel();
            std::string loopEndFalse = getNewLabel();
            std::string loopEndTrue  = getNewLabel();
            
            std::string onEquality = "1";
            std::string onInequality = "0";

            if (op == "jne" | op == "jg") {
                onEquality = "0";
                onInequality = "1";
            }


            emitComment("op is " + op);

            emitInstruction("mov r12, 0"); // Char index            
            emitInstruction(loopStart + ":");

            // Compare characters at R11
            emitInstruction("mov r8b, byte[" + A + " + r12]");
            emitInstruction("mov r9b, byte[" + B + " + r12]");
            

            if (op == "je" | op == "jne") {
                emitInstruction("cmp r8b, r9b");
                emitInstruction("jne " + loopEndFalse);
                
                // Check if either is null -> if so, jump out to true
                // Both are null at this point
                emitInstruction("mov r8b, byte[" + A + " + r12]");
                emitInstruction("cmp r8b, 0");
                emitInstruction("je " + loopEndTrue);

            } else if(op == "jl" | op =="jg") {
                emitInstruction("cmp r8b, r9b");
                emitInstruction("jl " + loopEndTrue);
                                
                // Check if either is null -> if so, jump out to false
                // Both are null at this point
                emitInstruction("mov r8b, byte[" + A + " + r12]");
                emitInstruction("cmp r8b, 0");
                emitInstruction("je " + loopEndFalse);

            }                            
            emitInstruction("inc r12");

            // Return to the beginning of loop
            emitInstruction("jmp " + loopStart);

            emitInstruction(loopEndFalse + ":");
            emitInstruction("mov r8, " + onInequality);
            emitInstruction("jmp " + loopEnd);

            emitInstruction(loopEndTrue + ":");
            emitInstruction("mov r8, " + onEquality);
            
            emitInstruction(loopEnd + ":");

    }
    
    // R8 is now 0/1 depending on the comparison


}




std::string Parser::mapOperatorToInstruction() {
    // TODO: all fails should throw with list of accepted operators

    std::string lookOp = look.getContent();

    if (lookOp == "==") {
        matchToken("==");
        return "je";
    }

    if (lookOp == "!=") {
        matchToken("!=");
        return "jne";
    }

    if (lookOp == ">") {
        matchToken(">");
        return "jg"; 
    }
    
    if (lookOp == "<") {
        matchToken("<");
        return "jl"; 
    }

    if (lookOp == ">=") {
        matchToken(">=");
        return "jge";
    }
    
    if (lookOp == "<=") {
        matchToken("<=");
        return "jle";
    }

    expected(" ==, !=, >=, <=, <, <, got: " + look.getContent());
    return ""; // Unreachable
}
