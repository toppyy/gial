#include "./include/GAST.h"


GAST::GAST() : gnodes(vector<shared_ptr<GNODE>> {}),
    current(nullptr),
    toLeft(false),
    nextToLeft(false),
    nextToRight(false),
    branchStack(stack<shared_ptr<GNODE>>()) {
    GNODE root = GNODE();
    root.isRoot = true;
    root.setType("root");
    shared_ptr<GNODE> ptr = make_shared<GNODE>(root);
    current = ptr;
    gnodes.push_back(ptr);
    
};


void GAST::openBranch() {
    branchStack.push(current);
}

void GAST::closeBranch() {
    current = branchStack.top();
    branchStack.pop();
}

void GAST::addAsLeftToCurrent(GNODE node) {
    node.setParent(current);
    gnodes.push_back(make_shared<GNODE>(node));
    current->setLeft(make_shared<GNODE>(node));
}

void GAST::setLeftAsDefault() {
    toLeft = true;
}
void GAST::unsetLeftAsDefault() {
    toLeft = false;
}



void GAST::addToCurrent(GNODE node) {
    node.setParent(current);
    shared_ptr<GNODE> ptr = make_shared<GNODE>(node);
    node.setParent(current->getParent());
    gnodes.push_back(ptr);
    if (nextToLeft) {
        current->setLeft(ptr);
        nextToLeft = false;
    }
    else if (toLeft) {
        current->setLeft(ptr);
    }  
    else if (nextToRight) {
        current->setRight(ptr);
        nextToRight = false;
    }
    else {
        current->setNext(ptr);
    }
    
    current = ptr;
}

void GAST::moveToNode(GNODE node) {

}
void GAST::branchFromroot(GNODE node) {

}
void GAST::returnToRoot() {
    current = gnodes[0];
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

void GAST::branchLeft() {
    nextToLeft = true;
    openBranch();
}


void GAST::branchRight() {
    nextToRight = true;
    openBranch();
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
    name(""),
    size(1)
    {

}

void GNODE::makeRightNull() {
    right = nullptr;
}

void GNODE::setLeft(shared_ptr<GNODE> p_left) {
    left = p_left;
}

void GNODE::setNext(shared_ptr<GNODE> p_node) {
    next = p_node;
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
    std::cout <<
        this->getType() <<
        tab << "parent: " << getNodeType(parent) <<
        tab << "right: "  << getNodeType(right) <<
        tab << "left: " << getNodeType(left) << 
        tab << "next: " << getNodeType(next)
        ;
    
    if (name != "") {
        std::cout << " and my name is " << name;
    }
    std::cout << "\n";
    
}

bool GNODE::hasMathOperator() {
    
    if (op == "+" | op == "-" | op == "*" | op == "/") {
        return true;
    }
    return false;
}

shared_ptr<GNODE> GNODE::getLeft() {
    return left;
}

shared_ptr<GNODE> GNODE::getNext() {
    return next;
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
    setType("PRINTSTRCONST");
};
PRINTASCII::PRINTASCII(string p_value) {
    value = p_value;
    setType("PRINTASCII");
};



DECLARE::DECLARE(string p_name, string p_datatype) {
    name = p_name;
    datatype = p_datatype;
    setType("DECLARE");
};


INPUT::INPUT(string p_name, string p_datatype) {
    name = p_name;
    datatype = p_datatype;
    setType("INPUT");
};

WHILE::WHILE() {
    setType("WHILE");
}

IF::IF() {
    setType("IF");
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

PRINTINT::PRINTINT() {
    setType("PRINTINT");
}

ADDOP::ADDOP(string p_op) {
    op = p_op;
    setType("ADDOP");
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