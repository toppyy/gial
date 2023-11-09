#include "./include/GAST.h"



GAST::GAST() :
    toLeft(false),
    nextToLeft(false),
    nextToRight(false),
    branchStack(stack<uint32_t>()),
    nodes(vector<shared_ptr<GNODE>> {}),
    current(0),
    nodecount(0)
    {
    
    nodes.push_back(nullptr);
    current++;
    nodecount++;
    nodes.push_back(make_shared<GNODE>());
    nodes[current]->id = nodecount;
    nodes[current]->isRoot = true;
    nodes[current]->setType("root");

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


void GAST::addToCurrent(shared_ptr<GNODE>  node) {
    

    if (nextToLeft) {
        nodes[current]->setLeft(nodecount + 1);
        nextToLeft = false;
    }
    else if (nextToRight) {
        nodes[current]->setRight(nodecount + 1);
        nextToRight = false;
    }
    else if (toLeft) {
        nodes[current]->setLeft(nodecount + 1);
    }  
    
    else {
        nodes[current]->setNext(nodecount + 1);
    }
    
    node->setParent(current);
    nodecount++;
    current = nodecount;
    node->id = nodecount;
    nodes.push_back(node);

}


shared_ptr<GNODE> GAST::getNode(uint32_t nodeidx) {
    return nodes[nodeidx];
}

shared_ptr<GNODE> GAST::getRight(shared_ptr<GNODE> ptr) {
    return nodes[ptr->getRight()];
}

shared_ptr<GNODE> GAST::getLeft(shared_ptr<GNODE> ptr) {
    return nodes[ptr->getLeft()];
}

shared_ptr<GNODE> GAST::getParent(shared_ptr<GNODE> ptr) {
    return nodes[ptr->getParent()];
}

shared_ptr<GNODE> GAST::getNext(shared_ptr<GNODE> ptr) {
    return nodes[ptr->getNext()];
}

shared_ptr<GNODE> GAST::getCurrent() {
    return nodes[current];
}


shared_ptr<GNODE> GAST::getRoot() {
    return nodes[1];
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
    right(0),
    next(0),
    left(0),
    parent(0),
    id(0),
    type("unknown"),
    op(""),
    value(""),
    datatype(""),
    name(""),
    size(1),
    info(map<string,string>()),
    infoVector(vector<string>())
    {

}

void GNODE::makeRightNull() {
    right = 0;
}

void GNODE::setLeft(uint32_t p_left) {
    left = p_left;
}

void GNODE::setNext(uint32_t p_node) {
    next = p_node;
}

void GNODE::setParent(uint32_t p_parent) {
    parent = p_parent;
}

void GNODE::setRight(uint32_t p_right) {
    right = p_right;
}

void GNODE::setType(string p_type) {
    type = p_type;
}

bool GNODE::hasMathOperator() {
    
    if (op == "+" | op == "-" | op == "*" | op == "/") {
        return true;
    }
    return false;
}

uint32_t GNODE::getLeft() {
    return left;
}

uint32_t GNODE::getNext() {
    return next;
}
uint32_t GNODE::getParent() {
    return parent;
}

uint32_t GNODE::getRight() {
    return right;
}

string GNODE::getType() {
    return type;
}

string GNODE::getFromInfo(string key) {
    auto search = info.find(key);
    
    if (search == info.end()) {
        throw std::runtime_error("Node (" + type +  ") info does not have key " + key + "\n");
    }

    return search->second;    
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

FUNCTIONCALL::FUNCTIONCALL(string p_name, vector<string> p_arguments)  {
    setType("FUNCTIONCALL");
    name = p_name;
    infoVector = p_arguments;
}