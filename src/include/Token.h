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

