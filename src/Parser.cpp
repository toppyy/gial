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

void Parser::insertToken(Token tkn) {
    tokens.insert(tokens.begin() + cursor - 1, tkn);
    token_count++;
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

    if (statement == "TOST") {
        forStatement();
        return;
    }

}


void Parser::block() {
    std::string nextToken = "?";
    std::string endToken = "NYLOPPUS";


    while (
        look != "NYLOPPUS" &
        look != "EOF" &
        look != "MUUTES" &
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
}

/* ---------- STATEMENTS ----------------------------------------------------------------------------------------------------------------  */
void Parser::printStatement() {
        

    if (look.isString) {
        // Is a string constant
        std::string instr;
        std::string label = getNewLabel();
        emitInstruction("printBytes " + label + ", 0, " + std::to_string(look.length()));
        emitConstant(label, look.getContent(), "str");

        getToken();
        return;
    }

    if (look.isName & peek() != "[") {
        // it's a (not indexed) variable . Interpret content as ASCII-string
        // iterate until NULL

        Token variable = look;
        getToken();
        std::string loopStart   = getNewLabel();

        emitInstruction("mov r11, 0");
        
        emitInstruction(loopStart + ":");
        emitInstruction("push r11");
        emitInstruction("mov dil, byte[" + variable + " + r11]");
        emitInstruction("call PrintASCII");
        emitInstruction("pop r11");
        emitInstruction("inc r11");
        emitInstruction("mov r8b, byte[" + variable + " + r11]");
        emitInstruction("cmp r8b, 0");
        emitInstruction("jne " + loopStart);
        return;
    }    
    expression();
    emitInstruction("mov dil, r8b");
    emitInstruction("call PrintASCII");
    return;
}


void Parser::printIntStatement() {

    
    if (look.isName) {
        // it's a variable. 
        Token var = getName();
        emitInstruction("mov rdi, qword[" + var + "]");

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
    
    Token varName = getName();
    std::string bufferName = varName.getContent();
    std::string labelInputLoop = getNewLabel();
    std::string labelLoopOut = getNewLabel();
    
    std::string bufferRef = "byte[" + bufferName + " + r9]";

    bool integerInput = false;
    std::string varType = "str";
    std::string varSize = "byte";
    int varLength = 100;


    if (look == "LUGU") {
        integerInput = true;
        varType = "int";
        varSize = "qword";
        varLength = 0;
        getToken();
    }

    if (integerInput) {
        // For converting the ASCII-representation to int
        emitInstruction("mov r8, 0");   // Intermediate result
        emitInstruction("push r8");
    }

    emitVariable(varName, varType, varSize, varLength);     // Reserve space for input buffer (100 bytes)
    
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
    std::string labelOut   = getNewLabel();
    // Sets r8 to 0/1 depending on the evaluation
    boolExpression();

    // Compare it and determine whether to run 'block'
    emitInstruction("cmp r8, 1");
    emitInstruction("jne " + labelFalse); // jump to 'labelFalse' if evaluates to false

    block();
    
    emitInstruction("jmp " + labelOut);
    emitInstruction(labelFalse + ":");

    if (look == "MUUTES") {
        getToken();
        block();
    }
    
    emitInstruction(labelOut + ":");
    
    matchEndStatement();
}


void Parser::whileStatement(std::string afterNestedBlock) {
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
    emitInstruction(afterNestedBlock);
    emitInstruction("jmp " + labelTrue);
    emitInstruction(labelFalse + ":");
    matchEndStatement();
}

void Parser::letStatement() {
    Token varName = getName();
    
    if (look != "=") {
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
    emitVariable(varName, "str", "byte", bytesNeeded + 1);
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
       n =  "qword[" + look + "]";
    } else {
        error("Expected a number or a variable name after TOIST. Got: " + look);
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





void Parser::forStatement() {
    // For statement is internally represented as while-loop
    // For-loops are BASIC-like:
    // FOR x = 0 TO 10 STEP 1
    // (STEP is optional, defaults to 1)

    Token name = getName();

    emitIntVariable(name);
    
    assignment(name);
    matchToken("TOHO");
    expectNumber();

    Token to = getNumber();
    
    std::string step = "1";
    if (look == "HYBYIL") {
        getToken();
        expectNumber();
        step = look.getContent();
        getToken();
    }

    std::string loopStart  = getNewLabel();
    std::string variableRef = "qword[" + name + "]";

    emitInstruction("mov r8, " + variableRef);   // Init counter
    emitInstruction(loopStart + ":");
    emitInstruction("push r8");     // Push counter onto stack
    
    block();

    emitInstruction("pop r8");                  // Pop counter from stack
    emitInstruction("add r8, " + step);         // Increment counter from stack
    emitInstruction("mov " + variableRef + ", r8"); // Update loop variable
    emitInstruction("cmp r8, " + to);           // Compare counter to n
    emitInstruction("jl " + loopStart);         // If it's less than n, jmp to labelTrue
    matchEndStatement();    

}


/* ------- END STATEMENTS ----------------------------------------------------------------------------------------------------------------  */


std::string Parser::getNewLabel() {
    labelCount += 1;
    return "LBL_" + std::to_string(labelCount);
}

void Parser::emitInstruction(std::string out) {
    program.addInstruction(out);
}

void Parser::emitStringVariable(Token var, int length) {
    emitVariable(var, "str", "byte", length);
}

void Parser::emitIntVariable(Token var) {
    emitVariable(var, "int", "qword", 1);
}


void Parser::emitVariable(Token out, std::string varType, std::string size, int length) {
    std::string content = out.getContent();
    if (!program.inVariables(content)) {
            program.addVariable(content, varType, size, length);
    }
    
}

void Parser::emitConstant(std::string out, std::string value, std::string varType = "str") {
    program.addConstant(out, value, varType);
    
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

Token Parser::peek() {
    return tokens[cursor];
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

Token Parser::getNumber() {
    if (look.isNumber) {
        Token rtrn = look;
        getToken();
        return rtrn;
    }
    error("Expected a number, got: " + look);
    return Token("");
}


Token Parser::getName() {
    if (look.isName) {
        Token rtrn = look;
        getToken();
        return rtrn;
        
    }
    error("Expected a name, got: " + look);
    return Token("");
}


void Parser::matchToken(std::string expected_content) {

    if (look == expected_content) {        
        getToken();
        return;
    }
    expected("character " + expected_content + ", got: " + look);
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
   
    // If an opening parenthesis is met, start an another expression recursively
    // This works because nothing is emitted before the 'inner most' parenthesis have been met
    if (lookChar == '(') {
        matchToken("(");
        expression();
        matchToken(")");
        return;
    }
    
    std::string instr;

    if (look.isString & look.length() == 1) { // TODO: Should be checked in bool term
        instr = "mov r8, '" + look + "'";
        emitInstruction(instr);
        getToken();
        return;
    }


    if (isAlpha(lookChar)) {
        ident(); // Checks if it's a function or a variable name
        return;
    }

    
    Token nextToken = look;
    if (isDigit(lookChar)) {
        instr = "mov r8, " + nextToken;
        emitInstruction(instr);
        getToken();
        return;
    }

    if (lookChar == 0) {
        return;
    }


    error("Should have not reached token \\w content " + nextToken);
    return;
}   




void Parser::ident() {
    Token name = getName();
    std::string instr;

    // check if the name is a name of a function (not a variable)
    if (lookChar == '(') {
        matchToken("(");
        matchToken(")"); // Currently can match only empty argument lists
        instr = "call " + name;
        emitInstruction(instr);
        return;
    }


    if (lookChar == '[') {
        matchToken("[");
        expression();
        matchToken("]");
        
        if (look == "=") {
            // TODO: assigment should support this
            emitInstruction("push r8");
            matchToken("=");

            if (look.isNumber) {
                emitIntVariable(name);                
            } else if (look.isName) {
                emitStringVariable(name, 100);
            } else {
                expected("String or integer variable ref.");
            }

            expression();

            std::string reg = "r8b";
            emitInstruction("pop r9");
            emitInstruction( "mov byte[ " + name + " + r9], " + reg);
            return;
        }
        emitInstruction("mov r9, r8");
        emitInstruction("mov r8b, byte[" + name + " + r9]");
        
        return;
    }


    // Check if this is actually a condition, not an assignment. If a condition, do nothing.
    if ( look == "=") {
        assignment(name);
    }
        
    instr = "mov r8, qword[" + name + "]";
    emitInstruction(instr);
    return;
}

void Parser::assignment(Token name) {

    matchToken("=");

    Variable var = program.getVariable(name.getContent());
    expression();

    std::string reg = "r8";
    if (var.size == "byte") {
        reg = "r8b";
    }

    emitInstruction( "mov " + var.size + "[ " + name + "], " + reg);
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

void Parser::boolStringComparison() {

    std::string A = look.getContent();
    getToken();
    
    std::string op = mapOperatorToInstruction();
    
    std::string B = look.getContent();

    if (!program.isStringVariable(B)) {
        error("Expected a string, got: " + B);
    }
    getToken();

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

void Parser::boolTerm() {   
    // When return R8 must be 0/1 depending on the comparison
    std::string labelFalse = getNewLabel();
    
    // If the first token in the boolean expression is a string, expect the other is also
    if (
        look.isName &
        program.isStringVariable(look.getContent()) &
        peek() != "[" // If it's a index, proceed
        ) {
        boolStringComparison();
        return;    
    }
    expression();
    emitInstruction("push r8");
    
    std::string op = mapOperatorToInstruction();
    
    if (program.isStringVariable(look.getContent())) {
        error("Mismatch: can't compare to type string with content: " + look);
    }
    
    expression();
    
    emitInstruction("pop r9");
    emitInstruction("cmp r9, r8");
    emitInstruction("mov r8, 1");
    emitInstruction(op + " " + labelFalse);
    emitInstruction("mov r8, 0");
    emitInstruction(labelFalse + ":");
    
}

std::string Parser::mapOperatorToInstruction() {

    std::string lookOp = look.getContent();

    std::unordered_map<std::string,std::string> operatorToInstruction {
        {"==", "je"},
        {"!=", "jne"},
        {">", "jg"},
        {"<", "jl"},
        {">=", "jge"},
        {"<=", "jle"}
    };


    if (operatorToInstruction.count(lookOp) == 0) {
            expected(" ==, !=, >=, <=, <, <, got: " + lookOp);
    }

    matchToken(lookOp);
    auto search = operatorToInstruction.find(lookOp);

    return search->second;
}


void Parser::expectName() {
    if (!look.isName) {
        error("Expected a name, got " + look);
    }
}
void Parser::expectNumber() {
    if (!look.isNumber) {
        error("Expected a number, got " + look);
    }
}