#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>
#include<set>
#include "./Token.h"

using std::string, std::set, std::vector;

class Scanner {
    public:
        Scanner(string p_content, set<string> p_keywords);

        
        void init();
        void error(string error_message);
        void expected(string expected_thing);
        void skipWhite();
        void getChar();

        bool isWhite(char x);
        bool isAlphaNumeric(char x);
        bool isAlpha(char x);
        bool isDigit(char x);
        bool isQuote(char x);
    
        string getOperator();
        string getName();
        string getString();
        string getNum();
        vector<Token> getTokens();

        string lookAheadName();
        char peekCharacter();

        Token scan();
        Token handleName(string name);
        Token createNumberToken(string p_content);
        Token createStringToken(string p_content);
        Token createNameToken(string p_content);
        Token createKeywordToken(string p_content);
        
    private:
        char look;
        
        int cursor;
        int  cursor_max;


        set<string> keywords; 
        string content;
        vector<Token> tokens;  
};