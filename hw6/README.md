# CS 471 - Operating Systems
## Old Dominion University - Fall '22

## Homework 6 - Problem Description

In this assignment, we were tasked to write a program implementing the least
recently used (LRU) page-replacement algorithm for demand paging of memory. We 
were given a string of page references for a process, as well as sample output.

## Research

In the discussion of the LRU page-replacement algorithm, the textbook authors
describe an implementation using a stack:

>*Stack*. Another approach...is to keep a stack of page numbers. Whenever a
>page is referenced, it is removed from the stack and put on the top. In this
>way, the most recently used page is always at the top of the stack and the
>least recently used page is always at the bottom.

They recommend using a doubly-linked list with a head and tail pointer. I wrote
a very simple `struct node` and used a few more 'utility' pointers to help 
maintain the stack.

``` C++
struct node {
    node * prev = nullptr;
    node * next = nullptr;
    int id;
    }   *top, * bottom, *before,
        *after, *curr, *temp;
```

## Implementation

The heavy lifting in this program is done by the functions `lru` and `ref` -
please see the [source code](lru.cpp) for more details!
