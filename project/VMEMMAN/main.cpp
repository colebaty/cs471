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

/**
 * @brief nodes for stack/doubly linked list LRU implementation. see section
 * 9.4.4 in text for clarification on use of stack structure
 * 
 */
struct node {
    node * prev = nullptr;
    node * next = nullptr;
    int id; /* page */
}   *top, * bottom, *before, 
    *after, *curr, *temp; 

int pagesize, numframes, numpages;
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

/**
 * @brief debug function to print the queue map
 * 
 * @param r 
 */
void printqmap(multimap<int, pair<int, int>>& r);

/**
 * @brief returns the page table index if the referenced address is contained on a page 
 * which is allocated to the page table. returns -1 otherwise
 * 
 * @param r 
 * @return int 
 */
int isAllocated(int r, int *pt);

/**
 * @brief returns true if all frames frames contain a page, false otherwise
 * 
 * @param frames 
 * @return true 
 * @return false 
 */
bool isFull(int * frames);

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
 * replaced with the page supplied by the record in qmap. "replace the page that 
 * will not be used for the longest period of time"
 * 
 * 
 * @param r the memory reference to be processed
 * @param index the index of the currently processed request
 * @return int the index of the frame to replace in frames
 */
int optimal(int& r, int index);

/**
 * @brief for page faults, returns the index of the frame containing the least
 * recently used page. returns -1 for page hits.
 * 
 * @param ref 
 * @return int replacement frame index; -1 for hits
 */
int lru(int& ref);

/**
 * @brief returns the index of the frame containing the least-recently used 
 * page. begins searching from bottom of the stack
 * 
 * @param f 
 * @return int 
 */
int lru_victim(int * f);

/**
 * @brief for page faults, returns the index of the frame containing the most
 * recently used page. returns -1 for page hits
 * 
 * @param ref 
 * @return int replacement frame indes; -1 for hits
 */
int mru(int& ref);

/**
 * @brief returns the index of the frame containing the most-recently used
 * page.  begins searching from top of the stack
 * 
 * @param f 
 * @return int 
 */
int mru_victim(int * f);

/**
 * @brief maintains the stack data structure used in both lru and mru algorithms
 * 
 * @param page the referenced page
 */
void updatestack(int page);

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

    /* initialize frames */
    frames = new int*[NUM_ALGS]; 
    for (int i = 0; i < NUM_ALGS; i++) {
        frames[i] = new int[numframes];
        for (int j = 0; j < numframes; j++) {
            frames[i][j] = -1;
        }
    }

    int *victim = new int[NUM_ALGS];
    for (int i = 0; i < NUM_ALGS; i++) {
        victim[i] = -1;
    }

    cout << "===================" << endl;
    cout << "reading input file" << endl;
    cout << "===================" << endl;

    /* populate data structures */
    ifstream * in = new ifstream(filename);
    qmap = new multimap<int, pair<int, int>>();
    q = new vector<int>();
    int ref;
    for (int i = 0; *in >> ref; i++ ) {
        qmap->insert( { getLogicalPage(ref), { i, ref } } );
        q->push_back(ref);
    }

    /* 
        get number of pages. the key of very last element in qmap is the 
        zero-indexed number of the highest-numbered inserted page. 
        mathematically, i know that the number of pages is
            = ceil( largest reference in q / page size)
    */
    auto np = qmap->end();
    np--;
    numpages = np->first + 1;

    in->close();

    printf("pageSize: %d | numpages: %d | numframes: %d | filename: %s\n", 
            pagesize, numpages, numframes, filename);
    cout << "===================" << endl;
    
    ref = -1;
    int page = -1;
    for (int i = 0; i < q->size(); i++){
        ref = (*q)[i];
        page = getLogicalPage(ref);

        #ifdef DEBUG
        printf("ref: %d %s [%d]\n", ref,
                isAllocated(ref, frames[OPT]) < 0 ? "miss" : "hit",
                getLogicalPage(ref));
        #endif
        
        if ((victim[OPT] = optimal(ref, i)) > -1) { /* miss; replace victim */

            frames[OPT][victim[OPT]] = page;

            #ifdef DEBUG
            cout << "==== hit: frames after ====" << endl;
            printframes();
            cout << "=======================" << endl;
            #endif
        }
        /* implied else: hit; do nothing */

        if ((victim[LRU] = lru(ref)) > -1) { /* miss; replace victim */
            frames[LRU][victim[LRU]] = page;
        }
        /* implied else: hit; do nothing */

        if ((victim[MRU] = mru(ref)) > -1) { /* miss; replace victim */
            frames[MRU][victim[MRU]] = page;
        }
        /* implied else: hit; do nothing */

        #ifdef DEBUG
        printf("numrefs: %d | numfaults: %d | page fault ratio: %.3f\n", 
                i + 1, faults[OPT],
                (((double) faults[OPT]) / ((double) (i + 1))));
        cout << "=============" << endl;
        cout << "=======================" << endl << endl;
        #endif
        updateqmap(i);
        updatestack(page);
    }

    /**
     * @brief stub for printStats();
     * TODO: updates stats printed
     * Page Size    # pages     algorithm   page fault ratio
     * 
     */
    printf("numrefs[OPT]: %ld | numfaults[OPT]: %d | page fault ratio[OPT]: %.3f\n", 
            q->size(), faults[OPT], 
            (((double) faults[OPT]) / ((double) (q->size()))));
    printf("numrefs[LRU]: %ld | numfaults[LRU]: %d | page fault ratio[LRU]: %.3f\n", 
            q->size(), faults[LRU], 
            (((double) faults[LRU]) / ((double) (q->size()))));
    printf("numrefs[MRU]: %ld | numfaults[MRU]: %d | page fault ratio[MRU]: %.3f\n", 
            q->size(), faults[MRU], 
            (((double) faults[MRU]) / ((double) (q->size()))));


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

