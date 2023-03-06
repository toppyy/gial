#include "./include/Token.h"

Token::Token() {

}


Token::Token(std::string p_content) {
    content = p_content;
    isNumber = false;
    isKeyword = false;
    isName = false;
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



Number::Number(std::string p_content) : Token(p_content) {
    isNumber = true;
}


Keyword::Keyword(std::string p_content) : Token(p_content) {
    isKeyword = true;
}

Name::Name(std::string p_content) : Token(p_content) {
    isName = true;
}
