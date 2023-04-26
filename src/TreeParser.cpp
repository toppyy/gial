#include "./include/TreeParser.h"


TreeParser::TreeParser(
        std::vector<Token> p_tokens,
        std::set<std::string> p_keywords,
        Program p_program,
        std::shared_ptr<GAST> p_tree
        ) : 
                                        labelCount(0),
                                        look(Token("EOF")),
                                        lookChar(0),
                                        tokens(std::vector<Token> {}),
                                        cursor(0),
                                        token_count(0),
                                        program(p_program),
                                        tree(p_tree)
                                        
                                        {
    tokens = p_tokens;
    cursor = 0;
    token_count = tokens.size();
    keywords = p_keywords;
    tree = p_tree;

}

void TreeParser::build() {


}




void TreeParser::buildProgram() {
    program.buildProgram();
}

void TreeParser::init() {

    getToken();
    block();
}

void TreeParser::insertToken(Token tkn) {
    tokens.insert(tokens.begin() + cursor - 1, tkn);
    token_count++;
}


void TreeParser::mapStatementToFunction(std::string statement ) {

    if (statement == "SAN NY")  {
        printStatement();            
        return;
    }

    if (statement == "SIKSES") {
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
        whileStatement();
        return;
    }

    if (statement == "JOS")  {        
        ifStatement();
        return;
    }
        
    if (statement == "TOIST")  {
        repeatStatement();
        return;
    }

    if (statement == "SANAMBATK") {
        letStringStatement();
        return;
    }

    if (statement == "LUGU") {
        letIntStatement();
        return;
    }

    if (statement == "TOST") {
        forStatement();
        return;
    }

}


