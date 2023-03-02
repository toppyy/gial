#pragma once
#include<string>
#include<vector>
#include<iostream>
#include<exception>


class Program {
    public:
        Program();
        std::vector<std::string> getInstructions();
        void addInstruction(std::string instruction);
        void addVariable(std::string variable);

    private:
        std::vector<std::string> instructions;
        std::vector<std::string> variables;
};