/**
 * @file main.cpp
 * @author Cole Baty
 * @brief Optimal, LRU, MRU and FIFO implementations of demand paging for 
 * CS 471 at ODU Fall '22
 * @version 0.1
 * @date 2022-11-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */
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
}   *curr, * temp, *before, *after, /* manipulators */
    *top, *bottom, /* stack pointers */
    *head, *tail;  /* queue pointers */

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
 * @brief returns the page table index if the referenced address is contained 
 * on a page which is allocated to the page table. returns -1 otherwise
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
 * @return int replacement frame index; -1 for hits
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
 * @brief returns the index of the frame containing the page representing the
 * reference closest to the head of the input queue. returns -1 for page hits
 * 
 * @param f 
 * @return int replacement frame index; -1 for hits
 */
int fifo(int& ref);

/**
 * @brief returns the index of the frame containing the page representing
 * the refernce closest to the head of the input.
 * 
 * @param f 
 * @return int 
 */
int fifo_victim(int * f);

/**
 * @brief maintains the stack data structure used in both lru and mru algorithms
 * 
 * @param page the referenced page
 */
void updatestack(int page);

/**
 * @brief updates the queuemap data structure. removes from map all 
 * key:value pairs on the interval [0, index]
 * 
 * @param index 
 */
void updateqmap(int index);

/**
 * @brief updates the queue containing the fifo ordering of currently allocated
 * frames
 * 
 * @param index 
 */
void updatequeue(int page);

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

    #ifdef DEBUG
    cout << "===================" << endl;
    cout << "reading input file" << endl;
    cout << "===================" << endl;
    #endif

    /* populate data structures */
    ifstream * in = new ifstream(filename);
    if (!*in) {
        fprintf(stderr, "error: file %s not found\n", filename);
        return -1;
    }
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
    auto np = qmap->end(); /* one-past-last element */
    np--;/* move "back" one to last element */
    numpages = np->first + 1;

    in->close();

    #ifdef DEBUG
    printf("pageSize: %d | numpages: %d | numframes: %d | filename: %s\n", 
            pagesize, numpages, numframes, filename);
    cout << "===================" << endl;
    #endif
    
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
            printframes(OPT);
            cout << "=======================" << endl;
            #endif
        }
        /* implied else: hit; do nothing */

        if ((victim[LRU] = lru(ref)) > -1) { /* miss; replace victim */
            frames[LRU][victim[LRU]] = page;
            #ifdef DEBUG
            cout << "==== hit: frames after ====" << endl;
            printframes(LRU);
            cout << "=======================" << endl;
            #endif
        }
        /* implied else: hit; do nothing */

        if ((victim[MRU] = mru(ref)) > -1) { /* miss; replace victim */
            frames[MRU][victim[MRU]] = page;
            #ifdef DEBUG
            cout << "==== hit: frames after ====" << endl;
            printframes(MRU);
            cout << "=======================" << endl;
            #endif
        }
        /* implied else: hit; do nothing */

        if ((victim[FIFO] = fifo(ref)) > -1) { /* miss; replace victim */
            frames[FIFO][victim[FIFO]] = page;
            #ifdef DEBUG
            cout << "==== hit: frames after ====" << endl;
            printframes(FIFO);
            cout << "=======================" << endl;
            #endif
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
        updatequeue(page);
    }

    /**
     * @brief stub for printStats();
     * TODO: updates stats printed
     * Page Size    # pages     algorithm   page fault ratio
     * 
     */
    printf("Page size\t# of pages\tALG\tpage fault ratio (0 <= p <= 1)\n");
    printf("%d\t\t%d\t\tOPT\t%.3f\n", 
            pagesize, numpages, 
            ((double) faults[OPT]) / ((double) (q->size())));
    printf("%d\t\t%d\t\tLRU\t%.3f\n", 
            pagesize, numpages, 
            ((double) faults[LRU]) / ((double) (q->size())));
    printf("%d\t\t%d\t\tMRU\t%.3f\n", 
            pagesize, numpages, 
            ((double) faults[MRU]) / ((double) (q->size())));
    printf("%d\t\t%d\t\tFIFO\t%.3f\n", 
            pagesize, numpages, 
            ((double) faults[FIFO]) / ((double) (q->size())));

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

int optimal(int& r, int index) {
    int dist_to_next_ref[numframes];
    for (int i = 0; i < numframes; i++) {
        dist_to_next_ref[i] = -1;
    }
    int victim = -1;

    multimap<int, pair<int, int>>::iterator search;

    if (isAllocated(r, frames[OPT]) < 0) { /* miss */
        if (!isFull(frames[OPT])) { /* inevitable first numframes page faults */
            for (int i = 0; i < numframes; i++)
                if (frames[OPT][i] < 0) {
                    victim = i;
                    break;
                }
        }
        else {
            for (int i = 0; i < numframes; i++) {
                search = qmap->lower_bound(frames[OPT][i]);
                /* 
                    if there are remaining references in the queue to the 
                    allocated page 
                */
                if (search != qmap->end()) { 
                    /* 
                        compute the distance between current index and index 
                        position of next reference to page 
                    */
                    dist_to_next_ref[i] = search->second.first - index; 
                }
                /* 
                    all pages currently allocated to a frame are no longer 
                    needed 
                */
                else { 
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

int fifo(int& ref) {
    int victim = -1;
    if(isAllocated(ref, frames[FIFO]) < 0 ) { /* miss */
        if (!isFull(frames[FIFO])) { /* if there are empty frames */
            for (int i = 0; i < numframes; i++) { /* populate */
                if (frames[FIFO][i] == -1) {
                    victim = i;
                    break;
                }
            }
        }
        else { /* select a victim */
            victim = fifo_victim(frames[FIFO]);
        }
        faults[FIFO]++;
    }
    /* implied else: hit; do nothing */

    return victim;
}


void updatestack(int page) {
    /*  search from top. top is MRU, but locality means it's likeley to be on 
        top 
    */
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
                /* if curr not last element */
                if (after != nullptr) after->prev = before; 
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

void updatequeue(int page) {
    if (head != nullptr && tail != nullptr) { /* if queue not empty */
        curr = head;
        while (curr != nullptr) { /* find page */
            if (curr->id == page) { /* page already in queue */
                curr = nullptr; /* reset pointer */
                return;
            }
            curr = curr->next;
        }

        if (curr == nullptr) { /* page not found; add to tail */
            temp = new node();
            temp->id = page;
            tail->next = temp;
            temp->prev = tail;
            tail = temp;
            temp = nullptr;
        }
        else { /* hit: remove the page at the head of the queue */
            temp = head;
            head = head->next;
            delete temp;
        }
    }
    else { /* queue empty; add first node */
        temp = new node();
        temp->id = page;
        head = temp;
        tail = temp;
        temp = nullptr;
    }

}

int fifo_victim(int * f) {
    int victim = -1;

    if (head != nullptr) {
        for (int i = 0; i < numframes; i++) {
            if (f[i] == head->id) {
                victim = i;
                break;
            }
        }
    }

    temp = head;
    head = head->next;
    head->prev = nullptr;
    delete temp;

    return victim;
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