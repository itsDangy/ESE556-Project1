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

void writeOutput(string filename, int cutsize, vector<Node> Nodes) {
    ofstream OutputFile(filename);
    if(!OutputFile.is_open()) {
        cerr << filename << " not found" << endl;
        exit(EXIT_FAILURE);
    }

    vector<string> leftNodes;
    vector<string> rightNodes;
    int leftArea = 0;
    int rightArea = 0;

    OutputFile << "Cutsize: " << cutsize << endl;
    for (int i = 0; i < numNodes; i++) {
        if (Nodes[i].whichPartition() == 0) {
            leftNodes.push_back(Nodes[i].getID());
            leftArea += Nodes[i].getArea();
        } else {
            rightNodes.push_back(Nodes[i].getID());
            rightArea += Nodes[i].getArea();
        }
    }
    float ratio = (float)leftArea/rightArea;
    OutputFile << "Partition Ratio: " << ratio << endl;

    int totalArea = leftArea + rightArea;
    float leftPercent = (float)leftArea/totalArea * 100;
    float rightPercent = (float)rightArea/totalArea * 100;

    OutputFile << "Partition 1: " << leftNodes.size() << "\tArea: " << leftArea << " - " << leftPercent << "%" << endl;
    for (auto i : leftNodes) {
        OutputFile << i << endl;
    }

    OutputFile << "Partition 2: " << rightNodes.size() << "\tArea: " << rightArea << " - " << rightPercent << "%" << endl;
    for (auto i : rightNodes) {
        OutputFile << i << endl;
    }

    OutputFile.close();
}

void calculateCrossings(vector<Node>* Nodes, vector<Net>* Nets) {
    int cutsize = 0;
    bool flag = false;  // Used to see if we should break loop and increment the crossings for the nets

    //This is going to be O(n^3) because I'm spending too much brainpower trying to figure out how to minimize it
    //Our goal is to find the number of net crossings that occur for each node
    //We already know the total number of nets for each node
    //Noncrossings = totalNets - crossings
    //Gain = crossings - noncrossings
    //Therefore: gain = 2*crossings - totalNets
    
    //We will spend O(n^3)
    //Loop through all nets
        //Loop through each node on the net
            //Compare this to every other node (besides itself) and count the number of crossings
        //Store that number into node.crossings
    for (size_t i = 0; i < Nets->size(); i++) {
        for (size_t j = 0; j < (*Nets)[i].getConnectedNodes().size(); j++) {
            int crossings = 0;
            int nodejIdx = (*Nets)[i].getConnectedNodes()[j];
            for (size_t k = 0; k < (*Nets)[i].getConnectedNodes().size(); k++) {
                int nodekIdx = (*Nets)[i].getConnectedNodes()[k];
                if (j == k) break;
                if ((*Nodes)[nodejIdx].whichPartition() ^ (*Nodes)[nodekIdx].whichPartition()) {
                    //In here, there is a crossing.
                    crossings++;
                }
            }
            (*Nodes)[nodejIdx].setCrossings(crossings);
        }
    }







    // for (size_t i = 0; i < Nets->size(); i++) {
    //     int node0Idx = (*Nets)[i].getConnectedNodes()[0];
    //     for (size_t j = 0; j < (*Nets)[i].getConnectedNodes().size(); j++) {
    //         // Visit each node on the net and compare it to the first. 
    //         // If it crosses the partition (XOR)
    //         // globalCutsize++
    //         // set the bool cut (in net.h) to true
    //         // Break the for (j) loop
    //         int nodejIdx = (*Nets)[i].getConnectedNodes()[j];
    //         if ((*Nodes)[node0Idx].whichPartition() ^ (*Nodes)[nodejIdx].whichPartition()) {
    //             cutsize++;
    //             flag = true;
    //             break;
    //         }
    //     }

    //     // if flag is true
    //     if (flag == true) {
    //         for (size_t j = 0; j < (*Nets)[i].getConnectedNodes().size(); j++) {
    //             // Loop through every node on the net and increment that node's cut size
    //             int nodejIdx = (*Nets)[i].getConnectedNodes()[j];
    //             (*Nodes)[nodejIdx].incCrossings();
    //         }
    //     }
    // }

    // cout << "\nTotal Global Cutsize: " << cutsize << endl;
    // cout << "offset" << offset << endl;

    // // Used for testing 
    // int testNode = 5;
    // cout << "Net: " << (*Nets)[testNode].getName() << " has " << (*Nets)[testNode].getConnectedNodes().size() << " nodes connected." << endl;

    // for (size_t j = 0; j < (*Nets)[testNode].getConnectedNodes().size(); j++) {
    //     int nodejIdx = (*Nets)[testNode].getConnectedNodes()[j];
    //     cout << "Connected node [" << nodejIdx << "] has ID: " << (*Nodes)[nodejIdx].getID() << " and is partition: " << (*Nodes)[nodejIdx].whichPartition();
    //     if (nodejIdx > offset) {
    //         cout << " which is p" << nodejIdx - offset << endl;
    //     } else {
    //         cout << endl;
    //     }
    //     cout << "\t" << "Also has a gain of: " << (*Nodes)[nodejIdx].getCrossings() << endl;
    // }

    // return cutsize;
}

