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
#include <vector>

struct node {
    node * prev = nullptr;
    node * next = nullptr;
    int id;
}   *top, * bottom, *before, 
    *after, *curr, *temp; 

using namespace std;

void init();
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

int main()
{
    int size = 3;
    int frames[size];
    for (int i = 0; i < size; i++)
        frames[i] = -1;
    
    vector<int> sequence = { 
        0, 3, 5, 1, 1, 2, 5, 
        4, 3, 2, 1, 2, 5, 4, 
        6, 7, 5, 4, 7, 8, 9, 
        10, 5, 6, 10, 1, 2, 3 
    };

    for (int i : sequence) {
        allocate(i, frames, size);
    }

    return 0;
}

void init() {

    for (int i = 0; i < 12; i++)
    {
        temp = new node();
        temp->id = i;
        if (i == 0) { /* first node */
            top = bottom = temp;
            top->next = bottom;
            bottom->prev = top;
        }
        else {
            temp->prev = bottom;
            bottom->next = temp;
            bottom = temp;
        }
    }
}

void ref(int page) {
    /* search from bottom - LRU */
    if (top != nullptr && bottom != nullptr) { /* if stack not empty */
        /* find page */
        curr = bottom;
        while (curr != nullptr) {
            if (curr->id == page) break;
            curr = curr->prev;
        }

        if (curr != nullptr) { /* if page in stack */
            /* move curr to top */
            /* get neighbors */
            before = curr->prev;
            after = curr->next;

            before->next = after;
            after->prev = before;

            curr->next = top;
            curr->prev = nullptr;
            top->prev = curr;
            top = curr;
        }
        else { /* add to top */
            temp = new node();
            temp->id = page;

            top->prev = temp;
            temp->next = top;
            top = temp;
        }
    }
    else { /* stack empty; add first element */
        temp = new node();
        temp->id = page;
        top = bottom = temp;
    }

}

int lru(int * f, int size) {
    curr = bottom;
    while (curr != nullptr) {
        for (int i = 0; i < size; i++)
            if (f[i] == curr->id) return i;
        
        curr = curr->prev;
    }
}

int contains(int page, int *f, int size) {
    for (int i = 0; i < size; i++)
        if (f[i] == page) return i;

    return -1;
}

void allocate(int page, int * f, int size) {
    if (contains(page, f, size) < 0) { /* if page not already allocated */

    }
    ref(page);
}