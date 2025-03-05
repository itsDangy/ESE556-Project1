#ifndef LinkedList_H
#define LinkedList_H

#include "Node.h"

class linkedlist
{
private:
    linkedlist* next_;
    linkedlist* prev_;
    int nodeID_; // just an index of the respective node in the vector nodes. 
    bool pin_; // to know if a node is a pin or a node(Terminal or non terminal)
public:
    linkedlist(int nodeID, bool pin = false) { 
        nodeID_ = nodeID;
        next_ = prev_ = nullptr;
        pin_ = pin;
    };
    ~linkedlist(){};

    linkedlist* getNext() { return next_; };
    linkedlist* getPrev() { return prev_; };
    int getNodeID() { return nodeID_; };
    void setNext(linkedlist* next) { next_ = next; };
    void setPrev(linkedlist* prev) { prev_ = prev; };
    bool isPin() { return pin_; };
};

#endif