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
#include <cassert>

using namespace std;

enum algorithm { FIFO, LRU, MRU, OPT };

/* page size, # pages, algorithm, fault percentage */
typedef tuple<int, int, algorithm, long double> record;

int pagesize, numframes;
int * pt; /* page table */
multimap<int, int> * qmap; /* <reference, index in file> */
vector<int> * q; /* queue of references */

void printAll(multimap<int, int>& r) {
    for (const auto& [key, value] : r) {
        cout << "[" << key << "] = " << value << endl;
    }
};

/**
 * @brief optimal page replacement
 * 
 */
void optimal();

/**
 * @brief returns the page table index if the referenced address is contained on a page 
 * which is allocated to the page table. returns -1 otherwise
 * 
 * @param r 
 * @return int 
 */
int isAllocated(int r);

/**
 * @brief returns the logical page number for a given address reference.
 * pages are zero-indexed
 * 
 * @param r a byte-addressed memory reference
 * @return int the logical page on which it is contained
 */
int getLogicalPage(const int& r);

/**
 * @brief print page table
 * 
 */
void printpt();

int main(int argc, char **argv) {

    //TODO: make snippet
    if (argc != 4) {
        printf("usage: %s <page size> <num frames> <input file>\n", argv[0]);
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
    
    auto first = qmap->begin();
    auto last = qmap->end();
    last--;
    printf("first element of multimap: [%d] = %d\n", first->first, first->second);
    printf("first element assigned to page %d\n", getLogicalPage(first->first));
    printf("last element of multimap: [%d] = %d\n", last->first, last->second);
    printf("last element assigned to page %d\n", getLogicalPage(last->first));
    int numpages = 0;
    numpages = ceil((float) last->first / (float) pagesize);
    printf("number of pages: %d\n", numpages);

    cout << "===================" << endl;
    printf("allocating first %d even-indexed entries in queue\n", numframes);
    for (int i = 0; i < numframes; i++) {
        pt[i] = getLogicalPage((*q)[i * 2]);
    }

    cout << "===================" << endl;
    printpt();
    cout << "===================" << endl;

    cout << "testing whether first " << numframes << " queue entries are allocated" << endl
         << " expecting alternating yes/no" << endl;

    for (int i = 0; i < numframes; i++ ) {
        printf("q[%d]: %d %s pt[%d]\n", 
                i, 
                (*q)[i], 
                isAllocated((*q)[i]) > -1 
                    ? "hit" 
                    : "miss", 
                isAllocated((*q)[i])
                );
    }

    /* pointer housekeeping */
    delete qmap;
    delete q;
    // delete pt;
    // delete filename;
    delete in;

    return 0;
}

int isAllocated(int r) {
    for (int i = 0; i < numframes; i++){
        if (pt[i] == getLogicalPage(r)) return i; /* hit */
    }

    return -1; /* miss */
}

int getLogicalPage(const int& r) {
    return floor((float) r / (float) pagesize);
}

void printpt() {
    for (int i = 0; i < numframes; i++) {
        printf("pt[%d] = %d\n", i, pt[i]);
    }
}