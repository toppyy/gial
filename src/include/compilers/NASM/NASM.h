#pragma once
#include "../../GAST.h"
#include "../../Assembler.h"
#include "./NASMProgram.h"
#include<memory>
#include<iostream>

using std::shared_ptr, std::unique_ptr, std::string, std::to_string;

class NASM: public Assembler {
    public:
        NASM(GAST& p_tree);
        vector<string> assemble(int optimise);

    private:
        void traverse(shared_ptr<GNODE> node);
        void handleNode(shared_ptr<GNODE> node);
        void error(string error_message);
        string mapOperatorToInstruction(string op);
        void checkNullPtr(shared_ptr<GNODE> node, shared_ptr<GNODE> from);
        bool checkIfExpressionIsAString(shared_ptr<GNODE> node);
        void doStringComparison(string op);

        void emitInstruction(string inst);
        void emitConstant(string out, string value, string varType);
        void emitVariable(string out, string varType, string size, int length);
        void emitFunction(string name);
        void emitComment(std::string comment);
        

        // Operator handlers
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
        void handleInput(shared_ptr<GNODE> node);
        void handleFunctionCall(shared_ptr<GNODE> node);
        void handleMathOperation(shared_ptr<GNODE> node, string op);


        // IO
        void handleOpenFile();
        void handleReadFromFile(string fd);
        void handleCloseFile(string fd);

    
        GAST& tree;
        unique_ptr<NASMProgram> program;

};