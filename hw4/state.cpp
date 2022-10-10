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