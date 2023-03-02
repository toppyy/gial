#pragma once
#include<string>
#include<iostream>
#include<exception>

class Parser {
    public:
        Parser(std::string p_content);
        void init();
        void error(std::string error_message);
        void expected(std::string expected_thing);
        void getChar();
        void match(char x);
        bool isAlpha(char x);
        bool isDigit(char x);
        bool isAddOp(char x);
        char getName();
        char getNum();
        void emit(std::string out);
        void emitLine(std::string out);
        void term();
        void add();
        void minus();
        void expression();
        void printLookInfo();
        void factor();
        void divide();
        void multiply();

    private:
        std::string content;
        char look;
        int  cursor;
        int  cursor_max;

};