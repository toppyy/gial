#pragma once
#include "./GAST.h"
#include<memory>
#include<iostream>

using std::shared_ptr, std::string, std::to_string;

class Assembler {
    public:
        
        virtual void assemble(shared_ptr<GAST> p_tree) = 0;
        virtual void traverse(shared_ptr<GNODE> node) = 0;
        virtual void handleNode(shared_ptr<GNODE> node) = 0;
        virtual void error(string error_message) = 0;
        virtual string mapOperatorToInstruction(string op) = 0;
        virtual void checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from) = 0;
        virtual bool checkIfExpressionIsAString(shared_ptr<GNODE> node) = 0;
        virtual void doStringComparison(string op) = 0;

        virtual void emitInstruction(string inst) = 0;
        virtual void emitConstant(string out, string value, string varType) = 0;
        virtual void emitVariable(string out, string varType, string size, int length) = 0;
        virtual void emitComment(std::string comment) = 0;
        

        // Operator handlers
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