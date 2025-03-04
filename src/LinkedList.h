#ifndef LinkedList_H
#define LinkedList_H

#include "Node.h"

class linkedlist
{
private:
    linkedlist* next_;
    linkedlist* prev_;
    Node* node_;
public:
    linkedlist(Node* node) { 
        node_ = node;
        next_ = prev_ = nullptr;
    };
    ~linkedlist(){};

    linkedlist* getNext() { return next_; };
    linkedlist* getPrev() { return prev_; };
    Node* getNode() { return node_; };
    void setNext(linkedlist* next) { next_ = next; };
    void setPrev(linkedlist* prev) { prev_ = prev; };
};

#endif