void printqmap(multimap<int, pair<int, int>>& r) {
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

int lru(int& ref) {
    int victim = -1;
    if(isAllocated(ref, frames[LRU]) < 0 ) { /* miss */
        if (!isFull(frames[LRU])) { /* if there are empty frames */
            for (int i = 0; i < numframes; i++) { /* populate */
                if (frames[LRU][i] == -1) {
                    victim = i;
                    break;
                }
            }
        }
        else { /* select a victim */
            victim = lru_victim(frames[LRU]);
        }
        faults[LRU]++;
    }
    /* implied else: hit; do nothing */

    return victim;
}

int mru(int& ref) {
    int victim = -1;
    if(isAllocated(ref, frames[MRU]) < 0 ) { /* miss */
        if (!isFull(frames[MRU])) { /* if there are empty frames */
            for (int i = 0; i < numframes; i++) { /* populate */
                if (frames[MRU][i] == -1) {
                    victim = i;
                    break;
                }
            }
        }
        else { /* select a victim */
            victim = mru_victim(frames[MRU]);
        }
        faults[MRU]++;
    }
    /* implied else: hit; do nothing */

    return victim;
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

void updatestack(int page) {
    /* search from top. top is MRU, but locality means it's likeley to be on top */
    if (top != nullptr && bottom != nullptr) { /* if stack not empty */
        /* find page */
        curr = top;
        while (curr != nullptr) {
            if (curr->id == page) break;
            curr = curr->next;
        }

        if (curr != nullptr) { /* if page in stack */
            if (curr != top) { /* move curr to top */
                /* get neighbors */
                before = curr->prev;
                after = curr->next;

                before->next = after;
                if (after != nullptr) after->prev = before; /* if curr not last element */
                if (curr == bottom) bottom = before;

                curr->next = top;
                curr->prev = nullptr;
                top->prev = curr;
                top = curr;
                curr = before = after = nullptr;
            }
        }
        else { /* add to top */
            temp = new node();
            temp->id = page;

            top->prev = temp;
            temp->next = top;
            top = temp;
            temp = nullptr;
        }
    }
    else { /* stack empty; add first element */
        temp = new node();
        temp->id = page;
        top = temp;
        bottom = temp;
        temp = nullptr;
    }

}

int lru_victim(int * f) {
    curr = bottom;
    while (curr != nullptr) {
        for (int i = 0; i < numframes; i++)
            if (f[i] == curr->id) return i;
        
        curr = curr->prev;
    }

    return -1;
}

int mru_victim(int * f) {
    curr = top;
    while (curr != nullptr) {
        for (int i = 0; i < numframes; i++)
            if (f[i] == curr->id) return i;
        
        curr = curr->next;
    }

    return -1;
}