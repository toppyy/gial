#include "./include/GAST.h"

int ID = 0;

GAST::GAST() : gnodes(vector<shared_ptr<GNODE>> {}),  current(nullptr), nextToLeft(false), branchStack(stack<shared_ptr<GNODE>>()) {
    GNODE root = GNODE();
    root.isRoot = true;
    root.setType("root");
    shared_ptr<GNODE> ptr = make_shared<GNODE>(root);
    current = ptr;
    gnodes.push_back(ptr);
    nextToLeft = false;
    
};


void GAST::openBranch() {
    branchStack.push(current);
    nextToLeft = true;
}

void GAST::closeBranch() {
    current = branchStack.top();
    branchStack.pop();
    nextToLeft = false;
}

void GAST::addAsLeftToCurrent(GNODE node) {
    node.setParent(current);
    gnodes.push_back(make_shared<GNODE>(node));
    current->setLeft(make_shared<GNODE>(node));
    //std::cout << "added " << current->getLeft()->getType() << " as left to " << current->getType() << "\n";
}


void GAST::addToCurrent(GNODE node) {
    node.setParent(current);
    shared_ptr<GNODE> ptr = make_shared<GNODE>(node);
    node.setParent(current->getParent());
    gnodes.push_back(ptr);
    if (nextToLeft) {
        current->setLeft(ptr);
        nextToLeft = false;
    } else {
        current->setRight(ptr);
    }
    
    current = ptr;

    //std::cout << "added " << node.getType() << " Right to " << current->getType() << "\n";
}

void GAST::moveToNode(GNODE node) {

}
void GAST::branchFromroot(GNODE node) {

}
void GAST::returnToRoot() {
    current = gnodes[0];
}

bool GAST::toLeft() {
    if (current->getLeft() == nullptr) {
        return false;
    }
    current = current->getLeft();
    return true;
}
bool GAST::toParent() {
    if (current->isRoot) {
        return false;
    }
    current = current->getParent();
    return true;
}

shared_ptr<GNODE> GAST::getRoot() {
    return gnodes[0];
}


GNODE::GNODE() : 
    isRoot(false),
    right(nullptr),
    left(nullptr),
    parent(nullptr),
    type("unknown"),
    op(""),
    value(""),
    datatype(""),
    name("")
    {

}


void GNODE::setLeft(shared_ptr<GNODE> p_left) {
    left = p_left;
}

void GNODE::setParent(shared_ptr<GNODE> p_parent) {
    parent = p_parent;
}

void GNODE::setRight(shared_ptr<GNODE> p_right) {
    right = p_right;
}

void GNODE::setType(string p_type) {
    type = p_type;
}

string getNodeType(shared_ptr<GNODE> node) {
    if (node == nullptr) {
        return "null";
    }
    return node->getType();
}

void GNODE::print() {
    int tabs = 1;
    if (type.length() <= 7) {
        tabs++;
    }
    string tab = "";
    while (tabs > 0) {
        tab += "\t";
        tabs--;
    }
    //std::cout << getType() ; //<< tab << "(" << getNodeType(parent) << ")";
    std::cout << this->getType()<< tab <<"parent: " << getNodeType(parent) <<  tab << "right: " << getNodeType(right) << tab << "left: " << getNodeType(left);
    
    if (this->getType() == "BOOLTERM") {
        std::cout << " and my op is " << this->op << "\n";
    }
    std::cout << "\n";
    
}

shared_ptr<GNODE> GNODE::getLeft() {
    return left;
}
shared_ptr<GNODE> GNODE::getParent() {
    return parent;
}

shared_ptr<GNODE> GNODE::getRight() {
    return right;
}

string GNODE::getType() {
    return type;
}

void GNODE::setOperator(string p_op) {
    op = p_op;
}



PRINTSTRCONST::PRINTSTRCONST(string p_value) {
    value = p_value;
    this->setType("PRINTSTRCONST");
};
PRINTASCII::PRINTASCII(string p_value) {
    value = p_value;
    this->setType("PRINTASCII");
};



DECLARE::DECLARE(string p_name, string p_datatype) {
    name = p_name;
    datatype = p_datatype;
    this->setType("DECLARE");
};

WHILE::WHILE() {
    this->setType("WHILE");
}

BOOLTERM::BOOLTERM() {
    setType("BOOLTERM");
}


EXPRESSION::EXPRESSION() {
    setType("EXPRESSION");
}

ASSIGN::ASSIGN(string p_name) {
    name = p_name;
    setType("ASSIGN");
}

BLOCK::BLOCK() {
    setType("BLOCK");
}


CONSTANT::CONSTANT(string p_value, string p_datatype)  {
    setType("CONSTANT");
    value = p_value;
    datatype = p_datatype;
}


VARIABLE::VARIABLE(string p_name) {
    name = p_name;
    setType("VARIABLE");
}