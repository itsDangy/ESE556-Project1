#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "Node.h"
#include "Net.h"
#include "LinkedList.h"

using namespace std;

vector<Node> parseNodes(string filename) {
    ifstream NodeFile(filename);
    if(!NodeFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<Node> Nodes;
    char const* digits = "0123456789";
    int numNodes = -1;
    string line;

    getline(NodeFile,line); //gets first line

    //gets Number of Nodes
    while (numNodes == -1) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        numNodes = stoi(line.substr(line.find_first_of(digits)));
    }
   
    getline(NodeFile,line); //gets NumTerminals line (not needed)

    //Puts nodes into vector
    while (Nodes.size()<numNodes||!NodeFile.eof()) {
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

vector<Net> parseNets(string filename, vector<Node> nodes) {
    ifstream NetFile(filename);
    if(!NetFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<Net> Nets;
    char const* digits = "0123456789";
    int numNets = -1;
    string line;

    getline(NetFile,line); //gets first line

    //gets Number of Nets
    while (numNets == -1) {
        getline(NetFile,line);
        if (line.empty() || line[0] == '#') continue;
        numNets = stoi(line.substr(line.find_first_of(digits)));
    }

    getline(NetFile,line); //gets NumPins line (not needed)
    getline(NetFile,line);
    getline(NetFile,line);
    stringstream iss(line);
    string t;
    int numNodes;
    string netName;
    iss >> t >> t >> numNodes >> netName;
    cout << numNodes << " " << netName << endl;
    for (int i = 0; i < numNodes; i++) {
        getline(NetFile,line);
        stringstream iss(line);
        string nodeName;
        iss >> nodeName;
        
    }

    // while (Nets.size()<numNets||!NetFile.eof()) {
    //     getline(NetFile,line);
    //     if (line.empty() || line[0] == '#') continue;

    // }

    NetFile.close();
    return Nets;
}

int main() {
    string benchmark = "superblue18";
    string filepath = "../Benchmarks/" + benchmark + "/" + benchmark;

    vector<Node> Nodes;
    Nodes = parseNodes(filepath+".nodes");
    cout << Nodes.size() << endl;

    vector<Net> Nets;
    Nets = parseNets(filepath+".nets", Nodes);
    //cout << Nets.size() << endl;

    return 0;
}