#include "./include/GAST.h"

GAST::GAST() : gnodes(vector<GNODE> {}), root(GNODE()), current(nullptr) {
    root = GNODE();
    root.isRoot = true;
    root.setType("root");
    current = &root;
};


void GAST::addChildToCurrent(GNODE node) {
    node.setParent(current);
    gnodes.push_back(node);
    current->setChild(&node);
}
void GAST::moveToNode(GNODE node) {

}
void GAST::branchFromroot(GNODE node) {

}
void GAST::returnToRoot() {

}

void GAST::toChild() {
    current = current->getChild();
}
void GAST::toParent() {
    if (current->isRoot) {
        return;
    }
    current = current->getParent();
}


GNODE::GNODE() : isRoot(false), next(nullptr), child(nullptr), parent(nullptr), type("unknown") {

};


void GNODE::setChild(GNODE* p_child) {
    child = p_child;
}

void GNODE::setParent(GNODE* p_parent) {
    parent = p_parent;
}

void GNODE::setType(string p_type) {
    type = p_type;
}

string getNodeType(GNODE* node) {
    if (node == nullptr) {
        return "null";
    }
    return node->getType();
}

void GNODE::print() {
    std::cout << " i am node of type " << this->getType() << " and my parent is " << getNodeType(parent) <<"\n";
}

GNODE* GNODE::getChild() {
    return child;
}
GNODE* GNODE::getParent() {
    return parent;
}

string GNODE::getType() {
    return type;
}

