#include "./include/Parser.h"

Parser::Parser(
        vector<Token> p_tokens,
        set<string> p_keywords,
        Program p_program,
        shared_ptr<GAST> p_tree
        ) : 
        labelCount(0),
        look(Token("EOF")),
        lookChar(0),
        tokens(vector<Token> {}),
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

void Parser::init() {
    getToken();
    block();
}

void Parser::mapStatementToFunction(string statement ) {

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

void Parser::block() {
    string nextToken = "?";
    string endToken = "NYLOPPUS";


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
void Parser::printStatement() {

    tree->addToCurrent (PRINTSTR());
        
    if (look.isString) {
        // Is a string constant
        tree->branchLeft();
        tree->addToCurrent(CONSTANT(look.getContent(), "str"));
        tree->closeBranch();
        getToken();
        return;
    }

    if (look.isName & peek() != "[") {
        // it's a (not indexed) variable . Interpret content as ASCII-string
        // iterate until NULL
        Token variable = look;
        getToken();

        tree->branchLeft();
        tree->addToCurrent(VARIABLE(variable.getContent()));
        tree->closeBranch();
        return;
    }

    error("Expected a string name or constant");
}

void Parser::printIntStatement() {

    tree->addToCurrent(PRINTINT());
    if (look.isName) {
        // it's a variable. 
        Token varName = getName();

        tree->branchLeft();
        tree->addToCurrent(VARIABLE(varName.getContent()));
        
        if (look == "[") {
            matchToken("[");
            tree->branchRight();
            expression();
            tree->closeBranch();
            matchToken("]");    
        }

        tree->closeBranch();
    } else {
        // it's an expression
        tree->branchLeft();
        expression();
        tree->closeBranch();
    }
}

void Parser::inputStatement() {
    
    Token varName = getName();
    
    string datatype = "str";

    if (look == "LUGU") {
        datatype = "int";
        getToken(); // Eat LUGU
    }
    tree->addToCurrent( DECLARE(varName.getContent(), datatype));
    tree->addToCurrent( INPUT(  varName.getContent(), datatype));
}

void Parser::ifStatement() {
    
    tree->addToCurrent(IF());
    tree->branchLeft();
    boolExpression();
    tree->closeBranch();

    /*
        For IF-ELSE:
            Right branch is a BLOCK-wrapper of which right-branch is taken 
            IF and the left IF ELSE
    */

    tree->branchRight();
    // Init wrapper
    
    tree->addToCurrent(BLOCK());
    
    tree->branchRight();
    block();
    tree->closeBranch();

    if (look == "MUUTES") {
        getToken();
        tree->branchLeft();
        block();
        tree->closeBranch();
    }

    // Wrapper end
    tree->closeBranch();
    matchEndStatement();
}

void Parser::whileStatement() {
   
    tree->addToCurrent(WHILE());
    tree->branchLeft();
    boolExpression();
    tree->closeBranch();

    tree->branchRight();
    block();
    tree->closeBranch();
    matchEndStatement();
}

void Parser::letIntStatement() {
    Token varName = getName();
    
    if (look == "[") {
        // It's an array
        letIntArray(varName);
        return;
    }

    tree->addToCurrent(DECLARE(varName.getContent(),"int"));
    tree->branchLeft();    

    if (look != "=") {
        tree->closeBranch();
        // No associated assignment        
        return;
    }

    assignment(varName);
    tree->closeBranch();
    return;
}

void Parser::letIntArray(Token varName) {
    
    matchToken("[");
    int arraySize = stoi(getNumber().getContent()); // TODO: Catch non-numbers
    matchToken("]");

    tree->addToCurrent(DECLARE(varName.getContent(),"int",arraySize));
}

void Parser::letStringStatement() {
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

void Parser::repeatStatement() {
    // implements
    //  REPEAT n
    //      [code to be repeated n times]
    //  ENDREPEAT
    //

    // n is either a integer or a variable ref
    string to = look.getContent(), toType;
    if (look.isNumber) {
       toType = "int";
    }  else if (look.isName) {
       toType = "variable";
    } else {
        error("Expected a number or a variable name after TOIST. Got: " + look);
    }

    // Create an internal variable for FOR-loop
    // Must be unique so construct a string based on tree state
    string name = "GIALREP" + std::to_string(cursor);
    tree->addToCurrent(DECLARE(name,"int"));

    // REPEAT is represented as a FOR-loop internally
    tree->addToCurrent(
            FOR(
                name,
                map<string, string> { {"to", to }, { "toType", toType }, {"step", "1" } }
                )
            );

    tree->branchRight();
    block();
    tree->closeBranch();
    matchEndStatement();
}

void Parser::forStatement() {
    // For-loops are BASIC-like:
    // FOR x = 0 TO 10 STEP 1
    // (STEP is optional, defaults to 1)

    // This will declare 'x' and assign the first value
    Parser::letIntStatement();

    string name = tree->current->name;

    matchToken("TOHO");
    // Looping "to" - either a integer or a variable ref
    // toType will tell if it's a variable ref or an int

    string to = look.getContent(), toType;
    if (look.isNumber) {
       toType = "int";
    }  else if (look.isName) {
       toType = "variable";
    } else {
        error("Expected a number or a variable name after TOHO. Got: " + look);
    }

    string step = "1";
    if (look == "HYBYIL") {
        getToken();
        expectNumber();
        step = look.getContent();
        getToken();
    }

    tree->addToCurrent(
            FOR(
                name,
                map<string, string> { {"to", to }, { "toType", toType }, {"step", step } }
                )
            );

    tree->branchRight();
    block();
    tree->closeBranch();
    matchEndStatement();

}


/* ------- END STATEMENTS ----------------------------------------------------------------------------------------------------------------  */

void Parser::emitLine() {
    tree->addToCurrent ( PRINTASCII("LF")  );
    return;
}

void Parser::error(string error_message) {
    throw std::runtime_error(error_message);
}

void Parser::expected(string expected_thing) {
    error("Expected: " + expected_thing + "\n");
    return;
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
Token Parser::peek() {
    return tokens[cursor];
}

void Parser::getToken() {
    if (cursor >= token_count) {
        string err_msg = "No more tokens!\n";
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

void Parser::matchToken(string expected_content) {

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

    tree->addToCurrent(EXPRESSION());
    shared_ptr<GNODE> current = tree->current;
        
    tree->branchLeft();
        term();
    tree->closeBranch();

    while (lookChar == '+' | lookChar == '-' | lookChar == '*' | lookChar == '/') {
        tree->current->setOperator(look.getContent());
        getToken();
        tree->branchRight();
        term();
        tree->closeBranch();
    }
    tree->unsetLeftAsDefault();
    tree->current = current; // Return to the 'level' of this expr

}   

void Parser::term() {
   
    // If an opening parenthesis is met, start an another expression recursively
    // This works because nothing is emitted before the 'inner most' parenthesis have been met
    if (lookChar == '(') {
        matchToken("(");
        expression();
        matchToken(")");
        return;
    }
    
     if (look.isString & look.length() == 1) {
        // Single character
        tree->addToCurrent(CONSTANT(look.getContent(), "chr"));
        getToken();
        return;
    }


    if (isAlpha(lookChar)) {
        ident(); // Checks if it's a function or a variable name
        return;
    }

    
    if (isDigit(lookChar)) {
        tree->addToCurrent(CONSTANT(look.getContent(), "int"));
        getToken();
        return;
    }

    if (lookChar == 0) {
        return;
    }


    error("Should have not reached token \\w content " + look.getContent());
    return;
}   

void Parser::ident() { 
    Token name = getName();

    // Check if it's an indexed reference
    if (lookChar == '[') {
        matchToken("[");

        tree->addToCurrent(BLOCK());
        tree->branchRight();
        expression();
        tree->closeBranch();

        matchToken("]");

        if (look == "=") {
            // This manipulates current BLOCK
            indexedAssignment(name);
            return;
        } else {
            // It's an indexed variable ref

            tree->addToCurrent(VARIABLE(name.getContent()));
            // Add index to the left branch of variable
            tree->current->setRight(tree->current->getParent()->getRight());
            tree->current->getParent()->makeRightNull();


            return;
        }
    }        
    
    // Check if it's an assignment to a variable
    if (look == "=") {
        assignment(name);
        return;
    }

    // If not, store the value into r8   
    tree->addToCurrent(VARIABLE(name.getContent()));
}

void Parser::indexedAssignment(Token name) {

    matchToken("=");

    // Create ASSIGNment. Do some tree manipulation so
    // that this has the expr describing the index as the right
    // child (atm it is in the parent BLOCK's right branch)
    // This is also manipulated further in ident()
    tree->addToCurrent(ASSIGN(name.getContent()));

    shared_ptr<GNODE> indexExpr = tree->current->getParent()->getRight();
    tree->current->setRight(indexExpr);
    indexExpr->setParent(tree->current);
    tree->current->getParent()->makeRightNull();
    tree->current->getParent()->setNext(tree->current);

    tree->branchLeft();
    expression();
    tree->closeBranch();
}

void Parser::assignment(Token name) {
    matchToken("=");
    tree->addToCurrent(ASSIGN(name.getContent()));
    tree->branchLeft();
    expression();
    tree->closeBranch();
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

    tree->addToCurrent(BOOLEXPR());
    tree->branchLeft();
    if (lookChar == '(') {
        matchToken("(");
        boolExpression();
        matchToken(")");
    } else {
        boolTerm();
    }
    tree->closeBranch();
    
    string nextTokenContent = look.getContent();
    
    while (
        nextTokenContent == "JA" |
        nextTokenContent == "TAI"
    ) {
        string op = nextTokenContent == "JA" ? "and" : "or";
        tree->current->setOperator(op);
        matchToken(nextTokenContent);
        
        tree->branchRight();
        boolExpression();
        tree->closeBranch();
        
        nextTokenContent = look.getContent();
    }
}

void Parser::boolStringComparison() {
    /*
    string A = look.getContent();
    getToken();
    
    string op = mapOperatorToInstruction();
    
    string B = look.getContent();

    if (!program.isStringVariable(B)) {
        error("Expected a string, got: " + B);
    }
    getToken();

    string loopStart = getNewLabel();
    string loopEnd = getNewLabel();
    string loopEndFalse = getNewLabel();
    string loopEndTrue  = getNewLabel();


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
    */
    
}   

void Parser::boolTerm() {   

    tree->addToCurrent(BOOLTERM());
    tree->branchLeft();
    expression();
    tree->closeBranch();
    
    tree->current->setOperator(look.getContent());
    matchToken(look.getContent());
    
    tree->branchRight();
    expression();
    tree->closeBranch();
}
