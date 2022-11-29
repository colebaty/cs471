#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <tuple>
#include <utility>
#include <algorithm>
#include <random>
#include <cmath>
#include <stdio.h>
#include <cassert>

using namespace std;

enum algorithm { FIFO, LRU, MRU, OPT };

/* page size, # pages, algorithm, fault percentage */
typedef tuple<int, int, algorithm, long double> record;

int pagesize, numframes;
vector<int> * pt; /* page table */
multimap<int, pair<int, int>> * qmap; /* { page#, { index in q, reference }}*/
vector<int> * q; /* queue of references */
vector<int>::iterator head;

void printAll(multimap<int, pair<int, int>>& r);

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

/**
 * @brief returns the index to replace in the page table, which may then be
 * replaced with the page supplied by the record in qmap. "replac the page that 
 * will not be used for the longest period of time"
 * 
 * goverened by q::iterator head, which is moved along the queue and feeds each
 * algorithm
 * 
 * @param r 
 * @return int 
 */
int optimal();

int main(int argc, char **argv) {

    //TODO: make snippet
    if (argc != 4) {
        printf("usage: %s <page size> <num frames> <input file>\n", argv[0]);
        return 1;
    }

    pagesize = atoi(argv[1]);
    numframes = atoi(argv[2]);
    char * filename = argv[3];

    pt = new vector<int>(numframes);

    cout << "===================" << endl;
    printf("pageSize: %d | numframes: %d | filename: %s\n", pagesize, numframes, filename);
    cout << "===================" << endl;
    cout << "reading input file" << endl;

    /* populate data structures */
    ifstream * in = new ifstream(filename);
    qmap = new multimap<int, pair<int, int>>();
    q = new vector<int>();
    int ref;
    for (int i = 0; *in >> ref; i++ ) {
        qmap->insert( { getLogicalPage(ref), { i, ref } } );
        q->push_back(ref);
    }

    head = q->begin();

    #ifdef DEBUG
    cout << "contents of ref:" << endl;
    printAll(*qmap);
    #endif

    cout << "===================" << endl;
    
    auto first = qmap->begin();
    auto last = qmap->end();
    last--;
    printf("first element of multimap: { %d, { %d, %d}}\n", 
            first->first, first->second.first, first->second.second);
    printf("first element assigned to page %d\n", getLogicalPage(first->first));
    printf("last element of multimap: { %d, { %d, %d} }\n", 
            last->first, last->second.first, last->second.second);
    printf("last element assigned to page %d\n", getLogicalPage(last->first));
    int numpages = 0;
    numpages = ceil((float) last->first / (float) pagesize);
    printf("number of pages: %d\n", numpages);

    cout << "===================" << endl;
    printf("allocating first %d even-indexed entries in queue\n", numframes);
    for (int i = 0; i < numframes; i++) {
        (*pt)[i] = getLogicalPage((*q)[i * 2]);
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

    while (head != q->end()) {
        printf("next victim: %d\n", optimal());
        head++;
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
        if ((*pt)[i] == getLogicalPage(r)) return i; /* hit */
    }

    return -1; /* miss */
}

int getLogicalPage(const int& r) {
    return floor((float) r / (float) pagesize);
}

void printpt() {
    for (int i = 0; i < numframes; i++) {
        printf("pt[%d] = %d\n", i, (*pt)[i]);
    }
}

void printAll(multimap<int, pair<int, int>>& r) {
    for (const auto& [key, value] : r) {
        printf("[%d]:<%d, %d>\n", key, value.first, value.second);
    }
};

int optimal() {
    vector<int> dist_to_next_ref;
    for (auto frame : *pt) {
        auto target = next(q->begin(), (qmap->lower_bound(frame))->second.first );
        dist_to_next_ref.push_back(distance(head, target));
    }

    #ifdef DEBUG
    assert(dist_to_next_ref.size() == numframes);
    #endif

    auto dist_to_victim = max_element(dist_to_next_ref.begin(), dist_to_next_ref.end());
    auto victim = next(dist_to_next_ref.begin(), *dist_to_victim - 1);

    auto search = qmap->lower_bound(*victim);
    if (search != qmap->end()) {    /* if this key is still populated */
        qmap->erase(search);        /* remove from map */
    }

    return distance(q->begin(), victim);
}