void TreeParser::block() {
    std::string nextToken = "?";
    std::string endToken = "NYLOPPUS";


    while (
        look != "NYLOPPUS" &
        look != "EOF" &
        look != "MUUTES" 
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
void TreeParser::printStatement() {
        

    if (look.isString) {
        // Is a string constant
        std::string instr;
        std::string label = getNewLabel();
        emitInstruction("printBytes " + label + ", 0, " + std::to_string(look.length()));
        emitConstant(label, look.getContent(), "str");

        tree->addToCurrent (PRINTSTRCONST( look.getContent() ));

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


void TreeParser::printIntStatement() {

    tree->addToCurrent(PRINTINT());

    if (look.isName) {
        // it's a variable. 
        Token varName = getName();
        
        tree->current->name = varName.getContent();

        if (look == "[") {
            matchToken("[");
            expression();
            matchToken("]");    
        }
    } else {
        // it's an expression
        tree->openBranch();
        expression();
        tree->closeBranch();
    }
}

void TreeParser::emitLine() {

    tree->addToCurrent ( PRINTASCII("LF")  );
    emitInstruction("mov dil, LF");
    emitInstruction("call PrintASCII");
    return;
}

void TreeParser::inputStatement() {
    
    Token varName = getName();
    
    string datatype = "str";

    if (look == "LUGU") {
        datatype = "int";
        getToken(); // Eat LUGU
    }
    tree->addToCurrent( DECLARE(varName.getContent(), datatype));
    tree->addToCurrent( INPUT(  varName.getContent(), datatype));


}


void TreeParser::ifStatement() {
    
    tree->addToCurrent(IF());
    tree->branchLeft();
    boolExpression();
    tree->closeBranch();

    tree->branchRight();
    block();
    tree->closeBranch();
    matchEndStatement();
    /*
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
    */
}


void TreeParser::whileStatement() {
   
    tree->addToCurrent(WHILE());
    tree->branchLeft();
    boolExpression();
    tree->closeBranch();

    tree->branchRight();
    block();
    tree->closeBranch();
    matchEndStatement();
}



void TreeParser::letIntStatement() {
    Token varName = getName();
    
    if (look == "[") {
        // It's an array
        letIntArray(varName);
        return;
    }
    emitIntVariable(varName);



    tree->addToCurrent(DECLARE(varName.getContent(),"int"));
    tree->branchLeft();    


    if (look != "=") {
        tree->closeBranch();
        // No associated assignment        
        return;
    }

    // Assign an int
    if (!peek().isNumber) {
        expected("integer when assigning to numeric type");
    }
    assignment(varName);
    tree->closeBranch();
    return;
}

void TreeParser::letIntArray(Token varName) {
    matchToken("[");
    Token arraySize = getNumber();
    matchToken("]");

    emitVariable(varName, "int", "qword", stoi(arraySize.getContent()));


    if (look != "=") {
        // No associated assignment        
        return;
    }

    matchToken("=");
    matchToken("[");

    Variable var = program.getVariable(varName.getContent());

    std::vector<Token> numbers;
    int i = 0;
    emitInstruction("mov r9, 0");
    while (look != "]") {
        if (look.isNumber) {
            emitInstruction("mov " + var.size + "[ " + varName + " + " + std::to_string(i) + "], " + look);
            i += 8;
        }
        getToken();
    }
    matchToken("]");



}


void TreeParser::letStringStatement() {
    Token varName = getName();
    tree->addToCurrent(DECLARE(varName.getContent(),"str"));
    tree->branchLeft();
    if (look != "=") {
        // No associated assignment        
        tree->closeBranch();
        return;
    }

    getToken(); // Eat '='

    // 'assignment' can't do strings atm, so do it here
    if (!look.isString) {
        expected("a string when assigning to a string type");
    }
    string strConstant = look.getContent();
    tree->current->size = strConstant.length(); // Store string len with DECLARATION

    tree->addToCurrent(ASSIGN(varName.getContent()));
    tree->branchLeft();
    tree->addToCurrent(CONSTANT(strConstant, "str"));
    tree->closeBranch();
    tree->closeBranch();

    getToken();
}



void TreeParser::repeatStatement() {
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


void TreeParser::forStatement() {
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


std::string TreeParser::getNewLabel() {
    labelCount += 1;
    return "LBL_" + std::to_string(labelCount);
}

void TreeParser::emitInstruction(std::string out) {
    program.addInstruction(out);
}

void TreeParser::emitStringVariable(Token var, int length) {
    emitVariable(var, "str", "byte", length);
}

void TreeParser::emitIntVariable(Token var) {
    emitVariable(var, "int", "qword", 1);
}


void TreeParser::emitVariable(Token out, std::string varType, std::string size, int length) {
    std::string content = out.getContent();
    if (!program.inVariables(content)) {
            program.addVariable(content, varType, size, length);
    }
    
}

void TreeParser::emitConstant(std::string out, std::string value, std::string varType = "str") {
    program.addConstant(out, value, varType);
    
}



void TreeParser::emitComment(std::string comment) {
    emitInstruction("; " + comment);
}

void TreeParser::error(std::string error_message) {
    throw std::runtime_error(error_message);
}

void TreeParser::expected(std::string expected_thing) {
    error("Expected: " + expected_thing + "\n");
    return;
}

Token TreeParser::peek() {
    return tokens[cursor];
}

void TreeParser::getToken() {
    if (cursor >= token_count) {
        std::string err_msg = "No more tokens!\n";
        error(err_msg);
    }
    look = tokens[cursor];
    lookChar = look.getCharFromContent(0);
    cursor++;
}

Token TreeParser::getNumber() {
    if (look.isNumber) {
        Token rtrn = look;
        getToken();
        return rtrn;
    }
    error("Expected a number, got: " + look);
    return Token("");
}


Token TreeParser::getName() {
    if (look.isName) {
        Token rtrn = look;
        getToken();
        return rtrn;
        
    }
    error("Expected a name, got: " + look);
    return Token("");
}

Variable TreeParser::getVariable(std::string varname) {
    return program.getVariable(varname);
}



void TreeParser::matchToken(std::string expected_content) {

    if (look == expected_content) {        
        getToken();
        return;
    }
    expected("character " + expected_content + ", got: " + look);
}

void TreeParser::matchEndStatement() {
    matchToken("NYLOPPUS");
}


/* ----------------------- PARSING EXPRESSION ---------------------------- */

void TreeParser::expression() {
    // Trick: Add a zero to start calculations with
    // This way -3 + 3 becomes 0 - 3 + 3

    tree->addToCurrent(EXPRESSION());
    tree->setLeftAsDefault();
        
    if ( !isAddOp(lookChar)) {
        term();
    }

    while (isAddOp(lookChar)) {
        tree->current->setOperator(look.getContent());
        if (lookChar == '+') {
            add();
        } else if (lookChar == '-') {
            minus();
        }
    }
    tree->unsetLeftAsDefault();

}   



void TreeParser::term() {
    factor();
    while (lookChar == '*' | lookChar == '/') {
        tree->current->setOperator(look.getContent());
         if (lookChar == '*') {
            multiply();
        } else if (lookChar == '/') {
            divide();
        }
    }
}

void TreeParser::factor() {
   
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

        tree->addToCurrent(CONSTANT(nextToken.content, "int"));

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

void TreeParser::ident() { 
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

    // Check if it's an indexed reference
    bool indexedRefence = false;
    if (lookChar == '[') {
        indexedRefence = true;
        matchToken("[");
        expression();
        matchToken("]");
        // Move whatever was between brackets into r9
        emitInstruction("mov r9, r8");
    }        
    
    // Check if it's an assignment to a variable
    if (look == "=") {
        if (indexedRefence) {
            indexedAssignment(name);
            return;
        }
        assignment(name);
        return;
    }

    // If not, store the value into r8
    // We need to pull the variable to know whether to read a byte or ..?
    
    tree->addToCurrent(VARIABLE(name.getContent()));
}

void TreeParser::indexedAssignment(Token name) {

    // R8 has the result of the of expression in brackets
    // We need it later, so push it on the stack
    emitInstruction("push r8"); 

    matchToken("=");

    // Side-effect of this is that we check the variable has been declared
    Variable var = program.getVariable(name.getContent());

    // What is assigned?
    expression();

    // Do the assignment
    emitInstruction("pop r9");
    emitInstruction("mov " + var.makeReferenceTo("r9") + ", " + var.getRegister8Size());

    return;
}

void TreeParser::assignment(Token name) {
    matchToken("=");
    tree->addToCurrent(ASSIGN(name.getContent()));
    tree->branchLeft();
    expression();
    tree->closeBranch();
}

bool TreeParser::isAddOp(char x) {
    return x == '+' | x == '-';
}

bool TreeParser::isAlphaNumeric(char x) {
    return isAlpha(x) | isDigit(x);
}


void TreeParser::add() {
    matchToken("+");
    term();
}   

void TreeParser::minus() {
    matchToken("-");
    term();
}

void TreeParser::multiply() {
    matchToken("*");
    factor();                 // Moves multiplier to register (r8)
}

void TreeParser::divide() {
    matchToken("/");                // Consume the division character. 'look' now has the divisor
    factor();                       // Moves divisor to register (r8)
}


bool TreeParser::isAlpha(char x) {
    return 
        (x >= 65 & x <= 90) // A-Z
        |
        (x >= 97 & x <= 122); // a-z
}


bool TreeParser::isDigit(char x) {
    return x >= 48 & x <= 57; // 0-9
}

/* ------------------------ Parse boolean expressions -------------------------------------------------------- */

void TreeParser::boolExpression() {
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
        nextTokenContent == "JA" |
        nextTokenContent == "TAI"
    ) {
        
        matchToken(nextTokenContent); // eat the token
        emitInstruction("push r8");
        boolExpression();
        emitInstruction("pop r9");
        
        if (nextTokenContent == "JA") {
            emitInstruction("and r8, r9");
        }
        if (nextTokenContent == "TAI") {
            emitInstruction("or r8, r9");
        }
        nextTokenContent = look.getContent();
    }
}

void TreeParser::boolStringComparison() {

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


    emitInstruction("mov r12, 0"); // Char index            
    emitInstruction(loopStart + ":");

    // Compare characters at R11
    emitInstruction("mov r8b, byte[" + A + " + r12]");
    emitInstruction("mov r9b, byte[" + B + " + r12]");


    if (op == "je") {
        emitInstruction("cmp r8b, r9b");        
        emitInstruction("jne " + loopEndFalse);
        
        emitInstruction("mov r8b, byte[" + A + " + r12]");
        emitInstruction("cmp r8b, 0");
        emitInstruction("je " + loopEndTrue);

    } else if (op == "jne") {
        emitInstruction("cmp r8b, r9b");        
        emitInstruction("jne " + loopEndTrue);
        
        emitInstruction("mov r8b, byte[" + A + " + r12]");
        emitInstruction("cmp r8b, 0");
        emitInstruction("je " + loopEndFalse);

    } else if(op == "jl") {

        emitInstruction("cmp r8b, r9b");
        emitInstruction("jl " + loopEndTrue);

        emitInstruction("cmp r8b, r9b");
        emitInstruction("jg " + loopEndFalse);
                    
        emitInstruction("mov r8b, byte[" + A + " + r12]");
        emitInstruction("cmp r8b, 0");
        emitInstruction("je " + loopEndFalse);


    } else if(op == "jg") {

        emitInstruction("cmp r8b, r9b");
        emitInstruction("jg " + loopEndTrue);

        emitInstruction("cmp r8b, r9b");
        emitInstruction("jl " + loopEndFalse);

        emitInstruction("mov r8b, byte[" + A + " + r12]");
        emitInstruction("cmp r8b, 0");
        emitInstruction("je " + loopEndFalse);

    }                            
    emitInstruction("inc r12");

    // Return to the beginning of loop
    emitInstruction("jmp " + loopStart);

    emitInstruction(loopEndFalse + ":");
    emitInstruction("mov r8, 0");
    emitInstruction("jmp " + loopEnd);

    emitInstruction(loopEndTrue + ":");
    emitInstruction("mov r8, 1");

    emitInstruction(loopEnd + ":");
    
}   

void TreeParser::boolTerm() {   
    // When return R8 must be 0/1 depending on the comparison
    std::string labelFalse = getNewLabel();
    
    // If the first token in the boolean expression is a string, expect the other is also
    /*
    if (
        look.isName &
        program.isStringVariable(look.getContent()) &
        peek() != "[" // If it's a index, proceed
        ) {
        boolStringComparison();
        return;    
    }
    */
    BOOLTERM term = BOOLTERM();
    shared_ptr<BOOLTERM> p_term = make_shared<BOOLTERM>(term);
    tree->addToCurrent(term);
    tree->branchLeft();
    expression();
    tree->closeBranch();
    


    //std::cout << " current is " << tree->current->getType() << "\n";

    tree->current->setOperator(look.getContent());
    
    emitInstruction("push r8");
    std::string op = mapOperatorToInstruction();
    
    
    if (program.isStringVariable(look.getContent())) {
        error("Mismatch: can't compare to type string with content: " + look);
    }
    tree->branchRight();
    expression();
    tree->closeBranch();    
    

    

    emitInstruction("pop r9");
    emitInstruction("cmp r9, r8");
    emitInstruction("mov r8, 1");
    emitInstruction(op + " " + labelFalse);
    emitInstruction("mov r8, 0");
    emitInstruction(labelFalse + ":");
    
}

std::string TreeParser::mapOperatorToInstruction() {

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


void TreeParser::expectName() {
    if (!look.isName) {
        error("Expected a name, got " + look);
    }
}
void TreeParser::expectNumber() {
    if (!look.isNumber) {
        error("Expected a number, got " + look);
    }
}