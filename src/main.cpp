#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

#include "Node.h"
#include "Net.h"
#include "LinkedList.h"

using namespace std;

int numNodes = -1;
int numTerm = -1;
int numNets = -1;
int offset;

struct timePoint { 
    Node lockedNode; 
    int cutSize; 
    float area;
};

vector<Node> parseNodes(string filename) {
    ifstream NodeFile(filename);
    if(!NodeFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<Node> Nodes;
    char const* digits = "0123456789";
    string line;
    bool flag = 0;

    getline(NodeFile,line); //gets first line

    //gets Number of Nodes
    while (numNodes == -1) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        numNodes = stoi(line.substr(line.find_first_of(digits)));
    }
   
    getline(NodeFile,line); //gets NumTerminals line (not needed)
    numTerm = stoi(line.substr(line.find_first_of(digits)));

    //Puts nodes into vector
    while (Nodes.size()<numNodes||!NodeFile.eof()) {
        getline(NodeFile,line);
        if (line.empty() || line[0] == '#') continue;
        stringstream iss(line);
        string nodeID;
        int width;
        int height;
        bool terminal = false;
        iss >> nodeID >> width >> height;
        if(nodeID.find("p") != string::npos&&flag == 0) {
            flag = 1;
            offset = Nodes.size();
        }
        if(line.find("terminal") != string::npos)
            terminal = true;
        Nodes.push_back(Node(nodeID,width,height,terminal));
    }

    NodeFile.close();
    return Nodes;
}

