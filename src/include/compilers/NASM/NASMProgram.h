#pragma once
#include<string>
#include<unordered_map>
#include<vector>
#include<iostream>
#include<exception>
using std::string;

class Variable {
    public:
        Variable(string identifier, string type, string size, int length);
        string identifier;
        string type;       // Should be enum. Atm string: 'int' or 'str'
        string size;       // byte, word, dword, qword
        int length;             // For strings
        
        int sizeInBytes();
        string strSizeInBytes();


        string makeReferenceTo();
        string makeReferenceTo(string offsetRegister);

        string getRegister8Size();
};

class Function {
    public:
        Function(string identifier);
        string identifier;
        std::vector<string> instructions;
        std::vector<std::pair<string, string>> parameters;

        void addInstruction(string instruction);
        void addParameter(string parameter, string type);
        
};

class Constant {
    public:
        Constant(string identifier, string value, string type);
        string identifier;
        string value;
        string type;       // Should be enum. Atm string: 'int' or 'str'
};


class NASMProgram {
    public:
        NASMProgram(std::ostream &p_output_stream);
        std::vector<string> getInstructions();
        void addInstruction(string instruction);
        void addParameterToFunction(string parameter, string type, string identifier);
        void addInstructionToFunction(string instruction, string identifier);
        void addVariable(string identifier, string type, string size, int length);
        void addFunction(string identifier);
        void addConstant(string identifier, string value, string type);
        Variable getVariable(string identifier);
        std::vector<string> buildProgram();
        void outputLine(string s);
        
        bool inVariables(string variable);
        bool isStringVariable(string variable);
        string getNewLabel();

        void inFunction(string functionName);
        void notInFunction();

        bool instructionsToFunction;
    private:
        std::vector<string> instructions;
        string instructionsToFunctionName;
        int icount;
        int labelCount;
        std::ostream &output_stream;
        std::unordered_map<string, Variable> variables;
        std::unordered_map<string, Function> functions;
        std::unordered_map<string, Constant> constants;
};