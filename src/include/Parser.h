#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>
#include "./Program.h"
#include "./Scanner.h"
#include "./Token.h"

class Parser {
    public:
        Parser(Scanner &p_scanner, Program &p_program);
        void init();
        void error(std::string error_message);
        void expected(std::string expected_thing);
        


        void matchToken(std::string expected_content);
        void getToken();

        std::string getNewLabel();

        void emitComment(std::string comment);
        void emitLine();
        void emitInstruction(std::string out);
        void emitVariable(std::string out, int bytes);

        void block();

        // Parsing expressions --------

        void term();
        void add();
        void ident();
        void minus();
        void expression();
        void printLookInfo(char info);
        void factor();
        void divide();
        void multiply();
        void assignment(Name name);
        bool isDigit(char x);
        bool isAlpha(char x);
        bool isAlphaNumeric(char x);
        bool isAddOp(char x);
        Name getName();

        // Statements ---------------------------

        void printStatement();
  

        // 

        /*
        void getChar();
        void match(char x);
        void matchString(std::string str);
        
        bool isDigit(char x);
        bool isAddOp(char x);
        bool isWhite(char x);
        bool isEOL(char x);
        void skipWhite();
        void skipEOL();
        void statement();
        std::string logoperator();
        std::string lookAheadToken();
        bool isNextToken(std::string keyword);
        std::string getName();
        std::string getNum();
        void emitVariable(std::string out, int bytes);
        void ifStatement();
        void letStatement();
        void repeatStatement();
        void whileStatement();
        void inputStatement();
        void printIntStatement();
        void printLine();
        void boolExpression();
        void boolTerm();
        void relOp();
        */
        

    private:
        std::vector<Token> tokens;
        Token look;
        char lookChar;
        int  labelCount;
        int  cursor;
        int  token_count;
        Program &program;

};