#include "./include/Parser.h"

Parser::Parser(
        vector<Token> p_tokens,
        set<string> p_keywords,
        GAST& p_tree
        ) : 
        labelCount(0),
        look(Token("EOF")),
        lookChar(0),
        tokens(vector<Token> {}),
        cursor(0),
        token_count(0),
        tree(p_tree)
        {
    tokens = p_tokens;
    cursor = 0;
    token_count = tokens.size();
    keywords = p_keywords;
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

    if (statement == "FUNKTIO") {
        functionDeclarationStatement();
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

    tree.addToCurrent(make_shared<PRINTSTR>());
        
    if (look.isString) {
        // Is a string constant
        tree.branchLeft();
        tree.addToCurrent(make_shared<CONSTANT>(look.getContent(), "str"));
        tree.closeBranch();
        getToken();
        return;
    }

    if (look.isName & peek() != "[") {
        // it's a (not indexed) variable . Interpret content as ASCII-string
        // iterate until NULL
        Token variable = look;
        getToken();

        tree.branchLeft();
        tree.addToCurrent(make_shared<VARIABLE>(variable.getContent()));
        tree.closeBranch();
        return;
    }

    error("Expected a string name or constant");
}

void Parser::printIntStatement() {

    tree.addToCurrent(make_shared<PRINTINT>());
    if (look.isName) {
        // it's a variable. 
        Token varName = getName();

        tree.branchLeft();
        tree.addToCurrent(make_shared<VARIABLE>(varName.getContent()));
        
        if (look == "[") {
            matchToken("[");
            tree.branchRight();
            expression();
            tree.closeBranch();
            matchToken("]");    
        }

        tree.closeBranch();
    } else {
        // it's an expression
        tree.branchLeft();
        expression();
        tree.closeBranch();
    }
}

void Parser::inputStatement() {
    
    Token varName = getName();
    
    string datatype = "str";

    if (look == "LUGU") {
        datatype = "int";
        getToken(); // Eat LUGU
    }
    tree.addToCurrent(make_shared<DECLARE>(varName.getContent(), datatype));
    tree.addToCurrent(make_shared<INPUT>(  varName.getContent(), datatype));
}

void Parser::ifStatement() {
    
    tree.addToCurrent(make_shared<IF>());
    tree.branchLeft();
    boolExpression();
    tree.closeBranch();

    /*
        For IF-ELSE:
            Right branch is a BLOCK-wrapper of which right-branch is taken 
            IF and the left IF ELSE
    */

    tree.branchRight();
    // Init wrapper
    
    tree.addToCurrent(make_shared<BLOCK>());
    
    tree.branchRight();
    block();
    tree.closeBranch();

    if (look == "MUUTES") {
        getToken();
        tree.branchLeft();
        block();
        tree.closeBranch();
    }

    // Wrapper end
    tree.closeBranch();
    matchEndStatement();
}

void Parser::whileStatement() {
   
    tree.addToCurrent(make_shared<WHILE>());
    tree.branchLeft();
    boolExpression();
    tree.closeBranch();

    tree.branchRight();
    block();
    tree.closeBranch();
    matchEndStatement();
}

void Parser::letIntStatement() {
    Token varName = getName();
    
    if (look == "[") {
        // It's an array
        letIntArray(varName);
        return;
    }

    tree.addToCurrent(make_shared<DECLARE>(varName.getContent(),"int"));
    tree.branchLeft();    

    if (look != "=") {
        tree.closeBranch();
        // No associated assignment        
        return;
    }

    assignment(varName);
    tree.closeBranch();
    return;
}

void Parser::letIntArray(Token varName) {
    
    matchToken("[");
    int arraySize = stoi(getNumber().getContent()); // TODO: Catch non-numbers
    matchToken("]");

    tree.addToCurrent(make_shared<DECLARE>(varName.getContent(),"int",arraySize));
}

void Parser::letStringStatement() {
    Token varName = getName();
    tree.addToCurrent(make_shared<DECLARE>(varName.getContent(),"str"));
    tree.branchLeft();
    if (look != "=") {
        // No associated assignment        
        tree.closeBranch();
        return;
    }

    getToken(); // Eat '='

    // 'assignment' can't do strings atm, so do it here
    if (!look.isString) {
        expected("a string when assigning to a string type");
    }
    string strConstant = look.getContent();
    tree.getCurrent()->size = strConstant.length(); // Store string len with DECLARATION

    tree.addToCurrent(make_shared<ASSIGN>(varName.getContent()));
    tree.branchLeft();
    tree.addToCurrent(make_shared<CONSTANT>(strConstant, "str"));
    tree.closeBranch();
    tree.closeBranch();

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
    getToken();
   

    // Create an internal variable for FOR-loop
    // Must be unique so construct a string based on tree state
    string name = "gialvar" + std::to_string(cursor);
    tree.addToCurrent(make_shared<DECLARE>(name,"int"));
    // Assign 0 to it
    tree.branchLeft();
    tree.addToCurrent(make_shared<ASSIGN>(name));
    tree.branchLeft();
    tree.addToCurrent(make_shared<CONSTANT>("0", "int"));
    tree.closeBranch();
    tree.closeBranch();

    // REPEAT is represented as a FOR-loop internally
    tree.addToCurrent(
            make_shared<FOR>(
                name,
                map<string, string> { {"to", to }, { "toType", toType }, {"step", "1" } }
                )
            );

    tree.branchRight();
    block();
    tree.closeBranch();
    matchEndStatement();
}

void Parser::forStatement() {
    // For-loops are BASIC-like:
    // FOR x = 0 TO 10 STEP 1
    // (STEP is optional, defaults to 1)

    // This will declare 'x' and assign the first value
    Parser::letIntStatement();

    string name = tree.getCurrent()->name;

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

    // Eat the 'to'
    getToken();

    string step = "1";
    if (look == "HYBYIL") {
        getToken();
        expectNumber();
        step = look.getContent();
        getToken();
    }

    tree.addToCurrent(
            make_shared<FOR>(
                name,
                map<string, string> { {"to", to }, { "toType", toType }, {"step", step } }
                )
            );

    tree.branchRight();
    block();
    tree.closeBranch();
    matchEndStatement();

}

void Parser::functionDeclarationStatement() {
    // Expects:
    //  FUNKTIO nameoffunction(params)
    //      block
    //  NYLOPPUS

    // Get function name
    Token funName = getName();

    tree.addToCurrent(make_shared<DECLARE>(funName.getContent(),"function"));


    vector<pair<string,string>> params;
    string name, datatype = "fail";
    // Get arguments
    if (lookChar == '(') {
        matchToken("(");
        while (lookChar != ')') {
            if (look == "LUGU") {
                datatype = "int";
            } else if (look == "SANAMBATK") {
                datatype = "string";
            } else {
                expected("argument type (LUGU or SANAMBATK). Got: " + look);
            }
            getToken();
            Token funName = getName();
            params.push_back(std::make_pair(funName.getContent(), datatype));
            if (lookChar == ',') {
                getToken();
            }
        }
        matchToken(")");

        tree.branchRight();
        block();
        tree.closeBranch();
    } else {
        expected("(");
        return;
    }

    // TODO: better data structure
    // ATM: parameters are stored in a string
    // name:datatype,name:datatype,..
    string parameterString;
    for (auto a: params) {
        parameterString += a.first + ":" + a.second + ",";
    }

    // Remove last comma
    if (parameterString.size() > 0) {
        parameterString.pop_back();
    }
    tree.getCurrent()->info["parameters"] = parameterString;

    matchEndStatement();
}


/* ------- END STATEMENTS ----------------------------------------------------------------------------------------------------------------  */

void Parser::emitLine() {
    tree.addToCurrent(make_shared<PRINTASCII>("LF")  ); // TODO: don't use PRINTASCII? 
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
    expected("token '" + expected_content + "', got: " + look);
}

void Parser::matchEndStatement() {
    matchToken("NYLOPPUS");
}

/* ----------------------- PARSING EXPRESSION ---------------------------- */

void Parser::expression() {
    // Trick: Add a zero to start calculations with
    // This way -3 + 3 becomes 0 - 3 + 3

    tree.addToCurrent(make_shared<EXPRESSION>());
    uint32_t current = tree.current;
        
    tree.branchLeft();
        term();
    tree.closeBranch();

    while (lookChar == '+' | lookChar == '-' | lookChar == '*' | lookChar == '/') {
        tree.getCurrent()->setOperator(look.getContent());
        getToken();
        tree.branchRight();
        term();
        tree.closeBranch();
    }
    tree.unsetLeftAsDefault();
    tree.current = current; // Return to the 'level' of this expr

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
    
     if (look.isString & (look.length() == 1)) {
        // Single character
        tree.addToCurrent(make_shared<CONSTANT>(look.getContent(), "chr"));
        getToken();
        return;
    }


    if (isAlpha(lookChar)) {
        ident(); // Checks if it's a function or a variable name
        return;
    }

    
    if (isDigit(lookChar)) {
        tree.addToCurrent(make_shared<CONSTANT>(look.getContent(), "int"));
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

    
    // Check if it's a function call 
    if (lookChar == '(') {
        functionCall(name);
        return;
    }

    // Check if it's an indexed reference
    if (lookChar == '[') {
        matchToken("[");
        
        // Add a block to the current expression
        // The right branch of the block will contain the expr in variable[expr]
        tree.addToCurrent(make_shared<BLOCK>());
        tree.branchRight();
        expression();
        tree.closeBranch();
        matchToken("]");
        
        if (look == "=") {
            // This manipulates current BLOCK
            indexedAssignment(name);
            return;
        }

        // It's an indexed variable ref
        tree.addToCurrent(make_shared<VARIABLE>(name.getContent()));

        // Add index to the left branch of variable

        shared_ptr<GNODE> current   = tree.getCurrent();
        shared_ptr<GNODE> parent    = tree.getParent(current);
        
        current->setRight(parent->getRight());
        parent->makeRightNull();
        return;
    }        
    
    // Check if it's an assignment to a variable
    if (look == "=") {
        assignment(name);
        return;
    }

    // If not, store the value into r8   
    tree.addToCurrent(make_shared<VARIABLE>(name.getContent()));
}

void Parser::indexedAssignment(Token name) {
    matchToken("=");

    // Create ASSIGNment. Do some tree manipulation so
    // that this has the expr describing the index as the right
    // child (atm it is in the parent BLOCK's right branch)
    tree.addToCurrent(make_shared<ASSIGN>(name.getContent()));

    shared_ptr<GNODE> current   = tree.getCurrent();
    shared_ptr<GNODE> parent    = tree.getParent(current);
    shared_ptr<GNODE> indexExpr = tree.getRight(parent);
    
    current->setRight(indexExpr->id);
    indexExpr->setParent(tree.current);
    parent->makeRightNull();
    parent->setNext(tree.nodecount);

    tree.branchLeft();
    expression();
    tree.closeBranch();
}

void Parser::assignment(Token name) {
    matchToken("=");
    tree.addToCurrent(make_shared<ASSIGN>(name.getContent()));
    tree.branchLeft();
    expression();
    tree.closeBranch();
}

void Parser::functionCall(Token name) {
    matchToken("(");
    string arguments;
    while (lookChar != ')') {
        arguments += getName().getContent() + ",";
        if (lookChar == ',') {
            matchToken(",");
        }
    }
    matchToken(")");

    if (arguments.size() > 0) {
        arguments.pop_back();
    }

    tree.addToCurrent(
        make_shared<FUNCTIONCALL>(
            name.getContent(),
            map<string, string> { {"arguments", arguments } }
        )
    );
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

    tree.addToCurrent(make_shared<BOOLEXPR>());
    tree.branchLeft();
    if (lookChar == '(') {
        matchToken("(");
        boolExpression();
        matchToken(")");
    } else {
        boolTerm();
    }
    tree.closeBranch();
    
    string nextTokenContent = look.getContent();
    
    while (
        nextTokenContent == "JA" |
        nextTokenContent == "TAI"
    ) {
        string op = nextTokenContent == "JA" ? "and" : "or";
        tree.getCurrent()->setOperator(op);
        matchToken(nextTokenContent);
        
        tree.branchRight();
        boolExpression();
        tree.closeBranch();
        
        nextTokenContent = look.getContent();
    }
}

void Parser::boolTerm() {   

    tree.addToCurrent(make_shared<BOOLTERM>());
    tree.branchLeft();
    expression();
    tree.closeBranch();
    
    tree.getCurrent()->setOperator(look.getContent());
    matchToken(look.getContent());
    
    tree.branchRight();
    expression();
    tree.closeBranch();
}
