#include <iostream> 
#include <fstream> // writing the nodes and the nets to a file. 
#include <cstdlib>
using namespace std; 


void generateNodes(int nodes, string filename){
    int numPins; // number of p nodes 
    int terminals; // numer of terminals.
    int npt ; //  non pin terminals. 



    numPins = rand() % (int)(nodes/3) + 1; // at most a third of the nodes can be pins. 

    npt = rand()% (int)((nodes-numPins)/5);// the number of o nodes that are terminals. 
    terminals = numPins +npt; 
    std :: ofstream nodesFile(filename); // open the file 
    if (!nodesFile.is_open()) {
        cout << "Can't open file" << endl; 
        exit(1); 
    }
    
    if(nodesFile.is_open()){
        nodesFile << "UCLA nodes 1.0" << endl; 
        nodesFile << "NumNodes : " << nodes << endl; 
        nodesFile << "NumTerminals : "<< terminals << endl; 

        for(int i = 0; i < nodes-(numPins+npt); i++){ // nodes - numPins are 0 nodes. 
            nodesFile <<"o"<<i<<"\t"<< rand()%11 + 1 <<"\t" << rand()%11 + 1<< endl; // o pins, the pins that can either be movable or non movable and start with o  
        }
        for (int i =0; i < npt ; i++){
            nodesFile <<"o"<<i<<"\t"<< rand()%11 + 1 <<"\t" << rand()%11 + 1<<"\t"<< "terminal"<< endl;
        }

        for (int i=0; i < numPins; i++){
            nodesFile <<"p"<<i<<"\t"<< rand()%11 + 1 <<"\t" << rand()%11 +1 << "\t" << "terminal_NI" << endl; // p pins. 
        }
    }

    nodesFile.close();
}

//void generateNets (int nets)
int main (){
    string benchmark = "Test";
    string filePath = "../Benchmarks/" + benchmark + "/" + benchmark; 
    int numNodes; 
    int numNets; // user intputs for num nodes and num nets. 

     
    std :: ofstream netFile(filePath+".nets"); 

    cout <<"Enter the number of nodes" << endl;   
    cin >> numNodes; 
    cout << "Enter the number of nets in the graph" << endl;
    cin  >> numNets; 
    generateNodes(numNodes, filePath+".nodes"); 
}