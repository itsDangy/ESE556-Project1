#ifndef Net_h
#define Net_h

#include <vector>
#include <string>
#include "Node.h"
#include "LinkedList.h"

class Net
{
private:
    vector<string> net_;
    string netName_;
public:
    Net(string netName, vector<string> net){
        netName_ = netName;
        net_ = net;
    };
    ~Net(){};
};

#endif