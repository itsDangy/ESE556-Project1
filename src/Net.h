#ifndef Net_h
#define Net_h

#include "Node.h"
#include "LinkedList.h"

class Net
{
private:
    linkedlist* net_;
    string netName_;
public:
    Net(string netName, linkedlist* net){
        netName_ = netName;
        net_ = net;
    };
    ~Net(){};
};

#endif