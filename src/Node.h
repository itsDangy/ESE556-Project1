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
    int area = width_ * height_; // area of the cell.  
    bool terminal;
    
public:
    bool partition; 
    vector<int> index; // a vector of index of nets connected to this given node as they are all sequentially given in the .nets file. 
    Node(string ID, int width, int height, bool moveable = true) {
        ID_ = ID;
        width_ = width;
        height_ = height;
        terminal = moveable;
    };
    ~Node() {};

    string getID() { return ID_; }
    int getWidth() { return width_; }
    int getHeight() { return height_; }
    int getArea() { return area; }
    bool isTerminal() { return terminal; }
    bool Partition(){return partition;} // returns 1 if partition 1. 

};

#endif //Node_H