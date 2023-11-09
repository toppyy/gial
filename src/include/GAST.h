#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <stack>
#include <map>
using   std::vector,
        std::string,
        std::shared_ptr,
        std::unique_ptr,
        std::weak_ptr,
        std::make_shared,
        std::make_unique,
        std::stack,
        std::map
        ;

/* 
    GAST - Gial Abstract Syntax Tree

    Represents a program written in gial as a tree structure which
    consists of nodes (GNODE)

*/


class GNODE {

    public:
        GNODE();
        
        void    setLeft(uint32_t p_left);
        uint32_t getLeft();

        void    setNext(uint32_t p_node);
        uint32_t  getNext();

        void    setRight(uint32_t p_right);
        uint32_t  getRight();
        
        void    setParent(uint32_t p_parent);
        uint32_t  getParent();

        void    setType(string p_type);
        string  getType();

        string getFromInfo(string key);

        void setOperator(string p_op);
    
        bool isRoot;
        
        bool hasMathOperator();
        void makeRightNull();

        // Shared btwn operators
        string datatype;
        string name;
        string op;
        string value;
        int size;

        // A key-value store for operator specific needs
        map<string, string> info;

        // A vector for operator specific needs
        vector<string> infoVector;

        int id;


    private:        
        uint32_t right;
        uint32_t left;
        uint32_t next;
        uint32_t parent;
        string type;
};


class GAST {
    public:
        GAST();
        
        void addToCurrent(shared_ptr<GNODE>  node);
        void openBranch();
        void closeBranch();
        void branchLeft();
        void branchRight();
        void setLeftAsDefault();
        void unsetLeftAsDefault();

        shared_ptr<GNODE> getNode(uint32_t nodeidx);

        shared_ptr<GNODE> getRoot();
        shared_ptr<GNODE> getRight(shared_ptr<GNODE> ptr);
        shared_ptr<GNODE> getLeft(shared_ptr<GNODE> ptr);
        shared_ptr<GNODE> getParent(shared_ptr<GNODE> ptr);
        shared_ptr<GNODE> getNext(shared_ptr<GNODE> ptr);
        shared_ptr<GNODE> getCurrent();
        uint32_t current;
        uint32_t nodecount;
 
        
        stack<uint32_t> branchStack;
        
        bool nextToLeft;
        bool nextToRight;
        bool toLeft;

        vector<shared_ptr<GNODE>> nodes;
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
    public:
        FOR(string p_name, map<string,string> p_info);
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

class BOOLEXPR: public GNODE {
    public:
        BOOLEXPR();
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

class FUNCTIONCALL: public GNODE {
    public:
        FUNCTIONCALL(string p_name, vector<string> p_arguments) ;
};
