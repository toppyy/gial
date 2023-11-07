#include <string>

extern char* compile(char* p_assembler, std::string content, int p_optimise);

extern "C" {

    char* compile_to_gial(char* p_assembler, char* content, int p_optimise) {
        std::string code = content;
        code.push_back(-1); // Compiler treats is as EOF
        return compile(p_assembler, code, p_optimise);
    }

}