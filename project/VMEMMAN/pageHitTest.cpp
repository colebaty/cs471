#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>
#include <utility>
#include <random>
#include <cmath>
#include <stdio.h>

using namespace std;

enum algorithm { FIFO, LRU, MRU, OPT };

/* page size, # pages, algorithm, fault percentage */
typedef tuple<int, int, algorithm, long double> record;

int * pt; /* page table */
multimap<int, int> * qmap; /* <reference, index in file> */
vector<int> * q; /* queue of references */

void printAll(multimap<int, int>& r) {
    for (const auto& [key, value] : r) {
        cout << "[" << key << "] = " << value << endl;
    }
};

int main(int argc, char **argv) {
    int pagesize, numframes;

    //TODO: make snippet
    if (argc != 4) {
        printf("usage: %s <page size> <num frames> <input file>", argv[0]);
        return 1;
    }

    pagesize = atoi(argv[1]);
    numframes = atoi(argv[2]);
    char * filename = argv[3];

    pt = new int[numframes];

    cout << "===================" << endl;
    printf("pageSize: %d | numframes: %d | filename: %s\n", pagesize, numframes, filename);
    cout << "===================" << endl;
    cout << "reading input file" << endl;

    /* populate data structures */
    ifstream * in = new ifstream(filename);
    qmap = new multimap<int, int>();
    q = new vector<int>();
    int ref;
    for (int i = 0; *in >> ref; i++ ) {
        qmap->insert( { ref, i} );
        q->push_back(ref);
    }

    #ifdef DEBUG
    cout << "contents of ref:" << endl;
    printAll(*qmap);
    #endif

    cout << "===================" << endl;
    
    auto last = qmap->end();
    last--;
    printf("last element of multimap: [%d] = %d\n", last->first, last->second);
    int numpages = 0;
    numpages = ceil((float) last->first / (float) pagesize);
    printf("number of pages: %d\n", numpages);

    random_device r;
    default_random_engine gen(r());

    /* pointer housekeeping */
    delete qmap;
    delete q;
    // delete pt;
    // delete filename;
    delete in;

    return 0;
}
