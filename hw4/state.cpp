#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

#include "state.h"

void State::readFile(string filename) {
    ifstream * filein = new ifstream(filename);
    string input;
    istringstream * linein;

    /* get resource amounts */
    getline(*filein, input);

    linein = new istringstream(input);
    int r;
    while (*linein >> r) {
        Available.push_back(r);
    }


    /* get num processes */
    int p;
    *filein >> p >> ws; 

    /* initialize Allocation */
    vector<int> * row;

    for (int i = 0; i < p; i++) {
        row = new vector<int>;
        for (int j = 0; j < Available.size(); j++)
        {
            row->push_back(0);
        }
        Allocation.push_back(*row);
    }
    
    /* get max request for each process */
    int max = 0;

    string line;
    for (int i = 0; i < p; i++) {
        getline(*filein, line);
        row = new vector<int>;
        linein = new istringstream(line);
        while (*linein >> max) {
            row->push_back(max);
        }
        Max.push_back(*row);
    }

    *filein >> ws;

    cout << "num processes == p : ";
    cout << "true" << endl;

    /* initialize, compute Need = Max - Available*/
    for (int i = 0; i < p; i++) {
        row = new vector<int>;
        for (int j = 0; j < Available.size(); j++) {
            row->push_back(0);
        }
        Need.push_back(*row);
    }

    for (size_t i = 0; i < p; i++) {
        for (size_t j = 0; j < Available.size(); j++) {
            Need[i][j] = Max[i][j] - Allocation[i][j];
        }
    }
    
    /* initialize, populate Requests */
    for (int i = 0; i < p; i++)
    {
        row = new vector<int>;
        for (int j = 0; j < Available.size(); j++)
        {
            row->push_back(0);
        }
        Request.push_back(*row);
        
    }
    
    /* get requests */
    int id, r1, r2;

    for (line; getline(*filein, line); ) {
        linein = new istringstream(line);
        *linein >> id >> r1 >> r2;
        Request[id - 1][0] = r1;
        Request[id - 1][1] = r2;
    }

    filein->close();
}

bool State::safe() {
    vector<bool> Finish(numProcesses, false);

    /* find i such that both */
    for (int i = 0; i < Finish.size(); i++)
    {
        if (!Finish[i] && Need[i] <= Work) {
            Work[0] += Allocation[i][0];
            Work[1] += Allocation[i][1];
            Finish[i] = true;
            continue;
        }

        for (auto it : Finish) {
            /* if any Finish[i] != true */
            if (!it) return false;
        }

    }

    cout << "system is in safe state" << endl;
    return true;
}

void State::request(vector<int>& Request) {
    
    for (int i = 0; i < Request.size(); i++)
    {
        if (Request[i] > Need[i]) {
            cerr << "error: process[" << i+1 << "] has exceeded its maximum claim" << endl;
            return;
        }

        if (Request[i] > Available) {
            cout << "process[" << i+1 << "] must wait; resources not available" << endl;
        }
        else { /* pretend */
            vector<int> tempAvailabl = Available;
            tempAvailabl[0] -= Request[i][0];
            tempAvailabl[1] -= Request[i][1];

            vector<vector<int>> tempAllocation = Allocation;
            tempAllocation[i][0] += Request[i][0];
            tempAllocation[i][1] += Request[i][1];

            vector<vector<int>> tempNeed = Need;
            tempNeed[i][0] -= Request[i][0];
            tempNeed[i][1] -= Request[i][1];

            if (!isSafe(tempAvailabl, Need.size(), tempAllocation, tempNeed)) {
                cout << "process[" << i+1 << "] must wait; restoring original state" << endl;
            }
            else {
                cout << "request may proceed " << endl;

                Available = tempAvailabl;

                Allocation[i][0] = tempAllocation[i][0];
                Allocation[i][1] = tempAllocation[i][1];

                Need[i][0] = tempNeed[i][0];
                Need[i][1] = tempNeed[i][1];

                Request[i][0] = 0;
                Request[i][1] = 0;
            }
        }

    }
}