vector<Net> parseNets(string filename, vector<Node>* nodes) {
    ifstream NetFile(filename);
    if(!NetFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<Net> Nets;
    char const* digits = "0123456789";
    string line;

    getline(NetFile,line); //gets first line

    //gets Number of Nets
    while (numNets == -1) {
        getline(NetFile,line);
        if (line.empty() || line[0] == '#') continue;
        numNets = stoi(line.substr(line.find_first_of(digits)));
    }

    getline(NetFile,line); //gets NumPins line (not needed)

    while (Nets.size()<numNets||!NetFile.eof()) {
        getline(NetFile,line);
        if (line.empty() || line[0] == '#') continue;
        stringstream iss(line);
        string t;
        int numNodes;
        string netName;
        vector<int> indicies;
        iss >> t >> t >> numNodes >> netName;
        int netIndex = stoi(netName.substr(1));
        //cout << numNodes << " " << netName << endl;
        for (int i = 0; i < numNodes; i++) {
            getline(NetFile,line);
            if (line.empty() || line[0] == '#') continue;
            stringstream iss(line);
            string nodeName;
            iss >> nodeName;
            int nodeIndex = stoi(nodeName.substr(1));
            if (nodeName.find("p") != string::npos)
            {
                nodeIndex += offset;
                indicies.push_back(nodeIndex);
            }
            else
                indicies.push_back(nodeIndex);
            
            (*nodes)[nodeIndex].addNet(netIndex);
        }
        Net net(netName, indicies);
        Nets.push_back(net);
    }

    NetFile.close();
    return Nets;
}


int calculateAllGains(vector<Node>* Nodes, vector<Net>* Nets) {
    int cutsize = 0;
    bool flag = false;  // Used to see if we should break loop and increment the crossings for the nets

    for (size_t i = 0; i < Nets->size(); i++) {
        int node0Idx = (*Nets)[i].getConnectedNodes()[0];
        for (size_t j = 0; j < (*Nets)[i].getConnectedNodes().size(); j++) {
            // Visit each node on the net and compare it to the first. 
            // If it crosses the partition (XOR)
            // globalCutsize++
            // set the bool cut (in net.h) to true
            // Break the for (j) loop
            int nodejIdx = (*Nets)[i].getConnectedNodes()[j];
            if ((*Nodes)[node0Idx].whichPartition() ^ (*Nodes)[nodejIdx].whichPartition()) {
                cutsize++;
                (*Nets)[i].cut = 1;
                flag = true;
                break;
            }
        }

        // if flag is true
        if (flag == true) {
            for (size_t j = 0; j < (*Nets)[i].getConnectedNodes().size(); j++) {
                // Loop through every node on the net and increment that node's cut size
                int nodejIdx = (*Nets)[i].getConnectedNodes()[j];
                (*Nodes)[nodejIdx].incCrossings();
            }
        }
    }

    cout << "\nTotal Global Cutsize: " << cutsize << endl;
    cout << "offset" << offset << endl;

    // Used for testing 
    int testNode = 5;
    cout << "Net: " << (*Nets)[testNode].getName() << " has " << (*Nets)[testNode].getConnectedNodes().size() << " nodes connected. isCut(): ";
    if ((*Nets)[testNode].cut == 1) {
        cout << "1" << endl;
    } else {
        cout << "0" << endl;
    }

    for (size_t j = 0; j < (*Nets)[testNode].getConnectedNodes().size(); j++) {
        int nodejIdx = (*Nets)[testNode].getConnectedNodes()[j];
        cout << "Connected node [" << nodejIdx << "] has ID: " << (*Nodes)[nodejIdx].getID() << " and is partition: " << (*Nodes)[nodejIdx].whichPartition();
        if (nodejIdx > offset) {
            cout << " which is p" << nodejIdx - offset << endl;
        } else {
            cout << endl;
        }
        cout << "\t" << "Also has a gain of: " << (*Nodes)[nodejIdx].getCrossings() << endl;
    }

    return cutsize;
}

int main() {
    string benchmark = "superblue18";
    string filepath = "../Benchmarks/" + benchmark + "/" + benchmark;

    //Parse the nodes
    vector<Node> Nodes;
    Nodes = parseNodes(filepath+".nodes");
    cout << Nodes.size() << endl;

    vector<Net> Nets;
    Nets = parseNets(filepath+".nets", &Nodes);
    cout << Nets.size() << endl;

    cout << Nodes[45].getID() << endl;
    vector<int> test = Nodes[45].getConnectedNets();
    for (auto i : test) {
        cout << i << " ";
    }

    //Check the area constructor
    // for (int i = 146; i < 166; i++) {
    //     cout << Nodes[i].getArea() << endl;
    // }

    //At this time, we should have everything parsed into the correct data structures

    //Creates the timeline necessary for FM to climb hills and work
    //The index will be the iteration number, while the timePoint struct holds the relevant information for that paticular iteration
    vector<timePoint> timeline;

    //Create the inital cut
    //Randomly assign all nodes a status of either left or right
    srand(time(0));
    for (int i = 0; i < numNodes; i++) {
        Nodes[i].setPartition(rand() % 2);
    }

    //For all terminal nodes, ensure that they will no longer move.


    //Determine the gains, store the current gains in the Nodes structure.
    int currentCutsize = calculateAllGains(&Nodes, &Nets);


    //Create 2 buckets -- left and right
    //This bucket is a unordered map (Hashmap), whos key is an int going form PMAX to -PMAX.
    //The value will be the pointer to a doubly linked list (DLL). The Data for this DLL will be an int, which will
    //reference the index in vector Nodes.

    unordered_map<int, linkedlist*> leftBucket; //Left bucket is considered 0
    unordered_map<int, linkedlist*> rightBucket;    //Right bucket is considered 1

    
    unordered_map<int, linkedlist*> *currentBucket; //Used to effectively swap between the two buckets
    //Seperate the nodes out to the buckets, dependant on parition and gain.
    for (int i = 0; i < numNodes; i++) {
        
        if (Nodes[i].whichPartition() == 1) {
            //If the partition is the left
            currentBucket = &leftBucket;
        } else {
            //If the partition is on the right
            currentBucket = &rightBucket;
        }

        //currentBucket is now set to either the left or right bucket, depending on which parition the current node is
        if((*currentBucket).find(Nodes[i].getCrossings()) == (*currentBucket).end()) {
            //If the key is not present
            // cout << "Bucket: " << currentBucket << " node: " << i << " Crossings: " << Nodes[i].getCrossings() << " Not present" << endl;

            //Create a new DLLnode, whose value is the index of the node
            //Insert this DLL node's address to the hashmap with key of crossings
            linkedlist insertDLLNode(i);
            (*currentBucket)[Nodes[i].getCrossings()] = &insertDLLNode;

        } else {
            //If the key is present
            // cout << "node: " << i << " Crossings: " << Nodes[i].getCrossings() << "present" << endl;

            linkedlist insertDLLNode(i);

            //Get the node at that DLL and follow it until the very end.
            linkedlist* dllNode = (*currentBucket)[Nodes[i].getCrossings()];
            while ((*dllNode).getNext() != nullptr) {
                dllNode = dllNode->getNext();
            }

            //We should be at the last node
            //Set this node to the next DLLNode and attach the pointer references
            (*dllNode).setNext(&insertDLLNode);
            insertDLLNode.setPrev(dllNode);

        }
    }


    //Select the biggest gain and move it to the other side

    //Then, lock it and recalculate gains
    //Repeat

    




    return 0;
}