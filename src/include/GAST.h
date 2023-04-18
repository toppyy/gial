#pragma once
#include <iostream>

/* 
    GAST - Gial Abstract Syntax Tree

    Represents a program written in gial as a tree structure which
    consists of nodes (GNODE)

*/
class GNODE {

    private:
        GNODE* next;
        GNODE* childred;

};


class GAST {
    public:
        GAST();
        GNODE root;
        void setRoot(GNODE rootNode);
    

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