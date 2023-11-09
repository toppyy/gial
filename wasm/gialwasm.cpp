#include <string>

extern std::string compile(char* p_assembler, std::string content, int p_optimise);

extern "C" {

    char* compile_to_gial(char* p_assembler, char* content, int p_optimise) {
        std::string code = content;
        code.push_back(-1); // Compiler treats is as EOF
        std::string instructions = compile(p_assembler, code, p_optimise);
        char* p_instr = (char*) calloc(instructions.size() + 1,sizeof(char));
        strcpy(p_instr, instructions.c_str());
        return p_instr;
    }

}
