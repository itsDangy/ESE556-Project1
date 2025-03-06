#ifndef Net_h
#define Net_h

#include <vector>
#include <string>
#include "Node.h"
#include "LinkedList.h"

class Net
{
private:
    vector<int> connectedNodes_; // This vector contains the index for the nodes connected to the net
    string netName_;
public:
    Net(string netName, vector<int> net){
        netName_ = netName;
        connectedNodes_ = net;
    };
    ~Net(){};
    string getName() {return netName_;}
    vector<int>& getConnectedNodes() { return connectedNodes_; }
};

#endif