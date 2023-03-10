#include "./include/Token.h"

Token::Token() {

}


Token::Token(std::string p_content) : content(), isNumber(false), isKeyword(false), isName(false), isString(false) {
    content = p_content;
    isNumber = false;
    isKeyword = false;
    isName = false;
    isString = false;
}

bool Token::operator == (const std::string &str) {
    return content == str;
}
bool Token::operator != (const std::string &str) {
    return content != str;
}

std::string Token::operator + (const std::string &str) {
    return str + content;
}

std::string operator+(const std::string &str, const Token &tkn ) {
    return str + tkn.content;
}



int Token::length() {
    return content.length();
}

char Token::getCharFromContent(int idx) {
    if (idx >= content.length()) {
        std::cout << "Token index out of bound in getCharFromContent" << "\n"; // TODO exception
    }
    return content[idx];
}

std::string Token::getContent() {
    return content;
}


