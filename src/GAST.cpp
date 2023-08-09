#include "./include/GAST.h"



GAST::GAST() :
    toLeft(false),
    nextToLeft(false),
    nextToRight(false),
    branchStack(stack<shared_ptr<GNODE>>())
    {
    current = make_shared<GNODE>();
    current->isRoot = true;
    current->setType("root");
    root = current;
};




void GAST::openBranch() {
    branchStack.push(current);
}

void GAST::closeBranch() {
    current = branchStack.top();
    branchStack.pop();
}


void GAST::setLeftAsDefault() {
    toLeft = true;
}
void GAST::unsetLeftAsDefault() {
    toLeft = false;
}


void GAST::addToCurrent(shared_ptr<GNODE> node) {
    
    node->setParent(current);    
    
    if (nextToLeft) {
        current->setLeft(node);
        nextToLeft = false;
    }
    else if (nextToRight) {
        current->setRight(node);
        nextToRight = false;
    }
    else if (toLeft) {
        current->setLeft(node);
    }  
    
    else {
        current->setNext(node);
    }
    current = node;
}



bool GAST::toParent() {
    if (current->isRoot) {
        return false;
    }
    current = current->getParent();
    return true;
}

shared_ptr<GNODE> GAST::getRoot() {
    return root;
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
    next(nullptr),
    left(nullptr),
    type("unknown"),
    op(""),
    value(""),
    datatype(""),
    name(""),
    size(1),
    info(map<string,string>())
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
    return parent.lock();
}

shared_ptr<GNODE> GNODE::getRight() {
    return right;
}

string GNODE::getType() {
    return type;
}

string GNODE::getFromInfo(string key) {
    if (auto search = info.find(key); search != info.end())
        return search->second;
    else
        throw std::runtime_error("Node (" + type +  ") info does not have key " + key + "\n");
}


void GNODE::setOperator(string p_op) {
    op = p_op;
}


DECLARE::DECLARE(string p_name, string p_datatype) {
    name = p_name;
    datatype = p_datatype;
    setType("DECLARE");
};

DECLARE::DECLARE(string p_name, string p_datatype, int p_size) {
    name = p_name;
    datatype = p_datatype;
    size = p_size;
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

FOR::FOR(string p_name, map<string,string> p_info) {
    name = p_name;
    info = p_info;
    setType("FOR");
}

BOOLTERM::BOOLTERM() {
    setType("BOOLTERM");
}
BOOLEXPR::BOOLEXPR() {
    setType("BOOLEXPR");
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


PRINTSTR::PRINTSTR() {
    setType("PRINTSTR");
}

PRINTASCII::PRINTASCII(string p_value) {
    value = p_value;
    setType("PRINTASCII");
};



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