#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>
#include <map>
#include <limits.h>

#include "Node.h"
#include "Net.h"
#include "LinkedList.h"

using namespace std;

int numNodes = -1;
int numTerm = -1;
int numNets = -1;
int offset;

struct timePoint { 
    int lockedNode; 
    int cutSize; 
    float ratio;
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
    // making a copy and not deleting this just yet. 
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

float getAreaRatio(vector<Node> Nodes){
    int leftArea, rightArea; 
    for (int i = 0; i < numNodes; i++) {
        if (Nodes[i].whichPartition() == 0) {
            //leftNodes.push_back(Nodes[i].getID());
            leftArea += Nodes[i].getArea();
        } else {
            //rightNodes.push_back(Nodes[i].getID());
            rightArea += Nodes[i].getArea();
        }
    }
    float ratio = (float)leftArea/rightArea;
    return ratio; 
}

int calculateCrossings(vector<Node>* Nodes, vector<Net>* Nets) {
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
                    flag = true;
                }
            }
            (*Nodes)[nodejIdx].setCrossings(crossings);
        }
        //We consider cutsize on a per net basis, so it goes here
        if (flag) {
            cutsize++;
        }
    }
    return cutsize;
}

void storeInBuckets(map<int, linkedlist*>* leftBucket, map<int, linkedlist*>* rightBucket, vector<Node>* Nodes, int* lSize, int* rSize) {
    *lSize = 0;
    *rSize = 0;
    
    map<int, linkedlist*> *currentBucket; //Used to effectively swap between the two buckets
    //Seperate the nodes out to the buckets, dependant on parition and gain.
    for (int i = 0; i < numNodes; i++) {
        
        if ((*Nodes)[i].whichPartition() == 1) {
            //If the partition is the left
            currentBucket = leftBucket;
            (*lSize)++;
        } else {
            //If the partition is on the right
            currentBucket = rightBucket;
            (*rSize)++;
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

void fmpass(vector<Node>* Nodes, vector<Net>* Nets) {
    //For all terminal nodes, ensure that they will no longer move.

    //Determine the gains, store the current gains in the Nodes structure.
    int cutsize = calculateCrossings(Nodes, Nets);

    //Creates the timeline necessary for FM to climb hills and work
    //The index will be the iteration number, while the timePoint struct holds the relevant information for that paticular iteration
    vector<timePoint> timeline;
    int lowestCutsize = INT_MAX;    //This will be used and updated as we find the lowest cutsize

    //Create 2 buckets -- left and right
    //This bucket is a unordered map (Hashmap), whos key is an int going form PMAX to -PMAX.
    //The value will be the pointer to a doubly linked list (DLL). The Data for this DLL will be an int, which will
    //reference the index in vector Nodes.
    map<int, linkedlist*> leftBucket; //Left bucket is considered 0
    map<int, linkedlist*> rightBucket;    //Right bucket is considered 1
    map<int, linkedlist*>* chosenBucket;

    int lBucketSize = 0;
    int rBucketSize = 0;

    storeInBuckets(&leftBucket, &rightBucket, Nodes, &lBucketSize, &rBucketSize);
    cout << "Left Size: " << lBucketSize << " R Size: " << rBucketSize << endl;
    
    /*
        Hereinlies the FM algorithm
        Repeat the following until both LBucket and RBucket is empty
        Choose the side with the largest key (and thus the largest gain) Using myMap.begin()->first
        Loop to the end of the linked list
        Remove that cell from the bucket and place it on the timeline
        move that cell to the other side
        For all nets connected to that cell:
            For all nodes connected to that net:
                Recalculate the node crossings (This can be a -1 if the node is now on the same side, +1 if the node is now on the other side)
                Remove the node form the ebucket
                Place the node in the reclaulted gain bucket
    */
    int gainSelector = 0;    //0 if left, 1 if right

    while (leftBucket.size() != 0 || rightBucket.size() != 0) {

        //Choose the side with the largest key
        if (leftBucket.rbegin()->first - rightBucket.rbegin()->first >= 0) {
            //Left bucket is bigger
            gainSelector = leftBucket.rbegin()->first;
            chosenBucket = &leftBucket;
        } else {
            //Right bucket is bigger
            gainSelector = rightBucket.rbegin()->first;
            chosenBucket = &rightBucket;
        }

        cout << "gain is: " << gainSelector << endl;

        //Loop to the end of the linked list
        linkedlist* dllNode = (*chosenBucket)[gainSelector];
        int selectedNode = -1;
        if (dllNode == nullptr) {
            exit(EXIT_FAILURE);
        }

        
        //Loop to the end of the node
        while (dllNode->getNext() != nullptr) {
            dllNode = dllNode->getNext();
        }
        //At this point we are at the last node
        selectedNode = (*dllNode).getNodeID(); 
        
        linkedlist* prevNode = dllNode->getPrev();
        if (prevNode != nullptr) {
            prevNode->setNext(nullptr);
        }
        free(dllNode);

        struct timePoint point;
        point.lockedNode = selectedNode;
        point.cutSize = cutsize;
        point.ratio = getAreaRatio(*Nodes); 
        timeline.push_back(point);
        cout <<"cutsize before pass" << cutsize << endl;

        bool inc_cutsize, dec_cutsize; // flags that indicate if the cutsize should be incremented or decremented. 

        (*Nodes)[selectedNode].movePartition();
        //i refers to the index of the current net
        for (int i = 0; i < (*Nodes)[selectedNode].getConnectedNets().size(); i++) {
            int currentNet = (*Nodes)[selectedNode].getConnectedNets()[i];
            //j refers to the index of the current node
            inc_cutsize = false; 
            dec_cutsize = false; 
            for (int j = 0; j < (*Nets)[currentNet].getConnectedNodes().size(); j++) {
                int oldGain = 2*((*Nodes)[j].getCrossings()) - (*Nodes)[j].getConnectedNets().size();

                //Find the jnode and remove it from the bucket structure
                //Loop to the end of the linked list
                linkedlist* nodeUpdater = (*chosenBucket)[oldGain];
                if (nodeUpdater == nullptr) {
                    exit(EXIT_FAILURE);
                }

                //Loop until the end of the node
                //Or until we find the index we're looking for
                int lookingFor = (*Nets)[currentNet].getConnectedNodes()[j];
                while (nodeUpdater->getNodeID() != lookingFor) {
                    if (nodeUpdater->getNext() != nullptr) {
                        exit(EXIT_FAILURE);
                    }
                    nodeUpdater = nodeUpdater->getNext();
                }
                //nodeUpdater now has the node we're trying to move
                //Update the DLL to remove nodeUpdater from the area

                //If nodeUpdater.getPrev() is nullptr, it is the first.
                //If nodeUpdater.getNext() is nullptr, it is the last.
                //If it is both, remove the key entierly from the map
                linkedlist* nodeUpdaterPrev = nodeUpdater->getPrev();
                linkedlist* nodeUpdaterNext = nodeUpdater->getNext();

                //Set nodeupdater to default nullptrs for both
                (*nodeUpdater).setPrev(nullptr);
                (*nodeUpdater).setNext(nullptr);

                if (nodeUpdaterPrev == nullptr && nodeUpdaterNext == nullptr) {
                    (*chosenBucket).erase(oldGain);
                } else if (nodeUpdaterPrev == nullptr && nodeUpdaterNext != nullptr) {
                    //This is the first node, link this to the head in the key
                    (*chosenBucket)[oldGain] = nodeUpdaterNext;
                } else if (nodeUpdaterPrev != nullptr && nodeUpdaterNext == nullptr) {
                    //This is the last node, simply remove the previous connection
                    (*nodeUpdaterPrev).setNext(nullptr);
                } else {
                    //Standard case, simply stitch the DLL back together
                    (*nodeUpdaterPrev).setNext(nodeUpdaterNext);
                    (*nodeUpdaterNext).setPrev(nodeUpdaterPrev);
                }



                if ((*Nodes)[selectedNode].whichPartition() ^ (*Nodes)[j].whichPartition()) {
                    //They are different sides
                    // a net will only be cut when it goes up from a 0 to 1. 
                    if ((*Nodes)[j].getCrossings() == 0){inc_cutsize = true;}
                    (*Nodes)[j].incCrossings();

                } else {
                    //they are same sides
                    // a net can only be uncut when the number of crossing go down from a 1 to a 0. 
                    if((*Nodes)[j].getCrossings() == 1){dec_cutsize = true; }
                    (*Nodes)[j].decCrossings();
                }
                int newGain = 2*((*Nodes)[j].getCrossings()) - (*Nodes)[j].getConnectedNets().size();


                //Place the selected node back into the correct bucket
                if ((*chosenBucket).find(newGain) != (*chosenBucket).end()) {
                    //If the key exists, add it to the beginning of the linked list
                    nodeUpdaterNext = (*chosenBucket)[newGain]; //Here, I'm reusing nodeUpdaterNext
                    (*chosenBucket)[newGain] = nodeUpdater;
                    (*nodeUpdater).setNext(nodeUpdaterNext);

                } else {
                    //If the key does not exist, add it to the map
                    (*chosenBucket)[newGain] = nodeUpdater;
                }
            }

            if(inc_cutsize == true){cutsize++;}
            if(dec_cutsize == true){cutsize--;}   
        }
        cout<< "cutsize after the pass" << cutsize << endl; 
    }


    //Here, all cells are now fixed and emptied out of the buckets.
    //Roll back the changes from the timeline one by one until we've reached the lowest cutsize during this iteration
    //Lowest cutsize is found in variable lowestCutsize, so just keep rolling back until we've gotten to a point in history
    //Where that is the case
    for (int i = timeline.size(); i > 0; i--) {
        if (timeline[i].cutSize == lowestCutsize) {
            break;
        }
        //Otherwise flip the node and continue
        (*Nodes)[timeline[i].lockedNode].movePartition();
    }

    //This is the most efficient cutsize of the current FM pass
}

int main() {
    string benchmark = "superblue18";

    cout << "Which benchmark would you like to run? (Blank will run superblue18)" << endl;
    cin >> benchmark;

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

    

    //Create the inital cut
    //Randomly assign all nodes a status of either left or right
    srand(time(0));
    for (int i = 0; i < numNodes; i++) {
        Nodes[i].setPartition(rand() % 2);
    }

    fmpass(&Nodes, &Nets);
    



    // writeOutput(ofilepath,currentCutsize,Nodes);
    return 0;
}