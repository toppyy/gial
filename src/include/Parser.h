#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>
#include "./Program.h"

class Parser {
    public:
        Parser(std::string p_content, Program &p_program);
        void init();
        void error(std::string error_message);
        void expected(std::string expected_thing);
        void getChar();
        void match(char x);
        bool isAlpha(char x);
        bool isAlphaNumeric(char x);
        bool isDigit(char x);
        bool isAddOp(char x);
        bool isWhite(char x);
        void skipWhite();
        bool parsingToBeDone();
        void incrementCursor();
        std::string getName();
        std::string getNum();
        void emitVariable(std::string out);
        void emitInstruction(std::string out);
        void term();
        void add();
        void ident();
        void minus();
        void expression();
        void printLookInfo();
        void factor();
        void divide();
        void multiply();
        void assignment(std::string name);

    private:
        std::string content;
        char look;
        int  cursor;
        int  cursor_max;
        Program &program;

};