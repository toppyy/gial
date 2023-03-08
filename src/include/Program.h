#pragma once
#include<string>
#include <unordered_map>
#include<vector>
#include<iostream>
#include<exception>

class Variable {
    public:
        Variable(std::string identifier, std::string type, int size);
        std::string identifier;
        std::string type;       // Should be enum. Atm string: 'int' or 'str'
        int size;               // In bytes
};


class Program {
    public:
        Program();
        std::vector<std::string> getInstructions();
        void addInstruction(std::string instruction);
        void addVariable(std::string identifier, int size, std::string type);
        void buildProgram();
        void outputLine(std::string s);
        
        bool inVariables(std::string variable);
        bool isStringVariable(std::string variable);


    private:
        std::vector<std::string> instructions;
        int icount;
        std::unordered_map<std::string, Variable> variables;
};