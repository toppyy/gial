#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>



class Token {
    public:
        Token();
        Token(std::string p_content);

        int length();
        std::string getContent();
        char getCharFromContent(int idx);

        bool isNumber;
        bool isKeyword;
        bool isName;
        bool isString;


    public:
        std::string content;

};

class Number: public Token {
    public:
        Number(std::string p_content);

};

class Name: public Token {
    public:
        Name(std::string p_content);

};



class Keyword: public Token {
    public:
        Keyword(std::string p_content);

};

