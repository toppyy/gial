#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>
#include<set>
#include "./Token.h"

class Scanner {
    public:
        Scanner(std::string p_content, std::set<std::string> p_keywords);

        
        void init();
        void error(std::string error_message);
        void expected(std::string expected_thing);
        void skipWhite();
        void getChar();

        bool isWhite(char x);
        bool isAlphaNumeric(char x);
        bool isAlpha(char x);
        bool isDigit(char x);
        bool isQuote(char x);
    
        std::string getOperator();
        std::string getName();
        std::string getString();
        std::string getNum();
        std::vector<Token> getTokens();

        std::string lookAheadName();
        char peekCharacter();

        Token scan();
        Token handleName(std::string name);
        Token createNumberToken(std::string p_content);
        Token createStringToken(std::string p_content);
        Token createNameToken(std::string p_content);
        Token createKeywordToken(std::string p_content);
        
    private:
        char look;
        
        int cursor;
        int  cursor_max;


        std::set<std::string> keywords; 
        std::string content;
        std::vector<Token> tokens;  
};