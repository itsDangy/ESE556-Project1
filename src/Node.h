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
    bool partition;     //This is going to be left or right (0 or left, 1 for right)
    int crossings;      //This is used to determine the gain sizes
    vector<int> connectedNets; // a vector of index of nets connected to this given node as they are all sequentially given in the .nets file.
    

public:

    Node(string ID, int width, int height, bool terminal = false) {
        ID_ = ID;
        width_ = width;
        height_ = height;
        terminal_ = terminal;
        area = width * height;
        crossings = 0;
    };
    ~Node() {};

    string getID() { return ID_; }
    int getWidth() { return width_; }
    int getHeight() { return height_; }
    int getArea() { return area; }
    bool isTerminal() { return terminal_; }
    void addNet(int i) { connectedNets.push_back(i); }
    
    vector<int>& getConnectedNets() { return connectedNets; }

    //Related to partitions
    void movePartition() {partition = !partition;} // changes the partition of the node.
    bool whichPartition() {return partition;} // returns 1 if partition 1. 
    void setPartition(bool side) {partition = side;}    //Sets the partition to whatever the input is

    //Used to calculate gains
    void incCrossings() {crossings++;}
};

#endif //Node_H