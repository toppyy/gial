#include "../../include/compilers/Javascript/Optimiser.h"




vector<string> JSOptimiser::optimise(vector<string> instructions) {
    
    std::clog << "Optimising " << instructions.size() << " statements\n";

    // instructions = remove_push_pop_pattern(instructions);


    std::clog << "Got it down to " << instructions.size() << " statements\n";    
    return instructions;
}