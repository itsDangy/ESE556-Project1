#ifndef Node_h
#define Node_h

#include <string>
using namespace std;

class Node
{
private:
    string ID_;
    int width_;
    int height_;
    bool moveable_;

public:
    Node(string ID, int width, int height, bool moveable = true) {
        ID_ = ID;
        width_ = width;
        height_ = height;
        moveable_ = moveable;
    };
    ~Node() {};

    string getID() { return ID_; }
    int getWidth() { return width_; }
    int getHeight() { return height_; }
    int getArea() { return (width_ * height_) ; }
    bool isMoveable() { return moveable_; }

};

#endif //Node_H