#ifndef LinkedList_H
#define LinkedList_H

#include "Node.h"

class linkedlist
{
private:
    linkedlist* next_;
    linkedlist* prev_;
    string nodeID_;
public:
    linkedlist(string nodeID) { 
        nodeID_ = nodeID;
        next_ = prev_ = nullptr;
    };
    ~linkedlist(){};

    linkedlist* getNext() { return next_; };
    linkedlist* getPrev() { return prev_; };
    string getNodeID() { return nodeID_; };
    void setNext(linkedlist* next) { next_ = next; };
    void setPrev(linkedlist* prev) { prev_ = prev; };
};

#endif