void storeInBuckets(unordered_map<int, linkedlist*>* leftBucket, unordered_map<int, linkedlist*>* rightBucket, vector<Node>* Nodes) {
    unordered_map<int, linkedlist*> *currentBucket; //Used to effectively swap between the two buckets
    //Seperate the nodes out to the buckets, dependant on parition and gain.
    for (int i = 0; i < numNodes; i++) {
        
        if ((*Nodes)[i].whichPartition() == 1) {
            //If the partition is the left
            currentBucket = leftBucket;
        } else {
            //If the partition is on the right
            currentBucket = rightBucket;
        }

        //Calculate the gain (See function calculateCrossings for more information)
        //gain = 2*crossings - totalNets
        int gain = 2*(*Nodes)[i].getCrossings() - (*Nodes)[i].getConnectedNets().size();

        //Create a new DLLnode, whose value is the index of the node
        // Allocate memory on the heap so it persists after the loop iteration
        linkedlist* insertDLLNode = new linkedlist(i);


        //currentBucket is now set to either the left or right bucket, depending on which parition the current node is
        if ((*currentBucket).find(gain) == (*currentBucket).end()) {
            //If the key is not present

            //Insert this DLL node's address to the hashmap with key of crossings
            (*currentBucket)[gain] = insertDLLNode;
        } else {
            //If the key is present

            //Get the node at that DLL and follow it until the very end.
            linkedlist* dllNode = (*currentBucket)[gain];
            if (dllNode != nullptr) {
                while (dllNode->getNext() != nullptr) {
                    dllNode = dllNode->getNext();
                }
                //We should be at the last node
                //Set this node to the next DLLNode and attach the pointer references
                dllNode->setNext(insertDLLNode);
                insertDLLNode->setPrev(dllNode);
            }
        }
    }
}

int main() {
    string benchmark = "superblue18";
    string ifilepath = "../Benchmarks/" + benchmark + "/" + benchmark;
    string ofilepath = "../Output/" + benchmark + "_Output.txt";

    //Parse the nodes
    vector<Node> Nodes;
    Nodes = parseNodes(ifilepath+".nodes");
    cout << Nodes.size() << endl;

    vector<Net> Nets;
    Nets = parseNets(ifilepath+".nets", &Nodes);
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

    //Create 2 buckets -- left and right
    //This bucket is a unordered map (Hashmap), whos key is an int going form PMAX to -PMAX.
    //The value will be the pointer to a doubly linked list (DLL). The Data for this DLL will be an int, which will
    //reference the index in vector Nodes.

    unordered_map<int, linkedlist*> leftBucket;
    unordered_map<int, linkedlist*> rightBucket;

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
    calculateCrossings(&Nodes, &Nets);

    //Create 2 buckets -- left and right
    //This bucket is a unordered map (Hashmap), whos key is an int going form PMAX to -PMAX.
    //The value will be the pointer to a doubly linked list (DLL). The Data for this DLL will be an int, which will
    //reference the index in vector Nodes.
    unordered_map<int, linkedlist*> leftBucket; //Left bucket is considered 0
    unordered_map<int, linkedlist*> rightBucket;    //Right bucket is considered 1

    storeInBuckets(&leftBucket, &rightBucket, &Nodes);
    
    /*
        Hereinlies the FM algorithm
        Repeat the following until both LBucket and RBucket is empty

    
    */

    //Select the biggest gain and move it to the other side

    //Then, lock it and recalculate gains
    //Repeat

    



    writeOutput(ofilepath,currentCutsize,Nodes);
    return 0;
}