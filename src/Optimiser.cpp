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
    return instructions;
}

string find_first_operand(string instruction) {
    
    size_t space_pos = instruction.find_first_of(" ");
    size_t comma_pos = instruction.find_first_of(",");
    
    if (space_pos == std::string::npos) {
        throw std::runtime_error("Could not find first operand!");
    }

    return instruction.substr(space_pos + 1, comma_pos - space_pos - 1);


}


static inline string ltrim(std::string s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
    return s;
}

vector<string> remove_push_pop_pattern(vector<string> instructions) {
    // Quite commonly the same register is pushed on to the stack and immediately popped back
    // Remove all such patterns.
    // Example:
    //      push r8
    //      pop r8

    if (instructions.size() < 2) {
        // Very odd corner case
        return instructions;
    }

    vector<string> rtrn;
    std::vector<bool> include(instructions.size(), true);

    string prev, cur;
    for (int i = 1; i < instructions.size(); i++) {
        
        prev = instructions[i-1];
        cur  = instructions[i];

        if (prev.size() < 5) {
            continue;
        }
        if (cur.size() < 4) {
            continue;
        }

        string reg1 = ltrim(prev);
        string reg2 = ltrim(cur);

        if (reg1.rfind("push", 0) == 0) {
            if (reg2.rfind("pop ", 0) == 0) {
                if (find_first_operand(reg1) == find_first_operand(reg2)) {
                    include[i-1] = false;
                    include[i] = false;
                }
            }
        }

    }
    
    for (int i = 1; i < instructions.size(); i++) {
        if (include[i]) {
            rtrn.push_back(instructions[i]);
        }
        
    }

    return rtrn;
}


vector<string> optimise(vector<string> instructions) {
    
    std::clog << "Optimising!\n";
    // std::clog << "operand: " << find_first_operand("push r8") << "\n";

    set<string>     regs = find_registers(instructions);
    vector<string>  vars = get_variables(instructions);



    instructions = remove_push_pop_pattern(instructions);

    // for (auto v: vars) {
    //     std::clog << v << ", ";
    // }
    // std::clog << "\n";
    // std::for_each(regs.cbegin(), regs.cend(), [](string r) {
    //     std::clog << "" << r << ", ";
    // });
    // std::clog << "\n";

    
    return instructions;
}