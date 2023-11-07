#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <exception>
#include <memory>
#include "./Scanner.h"
#include "./Token.h"
#include "./GAST.h"

using std::string, std::vector, std::shared_ptr, std::set;

class Parser
{
public:
    Parser(vector<Token> p_tokens, set<string> p_keywords, GAST& p_tree);
    void init();
    void mapStatementToFunction(string statement);

    void error(string error_message);
    void expected(string expected_thing);

    void matchEndStatement();
    void matchToken(string expected_content);
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
    vector<Token> tokens;
    Token look;
    char lookChar;
    int labelCount;
    int cursor;
    int token_count;
    GAST& tree;
    set<string> keywords;
};