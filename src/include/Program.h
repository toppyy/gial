#pragma once
#include<string>
#include <unordered_map>
#include<vector>
#include<iostream>
#include<exception>

class Variable {
    public:
        Variable(std::string identifier, std::string type, std::string size, int length);
        std::string identifier;
        std::string type;       // Should be enum. Atm string: 'int' or 'str'
        std::string size;       // byte, word, dword, qword
        int length;             // For strings
};

class Constant {
    public:
        Constant(std::string identifier, std::string value, std::string type);
        std::string identifier;
        std::string value;
        std::string type;       // Should be enum. Atm string: 'int' or 'str'
};


class Program {
    public:
        Program();
        std::vector<std::string> getInstructions();
        void addInstruction(std::string instruction);
        void addVariable(std::string identifier, std::string type, std::string size, int length);
        void addConstant(std::string identifier, std::string value, std::string type);
        void getVariable(std::string identifier);
        void buildProgram();
        void outputLine(std::string s);
        
        bool inVariables(std::string variable);
        bool isStringVariable(std::string variable);


    private:
        std::vector<std::string> instructions;
        int icount;
        std::unordered_map<std::string, Variable> variables;
        std::unordered_map<std::string, Constant> constants;
};