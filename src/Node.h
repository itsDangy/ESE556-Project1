#ifndef Node_h
#define Node_h

#include <string>
#include <vector>
using namespace std;

class Node
{
private:
    string ID_;
    int width_;
    int height_;
    int area; // area of the cell.  
    bool terminal_;
    bool partition; 
    vector<int> connectedNets; // a vector of index of nets connected to this given node as they are all sequentially given in the .nets file.
    
public:

    Node(string ID, int width, int height, bool terminal = false) {
        ID_ = ID;
        width_ = width;
        height_ = height;
        terminal_ = terminal;
        area = width * height;
    };
    ~Node() {};

    string getID() { return ID_; }
    int getWidth() { return width_; }
    int getHeight() { return height_; }
    int getArea() { return area; }
    bool isTerminal() { return terminal_; }
    bool whichPartition() {return partition;} // returns 1 if partition 1. 
    void addNet(int i) { connectedNets.push_back(i); }
    void movePartition() {partition = !partition;} // changes the partition of the node.
    vector<int> getConnectedNets() { return connectedNets; }

};

#endif //Node_H