#pragma once
#include <iostream>
#include <vector>

using std::vector, std::string;

/* 
    GAST - Gial Abstract Syntax Tree

    Represents a program written in gial as a tree structure which
    consists of nodes (GNODE)

*/
class GNODE {

    public:
        GNODE();
        void setChild(GNODE* p_child);
        GNODE* getChild();
        GNODE* getParent();
        void setParent(GNODE* p_parent);
        void setType(string p_type);
        string getType();
        void print();
        bool isRoot;

    private:
        
        GNODE* next;
        GNODE* child;
        GNODE* parent;
        string type;

};


class GAST {
    public:
        GAST();
        GNODE root;
        
        vector<GNODE> gnodes;

        void toChild();
        void toParent();
        void addChildToCurrent(GNODE node);
        void moveToNode(GNODE node);
        void branchFromroot(GNODE node);
        void returnToRoot();

        GNODE* current;
};



class IF: public GNODE {

};

class WHILE: public GNODE {

};

class FOR: public GNODE {

};


class REPEAT: public GNODE {
    
};

class ASSIGN: public GNODE {
    
};

class EXPRESSION: public GNODE {
    
};

class COMPARE: public GNODE {
    
};

class PRINT: public GNODE {
    
};