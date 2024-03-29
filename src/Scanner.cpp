#include "./include/Scanner.h"
#define CR 13
#define LF 10
#define TAB 9
#define SPACE 32

Scanner::Scanner(string p_content, set<string> p_keywords) :
                                    content(),
                                    cursor(0),
                                    cursor_max(0),
                                    look(0),
                                    tokens(vector<Token> {}),

                                    keywords(set<string> {}) 
{
    content = p_content;
    cursor = 0;
    cursor_max = p_content.length();
    keywords = p_keywords;

    
}

void Scanner::init() {
    Token token  = Token("placeholder");

    while (token.getContent() != "EOF") {
        token = scan();
        if (token.length() == 0) {
            continue;
        }

        tokens.push_back(token);
    };

}

vector<Token> Scanner::getTokens() {
    return tokens;
}

Token Scanner::handleName(string name) {

    string wNextToken = name + " " + lookAheadName();

    if (keywords.count(wNextToken) > 0) {
        skipWhite();
        getName();
        return createKeywordToken(wNextToken);
    }

    if (keywords.count(name) > 0) {
        return createKeywordToken(name);
    }

    return createNameToken(name);
}

Token Scanner::scan() {

    skipWhite();

    if (isQuote(look)) {
        return createStringToken(getString());
    }
    

    if (isAlpha(look)) {
        string name = getName();
        return handleName(name);
    }
    
    if (isDigit(look)) {
        return createNumberToken(getNum());
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
        if (look == '!') {
            getChar();
            getChar();
            return Token("!=");
        }

    }


    string rtrn = "";
    rtrn.push_back(look);
    getChar();
    
    return Token(rtrn);
}

char Scanner::peekCharacter() {
    return content[cursor];
}

string Scanner::lookAheadName() {
    // Looks ahead until a white space or end of cursor is met
    string token = "";
    int lookCursor = cursor;
    while ( isAlphaNumeric(content[lookCursor]) ) {
        token.push_back(content[lookCursor]);
        lookCursor++;
    }
    return token;
}



string Scanner::getString() {
    string token = "";
    getChar(); // Eat quote beginning
    while (!isQuote(look)) {
        token.push_back(look);
        getChar();
    }
    getChar(); // Eat quote end
    return token;
}



string Scanner::getName() {
    string token = "";
    
    if (!isAlpha(look)) {
        expected("Name");
    }
    while (isAlphaNumeric(look)) {
        token.push_back(look);
        getChar();
    }
    return token;
}


string Scanner::getNum() {
    string value = "";
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
    string err_msg = "Unexpected end!\n";
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

bool Scanner::isQuote(char x) {
    return x == 39; // ' is ASCII
}


void Scanner::error(string error_message) {
    throw std::runtime_error(error_message);
}

void Scanner::expected(string expected_thing) {
    error("Expected: " + expected_thing + "\n");
}


Token Scanner::createStringToken(string p_content) {
    Token newToken = Token(p_content);
    newToken.isString = true;
    return newToken;
}

Token Scanner::createKeywordToken(string p_content) {
    Token newToken = Token(p_content);
    newToken.isKeyword = true;
    return newToken;
}

Token Scanner::createNumberToken(string p_content) {
    Token newToken = Token(p_content);
    newToken.isNumber = true;
    return newToken;
}

Token Scanner::createNameToken(string p_content) {
    Token newToken = Token(p_content);
    newToken.isName = true;
    return newToken;
}
