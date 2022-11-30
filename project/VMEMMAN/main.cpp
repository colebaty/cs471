#include <iostream>
#include <iomanip>
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

#define NUM_ALGS 4
enum algorithm { FIFO, LRU, MRU, OPT };

/* page size, # pages, algorithm, fault percentage */
typedef tuple<int, int, algorithm, long double> record;

int pagesize, numframes;
int **frames; /* frames[algorithm][numframes] */
vector<int> * q; /* queue of references */

/**
 * @brief { page#, { index in q, reference} } 
 * 
 */
multimap<int, pair<int, int>> * qmap;

/**
 * @brief indexed by enum algorithm
 * 
 */
int * faults = new int[4];

void printAll(multimap<int, pair<int, int>>& r);

/**
 * @brief returns the page table index if the referenced address is contained on a page 
 * which is allocated to the page table. returns -1 otherwise
 * 
 * @param r 
 * @return int 
 */
int isAllocated(int r, int *pt);

/**
 * @brief returns the logical page number for a given address reference.
 * pages are zero-indexed
 * 
 * @param r a byte-addressed memory reference
 * @return int the logical page on which it is contained
 */
int getLogicalPage(const int& r);

/**
 * @brief print page table of indicated algorithm
 * 
 * @param alg 
 */
void printframes(algorithm alg);

/**
 * @brief returns the index to replace in the page table, which may then be
 * replaced with the page supplied by the record in qmap. "replac the page that 
 * will not be used for the longest period of time"
 * 
 * 
 * @param r the memory reference to be processed
 * @param index the index of the currently processed request
 * @return int the index of the frame to replace in frames
 */
int optimal(int& r, int index);

/**
 * @brief updates the queuemap data datastructure. removes from map all key:value pairs
 * on the interval [0, index]
 * 
 * @param index 
 */
void updateqmap(int index);

int main(int argc, char **argv) {

    //TODO: make snippet
    if (argc != 4) {
        printf("usage: %s <page size> <num frames> <input file>\n", argv[0]);
        return 1;
    }

    pagesize = atoi(argv[1]);
    numframes = atoi(argv[2]);
    char * filename = argv[3];

    frames = new int*[NUM_ALGS]; 
    for (int i = 0; i < NUM_ALGS; i++) {
        frames[i] = new int[numframes];
    }

    int *victim = new int[NUM_ALGS];
    for (int i = 0; i < NUM_ALGS; i++) {
        victim[i] = -1;
    }

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

    in->close();

    cout << "===================" << endl;
    
    ref = -1;
    for (int i = 0; i < q->size(); i++){
        ref = (*q)[i];

        #ifdef DEBUG
        printf("ref: %d %s [%d]\n", ref,
                isAllocated(ref, frames[OPT]) < 0 ? "miss" : "hit",
                getLogicalPage(ref));
        #endif
        
        if ((victim[OPT] = optimal(ref, i)) > -1) { /* miss; replace victim */

            #ifdef DEBUG
            printf("victim[OPT]: %d\n", victim[OPT]);
            cout << "==== hit: frames before ====" << endl;
            printframes();
            cout << endl;
            #endif

            frames[OPT][victim[OPT]] = getLogicalPage(ref);

            #ifdef DEBUG
            cout << "==== hit: frames after ====" << endl;
            printframes();
            cout << "=======================" << endl;
            #endif
        }

        updateqmap(i);

        #ifdef DEBUG
        printf("numrefs: %d | numfaults: %d | page fault ratio: %.3f\n", 
                i + 1, faults[OPT],
                (((double) faults[OPT]) / ((double) (i + 1))));
        cout << "=============" << endl;
        cout << "=======================" << endl << endl;
        #endif
    }

    cout << "=============" << endl;
    printf("numrefs[OPT]: %ld | numfaults[OPT]: %d | page fault ratio[OPT]: %.3f\n", 
            q->size(), faults[OPT], 
            (((double) faults[OPT]) / ((double) (q->size()))));
    cout << "=============" << endl;

    /* pointer housekeeping */
    delete qmap;
    delete q;
    delete faults;
    // delete filename;
    delete in;

    return 0;
}

int isAllocated(int r, int *pt) {
    for (int i = 0; i < numframes; i++){
        if (pt[i] == getLogicalPage(r)) return i; /* hit */
    }

    return -1; /* miss */
}

int getLogicalPage(const int& r) {
    return floor((float) r / (float) pagesize);
}

void printframes(algorithm alg) {
    for (int i = 0; i < numframes; i++) {
        printf("frames[%d] = %d\n", i, frames[alg][i]);
    }
}

void printAll(multimap<int, pair<int, int>>& r) {
    for (const auto& [key, value] : r) {
        printf("[%d]:<%d, %d>\n", key, value.first, value.second);
    }
};

bool isFull(int * frames) {
    for (int i = 0; i < numframes; i++) {
        if (frames[i] < 0) return false;
    }
    return true;
}

void updateqmap(int index) {
    multimap<int, pair<int, int>>::iterator search;

    int page = -1;
    /* remove key:value pairs which are behind the current index */
    for (int i = 0; i < numframes; i++) {
        page = frames[OPT][i];
        #ifdef DEBUG
        auto printr = qmap->lower_bound(page);
        while (printr != qmap->upper_bound(page)) {
            cout << "[" << printr->first << "]: <" 
                 << printr->second.first << ", " 
                 << printr->second.second << ">" << endl;
            printr++;
        }
        #endif
        search = qmap->lower_bound(page);
        /* while this key has values whose index is 
        behind the current position in the queue*/
        while ((search != qmap->end()) && (search->second.first <= index)) {
            qmap->erase(search);
            search = qmap->lower_bound(page);
        }
    }
}

int optimal(int& r, int index) {
    int dist_to_next_ref[numframes];
    for (int i = 0; i < numframes; i++) {
        dist_to_next_ref[i] = -1;
    }
    int victim = -1;

    multimap<int, pair<int, int>>::iterator search;

    if (isAllocated(r, frames[OPT]) < 0) { /* miss */
        if (!isFull(frames[OPT])) { /* inevitable first n = numframes page faults */
            for (int i = 0; i < numframes; i++)
                if (frames[OPT][i] < 0) {
                    victim = i;
                    break;
                }
        }
        else {
            for (int i = 0; i < numframes; i++) {
                search = qmap->lower_bound(frames[OPT][i]);
                if (search != qmap->end()) { /* if there are remaining references in the queue to the allocated page */
                    dist_to_next_ref[i] = search->second.first - index; /* compute the distance between current index and index position of next reference to page */
                }
                else { /* all pages currently allocated to a frame are no longer needed */
                    return index % numframes; /* can be any frame */
                }
            }

            int max = -1;

            for (int i = 0;  i < numframes; i++) {
                if (dist_to_next_ref[i] > max) {
                    max = std::max(max, dist_to_next_ref[i]);
                    victim = i;
                }
            }
        }

        faults[OPT]++;
    }
    /* implied else: hit; do nothing */

    return victim;
}