#pragma once
#include <string>
#include <bits/stdc++.h>
#include <vector>
#include <iostream>
#include <exception>
#include <memory>
#include "./Program.h"
#include "./Scanner.h"
#include "./Token.h"
#include "./GAST.h"

class TreeParser
{
public:
    TreeParser(std::vector<Token> p_tokens, std::set<std::string> p_keywords, Program p_program, std::shared_ptr<GAST> p_tree);
    void init();
    void mapStatementToFunction(std::string statement);

    void error(std::string error_message);
    void expected(std::string expected_thing);

    void matchEndStatement();
    void matchToken(std::string expected_content);
    void getToken();
    Token getName();
    Token getNumber();
    void expectNumber();
    void expectName();
    Token peek();


    void block();

    // Parsing expressions --------

    void term();
    void ident();
    void expression();
    void printLookInfo(char info);
    void assignment(Token name);
    void indexedAssignment(Token name);
    bool isDigit(char x);
    bool isAlpha(char x);

    // Parsing boolean expressions ---------

    void boolExpression();
    void boolTerm();
    void boolStringComparison();

    // Statements ---------------------------

    void emitLine();
    void printStatement();
    void printIntStatement();
    void inputStatement();
    void ifStatement();
    void whileStatement();
    void forStatement();
    void letStringStatement();
    void letIntStatement();
    void repeatStatement();
    void letIntArray(Token var);

private:
    std::vector<Token> tokens;
    Token look;
    char lookChar;
    int labelCount;
    int cursor;
    int token_count;
    std::shared_ptr<GAST> tree;
    Program program;
    std::set<std::string> keywords;
};