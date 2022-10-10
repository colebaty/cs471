#include <iostream>

#include "process.h"

using namespace std;

Process::Process(int id, int numResources) {
    Max = new vector<int>(numResources, 0);
    Allocation = new vector<int>(numResources, 0);
    Need = new vector<int>(numResources, 0);
    _id = id;
}

void Process::allocate(vector<int> assignment) {
    vector<int>::iterator ait = Allocation->begin();
    vector<int>::iterator asit = assignment.begin();

    while (ait != Allocation->end()) {
        *ait += *asit;
        ait++;
        asit++;
    }

}

void Process::display() {
    cout << "Process " << _id << endl;
    cout << "Max: ";
    for (int i = 0; i < Max->size(); i++) {
        cout << Max->at(i)<< " ";
    }
    cout << endl;
    
    cout << "Allocation: ";
    for (int i = 0; i < Allocation->size(); i++) {
        cout << Allocation->at(i)<< " ";
    }
    cout << endl;
    
    cout << "Need: ";
    for (int i = 0; i < Need->size(); i++) {
        cout << Need->at(i)<< " ";
    }
    cout << endl;
    
}