#include "./include/Optimiser.h"

#define ASCII_ZERO 48
#define ASCII_NINE 57


#define ASCII_ALPHABET_A 65
#define ASCII_ALPHABET_Z 90

#define ASCII_ALPHABET_a 97
#define ASCII_ALPHABET_z 122

bool is_digit(char c) {
    return c >= ASCII_ZERO && c <= ASCII_NINE;
}

bool is_ltr(char c) {
    return 
        (c >= ASCII_ALPHABET_A && c <= ASCII_ALPHABET_Z) |
        (c >= ASCII_ALPHABET_a && c <= ASCII_ALPHABET_z);
}


vector<string> get_variables(vector<string> instructions) {

    vector<string> vars;
    int i = 0;
    while (instructions[i++] != "section .bss") {}
    while (instructions[i]   != "section .data") {
        
        string instr = instructions[i];
        string var = "";
        int z = 0;

        while (!is_ltr(instr[++z])) {}

        while (is_ltr(instr[z])) {
            var += instr[z++];
        }

        vars.push_back(var);
        i++;
    }

    return vars;
}

set<string> find_registers(vector<string> instructions) {
    // Returns a vector of registers used in the asm

    set<string> regs;

    for (auto instr: instructions) {
        // There's a register if the we find the character 'r' followed by 1 or 2 digits.
        // (good enough)

        bool in_reg = false;
        string tmp;
        for (int i = 0; i < instr.size(); i++) {
            if (instr[i] == 'r') {
                in_reg = true;
                tmp = "r";
                continue;
            }

            if (!is_digit(instr[i]) & in_reg) {
                in_reg = false;
                if (tmp.size() > 1) {
                    regs.insert(tmp);
                    tmp = "";
                }
            }

            tmp += instr[i];

        }
    }   

    return regs;
}


vector<string> use_register_instead_of_variable(vector<string> instructions, vector<string> &vars) {

    string to_replace = "qword[" + vars[0] + "]";
    for (int i = 0; i < instructions.size(); i++) {

        std::size_t found = instructions[i].find(to_replace);
        if (found != std::string::npos) {
            instructions[i].replace(found, to_replace.size(), "r11");

        }
        
        
    }

}




vector<string> optimise(vector<string> instructions) {
    
    std::clog << "Optimising!\n";

    set<string>     regs = find_registers(instructions);
    vector<string>  vars = get_variables(instructions);


    for (auto v: vars) {
        std::clog << v << ", ";
    }
    std::clog << "\n";
    std::for_each(regs.cbegin(), regs.cend(), [](string r) {
        std::clog << "" << r << ", ";
    });
    std::clog << "\n";

    
    return instructions;
}