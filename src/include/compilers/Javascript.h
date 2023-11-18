#pragma once
#include "../GAST.h"
#include "../Assembler.h"
#include<memory>
#include<vector>
#include<iostream>
#include<set>
using std::shared_ptr, std::string, std::to_string;

class Javascript: public Assembler {
    public:
        Javascript(GAST& p_tree);
        vector<string> assemble();
    
    private:
        void traverse(shared_ptr<GNODE> node);
        void handleNode(shared_ptr<GNODE> node);
        void error(string error_message);
        void checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from);
        void writeToStdout(string tolog, bool quote = false);

        void emitInstruction(string inst);
        void emitVariable(string name);
        void emitComment(std::string comment);
        
        void handleConstant(shared_ptr<GNODE> node);
        void handleDeclare(shared_ptr<GNODE> node);
        void handleFor(shared_ptr<GNODE> node);
        void handlePrintString(shared_ptr<GNODE> node);
        void handlePrintInt(shared_ptr<GNODE> node);
        void handlePrintASCII(shared_ptr<GNODE> node);
        void handleWhile(shared_ptr<GNODE> node);
        void handleIf(shared_ptr<GNODE> node);
        void handleBoolTerm(shared_ptr<GNODE> node);
        void handleBoolExpression(shared_ptr<GNODE> node);
        void handleBlock(shared_ptr<GNODE> node);
        void handleExpression(shared_ptr<GNODE> node);
        void handleVariable(shared_ptr<GNODE> node);
        void handleAssign(shared_ptr<GNODE> node);
        void handleFunctionCall(shared_ptr<GNODE> node);
        void handleInput(shared_ptr<GNODE> node);
        void handleMathOperation(shared_ptr<GNODE> node, string op);

        bool variableDeclared(string name);

        GAST& tree;
        std::vector<string> instructions;
        std::set<string> variables;
        bool wrapToAsyncFunction;
};