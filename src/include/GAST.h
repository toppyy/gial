#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <stack>
using std::vector, std::string, std::shared_ptr, std::make_shared, std::stack;

/* 
    GAST - Gial Abstract Syntax Tree

    Represents a program written in gial as a tree structure which
    consists of nodes (GNODE)

*/


class GNODE {

    public:
        GNODE();
        
        void    setLeft(shared_ptr<GNODE> p_left);
        shared_ptr<GNODE>  getLeft();

        void    setNext(shared_ptr<GNODE> p_node);
        shared_ptr<GNODE>  getNext();

        void    setRight(shared_ptr<GNODE> p_right);
        shared_ptr<GNODE>  getRight();
        
        void    setParent(shared_ptr<GNODE> p_parent);
        shared_ptr<GNODE>  getParent();

        void    setType(string p_type);
        string  getType();

        void setOperator(string p_op);
      
        virtual void print();
        bool isRoot;
        
        bool hasMathOperator();
        void makeRightNull();


        // BOOLTERM
        string op;

        // CONSTANT
        string value;

        // Shared btwn operators
        string datatype;
        string name;
        int size;

    private:        
        shared_ptr<GNODE> right;
        shared_ptr<GNODE> left;
        shared_ptr<GNODE> next;
        shared_ptr<GNODE> parent;
        string type;
};


class GAST {
    public:
        GAST();
//        GNODE root;
        
        vector<shared_ptr<GNODE>> gnodes;

        bool toParent();
        void addAsLeftToCurrent(GNODE node);
        void addAsRightToCurrent(GNODE node);
        void addToCurrent(GNODE node);
        void openBranch();
        void closeBranch();
        void moveToNode(GNODE node);
        void branchFromroot(GNODE node);
        void returnToRoot();
        void branchLeft();
        void branchRight();
        void setLeftAsDefault();
        void unsetLeftAsDefault();

        shared_ptr<GNODE> getRoot();

        shared_ptr<GNODE> current;
        
        stack<shared_ptr<GNODE>> branchStack;
        bool nextToLeft;
        bool nextToRight;
        bool toLeft;
};



class WHILE: public GNODE {
    public:
        WHILE();

};


class IF: public GNODE {
    public:
        IF();
};


class FOR: public GNODE {

};


class REPEAT: public GNODE {
    
};

class ASSIGN: public GNODE {
    public:
        ASSIGN(string p_name);
    
};

class BLOCK: public GNODE {
    /* Just a container: Left is container to block start, right to after the block*/
    public:
        BLOCK();
};

class VARIABLE: public GNODE {
    public:
        VARIABLE(string p_name);
};

class EXPRESSION: public GNODE {
    public:
        EXPRESSION();
};

class BOOLTERM: public GNODE {
    public:
        BOOLTERM();
};

class PRINTSTR: public GNODE {
    public:
        PRINTSTR();
};


class PRINTASCII: public GNODE {
    public:
        PRINTASCII(string p_value);
};

class PRINTINT: public GNODE {
    public:
        PRINTINT();
};
class INPUT: public GNODE {
    public:
        INPUT(string p_name, string p_vartype);
};



class DECLARE: public GNODE {
    public:
        DECLARE(string p_name, string p_vartype);
        DECLARE(string p_name, string p_vartype, int p_size);
};

class ADDOP: public GNODE {
    public:
        ADDOP(string p_op);
};

class CONSTANT: public GNODE {
    public:
        CONSTANT(string p_value, string p_datatype);
};
