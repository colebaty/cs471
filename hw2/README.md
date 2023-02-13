# Homework 2  - C-style TAILQ implementation of FCFS queue

In this assignment, we implemented a first-come first-served (FCFS) queue 
for processes arriving into the system.  This is also known as a first-in,
first-out (FIFO) queue.

I was familiar with queues from previous coursework, especially [CS 361 Data 
Structures and Algorithms](https://github.com/colebaty/cs361). But I hadn't yet done much in C and 
I wanted to explore that.

The implementation of this program was accomplished mainly by following the
examples provided by the [tailq(3) man
page](https://man7.org/linux/man-pages/man3/tailq.3.html).

One of the key differences I've noticed between programming in C and
programming in C++ is that C++ lends itself more to object-oriented
conceptualization than C.  In C++, the Standard Template Library offers a
pre-built data structure called `queue` which can be manipulated by calls to
member fuctions of the underlying `queue` class.

In C, the same functionality is provided by the predefined `tailq` macros, but
C requires a little different approach, being a more function-oriented
language.

```c
// C++ implementation
#include <queue>

struct Process {
    ...
};

std::queue<Process> q;
q.push(new Process());

// C implementation
#include <sys/queue.h>

//initialize the queue
static TAILQ_HEAD(qname, element) q = TAILQ_HEAD_INITIALIZER(q);

static struct Process {
    ...
    TAILQ_ENTRY(Process) entries; //linked list of queue members
} *p;

p = malloc(sizeof Process);

//add *p to the end of the list of entries in q
TAILQ_INSERT_TAIL(&q, p, entries); 
```
