# VMEMMAN

### for CS471, Dr. Ravi Mukkamala, Fall 2022, Old Dominion Univrsity

## Background information
In this project, we were tasked to implement four different demand paging
replacement algorithms.

In the virtual memory model, program memory is partitioned into equally-sized
blocks of memory, called pages.  The size of the pages may vary.  Because 
physical memory is finite, it is not necessary to load an entire program into
memory; rather, only the pages containing memory being actively worked on are
loaded into 'frames' of physical memory, with the unneeded parts being written
to successively lower cache levels for retrieval.

However, this means that there will inevitably be a page fault when a requested
memory address is not currently loaded into a physical frame on the machine. In
this situation, the correct page must be retrieved from storage, which is a
costly, time-intensive operation.

Thus the incentive to optimize a page replacement algorithm.  We were tasked to
implement and compare the following four algorithms:
* First in, first out (FIFO) - replace the page which arrived first in the
  queue
* Least recently used (LRU) - replace the page which has not been referenced
  for the longest time
* Most recently used (MRU) - replace the page which has been referenced most
  recently
* Optimal - replace the page that will not be used for the longest time

## Implementation
Obviously, the optimal replacement algorithm requires foreknowledge of the
memory references; therefore, implementing the optimal replacement algorithm
serves to provide a "perfect world" baseline aganst which to compare the other
three algorithms.

[FIFO]() and [LRU]() were implemented in previous homework assignments, and so
were easily adapted to this assignment. For MRU, I used the same doubly-linked
stack structure as LRU, which did not require much modification.

### Optimal

For the optimal replacement algorithm, I chose the [`multimap`]() from the C++
STL:

```C++
/**
 * @brief { page#, { index in q, reference} }. keyed on page numbers. this was
 * chosen to exploit the property of multimaps that items with the same key
 * are partitioned together in the order they were inserted. this means that
 * each partition is effectively a FIFO queue, the head of which can be reached
 * in O(log n) by a call to qmap->lower_bound(page).
 *
 */

```

## Scripts
As in [PRODCONS](../prodcons/), I provided two scripts to help with
compilation, running the program, collecting samples, and converting the sample
data to a CSV file for further analysis in spreadsheet applications.
