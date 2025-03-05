#ifndef Net_h
#define Net_h

#include <vector>
#include <string>
#include "Node.h"
#include "LinkedList.h"

class Net
{
private:
    vector<int> net_; // This points to the root node (The first not)
    string netName_;
public:
    bool cut; // 1 if the net is cut by the partition and 0 if the net is not cut by the partition. (iterate throught the linkedlist to see if it is cut or not)
    
    Net(string netName, vector<int> net){
        netName_ = netName;
        net_ = net;
    };
    ~Net(){};
};

#endif