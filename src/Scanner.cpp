#include "./include/Scanner.h"
#define CR 13
#define LF 10
#define TAB 9
#define SPACE 32

Scanner::Scanner(std::string p_content) :
                                    content(),
                                    cursor(0),
                                    cursor_max(0),
                                    look(0),
                                    tokens(std::vector<Token> {}),

                                    keywords(std::set<std::string> {}) 
{
    content = p_content;
    cursor = 0;
    cursor_max = p_content.length();


    // List keywords
    keywords.insert("GUNNES");
    keywords.insert("GUHA");
    keywords.insert("OLGO");
    keywords.insert("TOIST");
    keywords.insert("SAN NY");
    keywords.insert("SAN JOTTAI");
    keywords.insert("SAN SNAA");
    keywords.insert("SAN LUGU");
}

void Scanner::init() {
    Token token  = Token("non empty");
    int i = 10;
    while (token.getContent() != "EOF") {
        token = scan();
        if (token.length() == 0) {
            continue;
        }

        tokens.push_back(token);        

        i--;
    };

}

std::vector<Token> Scanner::getTokens() {
    return tokens;
}

Token Scanner::handleName(std::string name) {

    std::string wNextToken = name + " " + lookAheadName();

    if (keywords.count(wNextToken) > 0) {
        skipWhite();
        getName();
        return Keyword(wNextToken);
    }

    if (keywords.count(name) > 0) {
        return Keyword(name);
    }

    return Name(name);
}




Token Scanner::scan() {

    skipWhite();
    

    if (isAlpha(look)) {
        std::string name = getName();
        return handleName(name);
    }
    
    if (isDigit(look)) {
        return Number(getNum());
    }
    
    if (look < -1) { // Just skip all non-ASCII characters
        getChar();
        return Token("");
    }
    
    if (look == -1) {
        return Token("EOF");
    }
    // Special cases are logical operators with multiple characters
    // So peek ahead a bit
    char peek = peekCharacter();

    if (peek == '=' ) {
        if (look == '=') {
            getChar();
            getChar();
            return Token("==");
        }
        if (look == '<') {
            getChar();
            getChar();
            return Token("<=");
        }
        if (look == '>') {
            getChar();
            getChar();
            return Token(">=");
        }

    }

    std::string rtrn = "";
    rtrn.push_back(look);
    getChar();
    
    return Token(rtrn);
}

char Scanner::peekCharacter() {
    return content[cursor];
}

std::string Scanner::lookAheadName() {
    // Looks ahead until a white space or end of cursor is met
    std::string token = "";
    int lookCursor = cursor;
    while ( isAlphaNumeric(content[lookCursor]) ) {
        token.push_back(content[lookCursor]);
        lookCursor++;
    }
    return token;
}




std::string Scanner::getName() {
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


std::string Scanner::getNum() {
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




void Scanner::getChar() {
    // We need to allow going over the string buffer by one byte
    // The logic looks 'ahead' one character
    std::string err_msg = "Unexpected end!\n";
    if (cursor >= cursor_max + 1) {
        error(err_msg);
    }
    look = content[cursor];
    cursor++;    
}

bool Scanner::isWhite(char x) {
    return x == LF | x == TAB | x == SPACE ;
}
void Scanner::skipWhite() {
    while (isWhite(look)) {
        getChar();
    }
}


bool Scanner::isAlphaNumeric(char x) {
    return isAlpha(x) | isDigit(x);
}

bool Scanner::isAlpha(char x) {
    return 
        (x >= 65 & x <= 90) // A-Z
        |
        (x >= 97 & x <= 122); // a-z
}

bool Scanner::isDigit(char x) {
    return x >= 48 & x <= 57; // 0-9
}


void Scanner::error(std::string error_message) {
    throw std::runtime_error(error_message);
}

void Scanner::expected(std::string expected_thing) {
    error("Expected: " + expected_thing + "\n");
}


/*

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



*/