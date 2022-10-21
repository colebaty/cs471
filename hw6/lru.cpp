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

struct node {
    node * prev = nullptr;
    node * next = nullptr;
    int id;
} *top, * bottom, *temp; 

using namespace std;

void init();

int main()
{
    int frames[3];
    init(); //initiate 

    

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