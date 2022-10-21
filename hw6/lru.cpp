/**
 * @file lru.cpp
 * @author Cole Baty
 * @brief Stack/doubly-linked list implementaiton for LRU.  Page 417 in textbook
 * describes a method of keeping track of a stack of page numbers.  Whenever a
 * page is referenced, it is removed from the stack and put on the top/head.
 * Therefore, the least recently used page is always at the bottom.
 * 
 * Pages are allocated to one of three frames.
 * @version 0.1
 * @date 2022-10-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include <iostream>
#include <iomanip>
#include <vector>

struct node {
    node * prev = nullptr;
    node * next = nullptr;
    int id;
}   *top, * bottom, *before, 
    *after, *curr, *temp; 

int numFaults = 0;
int numRefs = 0;

using namespace std;

/**
 * @brief LRU stack management. 
 * 
 * @param page 
 */
void ref(int page);

/**
 * @brief finds the index of whichever currently allocated frame was least 
 * recently used
 * 
 * @param f 
 * @param size 
 * @return int  the index of the "victim" frame to be replaced
 */
int lru(int * f, int size);

/**
 * @brief allocates the given page to a frame. if page not already found in frames,
 * issues page fault, and allocates a frame based on LRU
 * 
 * @param page 
 * @param f 
 * @param size 
 */
void allocate(int page, int * f, int size);

/**
 * @brief determines whether a page has already been allocated a frame. 
 * 
 * @param page 
 * @param f 
 * @param size 
 * @return int the index of the frame to which the page has been allocated; -1 otherwise
 */
int contains(int page, int *f, int size);

/**
 * @brief determines if all available frames have been allocated
 * 
 * @param f 
 * @param size 
 * @return true 
 * @return false 
 */
bool full(int *f, int size);

int main(int argc, char **argv)
{
    int size;
    if (argc > 1) 
        size = atoi(argv[1]);
    else 
        size = 3;

    int frames[size];
    for (int i = 0; i < size; i++)
        frames[i] = -1; /* -1 == empty */
    
    vector<int> sequence = { 
        0, 3, 5, 1, 1, 2, 5, 
        4, 3, 2, 1, 2, 5, 4, 
        6, 7, 5, 4, 7, 8, 9, 
        10, 5, 6, 10, 1, 2, 3 
    };

    cout << left 
         << setw(20) << "Page reference #" 
         << setw(20) << "Result" 
         << setw(20) << "Frame allocated to" << endl;

    for (int i : sequence) {
        allocate(i, frames, size);
    }

    cout << endl << "==================" << endl
         << "Total # of references: " << numRefs << endl
         << "Total # of page faults: " << numFaults << endl;

    return 0;
}

void ref(int page) {
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

int lru(int * f, int size) {
    curr = bottom;
    while (curr != nullptr) {
        for (int i = 0; i < size; i++)
            if (f[i] == curr->id) return i;
        
        curr = curr->prev;
    }

    return -1;
}

int contains(int page, int *f, int size) {
    for (int i = 0; i < size; i++)
        if (f[i] == page) return i;

    return -1;
}

void allocate(int page, int * f, int size) {
    cout << setw(20) << page;

    if (contains(page, f, size) > -1) { /* hit */
        cout << setw(20) << "No Page Fault" << "F" << contains(page, f, size) << endl;
    }
    else { /* miss */
        cout << setw(20) << "Page Fault" << "F";
        if (!full(f, size)) { /* if there are empty frames */
            int i;
            for (i = 0; i < size; i++) { /* populate */
                if (f[i] == -1) {
                    f[i] = page;
                    break;
                }
            }
            
            cout << i << endl;
        }
        else { /* select a victim */
            int victim = lru(f, size);
            f[victim] = page;
            cout << victim << endl;
        }

        numFaults++;
    }

    ref(page); /* update stack */
    numRefs++;
}

bool full(int *f, int size) {
    for (int i = 0; i < size; i++)
        if (f[i] < 0) return false; /* if any frame is empty, return false */
    return true;
}