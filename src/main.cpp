#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "Node.h"

using namespace std;

vector<Node> parseNodes(string filename) {
    ifstream NodeFile(filename);
    if(!NodeFile.is_open())
        cerr << filename << " not found" << endl;

    vector<Node> Nodes;
    char const* digits = "0123456789";
    int numNodes = -1;
    string line;
    int itr;

    getline(NodeFile,line); //gets first line

    //gets Number of Nodes
    while (numNodes == -1) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        itr = line.find_first_of(digits);
        numNodes = stoi(line.substr(itr));
    }
   
    getline(NodeFile,line); //gets numTerminals line (not needed)

    //Puts nodes into vector
    while (Nodes.size()<numNodes) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        stringstream iss(line);
        string nodeID;
        int width;
        int height;
        bool moveable = true;
        iss >> nodeID >> width >> height;
        if(line.find("terminal") != string::npos)
            moveable = false;
        Nodes.push_back(Node(nodeID,width,height,moveable));
    }

    NodeFile.close();
    return Nodes;
}

int main() {
    string benchmark = "superblue18";
    string filepath = "../Benchmarks/" + benchmark + "/" + benchmark;

    vector<Node> Nodes;
    Nodes = parseNodes(filepath+".nodes");
    cout << Nodes.size();

    
    return 0;
}