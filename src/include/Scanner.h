#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>
#include<set>
#include "./Token.h"

class Scanner {
    public:
        Scanner(std::string p_content);

        
        void init();
        void error(std::string error_message);
        void expected(std::string expected_thing);
        void skipWhite();
        void getChar();

        bool isWhite(char x);
        bool isAlphaNumeric(char x);
        bool isAlpha(char x);
        bool isDigit(char x);
    
        std::string getOperator();
        std::string getName();
        std::string getNum();
        std::vector<Token> getTokens();

        std::string lookAheadName();

        Token scan();
        Token handleName(std::string name);
        
    private:
        char look;
        
        int cursor;
        int  cursor_max;


        std::set<std::string> operators; 
        std::set<std::string> keywords; 
        std::string content;
        std::vector<Token> tokens;  
};