#include "./include/GAST.h"

GAST::GAST() {
    std::cout << "hello from tree!";
};

void GAST::setRoot(GNODE node) {
    root = node;
}