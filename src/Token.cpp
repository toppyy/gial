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


