#pragma once
#include "./GAST.h"
#include<memory>
#include<iostream>

using std::shared_ptr, std::string, std::to_string;

class Assembler {
    public:
        virtual vector<string> assemble() = 0;        

    private:
        virtual void handleConstant(shared_ptr<GNODE> node) = 0;
        virtual void handleDeclare(shared_ptr<GNODE> node) = 0;
        virtual void handleFor(shared_ptr<GNODE> node) = 0;
        virtual void handlePrintString(shared_ptr<GNODE> node) = 0;
        virtual void handlePrintInt(shared_ptr<GNODE> node) = 0;
        virtual void handlePrintASCII(shared_ptr<GNODE> node) = 0;
        virtual void handleWhile(shared_ptr<GNODE> node) = 0;
        virtual void handleIf(shared_ptr<GNODE> node) = 0;
        virtual void handleBoolTerm(shared_ptr<GNODE> node) = 0;
        virtual void handleBoolExpression(shared_ptr<GNODE> node) = 0;
        virtual void handleBlock(shared_ptr<GNODE> node) = 0;
        virtual void handleExpression(shared_ptr<GNODE> node) = 0;
        virtual void handleVariable(shared_ptr<GNODE> node) = 0;
        virtual void handleAssign(shared_ptr<GNODE> node) = 0;
        virtual void handleInput(shared_ptr<GNODE> node) = 0;
        virtual void handleMathOperation(shared_ptr<GNODE> node, string op) = 0;

};