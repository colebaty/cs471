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

enum algorithm { FIFO, LRU, MRU, OPT };

/* page size, # pages, algorithm, fault percentage */
typedef tuple<int, int, algorithm, long double> record;

int pagesize, numframes;
int * opt_frames; /* optimal page table */
multimap<int, pair<int, int>> * qmap; /* { page#, { index in q, reference }}*/
vector<int> * q; /* queue of references */

int opt_faults = 0;

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
 * @brief print page table
 * 
 */
void printopt_frames();

/**
 * @brief returns the index to replace in the page table, which may then be
 * replaced with the page supplied by the record in qmap. "replac the page that 
 * will not be used for the longest period of time"
 * 
 * 
 * @param r the memory reference to be processed
 * @param index the index of the currently processed request
 * @return int the index of the frame to replace in opt_frames
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

    opt_frames = new int[numframes];

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
    numpages = ceil((float) last->second.second / (float) pagesize);
    printf("number of pages: %d\n", numpages);

    cout << "===================" << endl;
    printf("allocating first %d even-indexed entries in queue\n", numframes);
    for (int i = 0; i < numframes; i++) {
        opt_frames[i] = getLogicalPage((*q)[i * 2]);
    }

    cout << "===================" << endl;
    printopt_frames();
    cout << "===================" << endl;

    cout << "testing whether first " << numframes << " queue entries are allocated" << endl
         << " expecting alternating yes/no" << endl;

    for (int i = 0; i < numframes; i++ ) {
        printf("q[%d]: %d %s opt_frames[%d]\n", 
                i, 
                (*q)[i], 
                isAllocated((*q)[i], opt_frames) > -1 
                    ? "hit" 
                    : "miss", 
                isAllocated((*q)[i], opt_frames)
        );
    }

    cout << "===================" << endl;
    cout << "resetting page table(s)" << endl;
    delete opt_frames;
    opt_frames = new int[numframes];
    for (int i = 0; i < numframes; i++) {
        opt_frames[i] = -1;
    }

    #ifdef DEBUG
    cout << "===================" << endl;
    printf("allocating first %ld requests in q\n", q->size());
    // printf("allocating first %d requests in q\n", numpages/4);
    cout << "q: ";
    for (int i = 0; i < q->size(); i++){
    // for (int i = 0; i < numpages / 4; i++) {
        if (i % 5 == 0) cout << endl;
        cout << "[" << (*q)[i] << "]: " << getLogicalPage((*q)[i]) << "\t";
    }
    cout << endl;
    
    cout << "===================" << endl;
    cout << endl;
    #endif

    int opt_victim = -1;
    ref = -1;
    for (int i = 0; i < q->size(); i++){
    // for (int i = 0; i < numpages / 4; i++){
        cout << "=======================" << endl;
        ref = (*q)[i];
        printf("ref: %d %s [%d]\n", ref,
                isAllocated(ref, opt_frames) < 0 ? "miss" : "hit",
                getLogicalPage(ref));
        if ((opt_victim = optimal(ref, i)) > -1) { /* miss; replace victim */
            cout << endl;
            printf("opt_victim: %d\n", opt_victim);
            cout << "==== hit: opt_frames before ====" << endl;
            printopt_frames();
            cout << endl;
            opt_frames[opt_victim] = getLogicalPage(ref);
            cout << "==== hit: opt_frames after ====" << endl;
            printopt_frames();
            cout << "=======================" << endl;
        }

        updateqmap(i);

        printf("numrefs: %d | numfaults: %d | page fault ratio: %.3f\n", 
                i + 1, opt_faults, 
                (((double) opt_faults) / ((double) (i + 1))));
        cout << "=============" << endl;
        cout << "=======================" << endl << endl;
    }

    cout << "=============" << endl;
    printf("numrefs: %ld | numfaults: %d | page fault ratio: %.3f\n", 
            q->size(), opt_faults, 
            (((double) opt_faults) / ((double) (q->size()))));
    cout << "=============" << endl;

    /* pointer housekeeping */
    delete qmap;
    delete q;
    // delete opt_frames;
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

void printopt_frames() {
    for (int i = 0; i < numframes; i++) {
        printf("opt_frames[%d] = %d\n", i, opt_frames[i]);
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
        page = opt_frames[i];
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

    if (isAllocated(r, opt_frames) < 0) { /* miss */
        if (!isFull(opt_frames)) { /* inevitable first n = numframes page faults */
            for (int i = 0; i < numframes; i++)
                if (opt_frames[i] < 0) {
                    victim = i;
                    break;
                }
        }
        else {
            for (int i = 0; i < numframes; i++) {
                search = qmap->lower_bound(opt_frames[i]);
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

        opt_faults++;
    }
    /* implied else: hit; do nothing */


    return victim;
}