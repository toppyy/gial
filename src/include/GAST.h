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
        
        void    setLeft(shared_ptr<GNODE>p_left);
        shared_ptr<GNODE>  getLeft();

        void    setRight(shared_ptr<GNODE>p_right);
        shared_ptr<GNODE>  getRight();
        
        void    setParent(shared_ptr<GNODE>p_parent);
        shared_ptr<GNODE>  getParent();

        void    setType(string p_type);
        string  getType();
      
        virtual void print();
        bool isRoot;

    private:
        
        shared_ptr<GNODE> right;
        shared_ptr<GNODE> left;
        shared_ptr<GNODE> parent;
        string type;

};


class GAST {
    public:
        GAST();
//        GNODE root;
        
        vector<shared_ptr<GNODE>> gnodes;

        bool toLeft();
        bool toParent();
        void addAsLeftToCurrent(GNODE node);
        void addAsRightToCurrent(GNODE node);
        void addToCurrent(GNODE node);
        void openBranch();
        void closeBranch();
        void moveToNode(GNODE node);
        void branchFromroot(GNODE node);
        void returnToRoot();

        shared_ptr<GNODE> getRoot();

        shared_ptr<GNODE> current;
        
        stack<shared_ptr<GNODE>> branchStack;
        bool nextToLeft;
};



class IF: public GNODE {

};

class WHILE: public GNODE {
    public:
        WHILE();

};

class FOR: public GNODE {

};


class REPEAT: public GNODE {
    
};

class ASSIGN: public GNODE {
    
};

class BLOCK: public GNODE {
    /* Just a container: Left is container to block start, right to after the block*/
    public:
        BLOCK();
};

class VARIABLE: public GNODE {
    public:
        VARIABLE(string p_name);
        string name;
};

class EXPRESSION: public GNODE {
    public:
        EXPRESSION();
};

class BOOLTERM: public GNODE {
    public:
        BOOLTERM();
        string op;
        void setOperator(string p_op);
};

class PRINTSTRCONST: public GNODE {
    public:
        PRINTSTRCONST(string p_whatToPrint);
        string whatToPrint;
};

class DECLARE: public GNODE {
    public:
        DECLARE(string p_name, string p_vartype);

        string name;
        string vartype;
};

class CONSTANT: public GNODE {
    public:
        CONSTANT(string p_value, string p_datatype);

        string value;
        string datatype;

};
