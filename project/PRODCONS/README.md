# PRODCONS
### for CS471, Dr. Ravi Mukkamala, Fall 2022, Old Dominion University

A thread-based implementation of the classic [producer-consumer
problem](https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem).
There are a finite number of producers and consumers who share a common buffer
of finite size. Producers deposit a resource into the shared buffer, from which 
consumers draw resources.  A producer may only deposit to empty slots in the
buffer, and consumers may only draw from occupied slots in the buffer.

We were required to implement this with a thread library of our choosing. The
instructions specified that we were to compare runtimes with each of the following
parameters:

```
Number of producers     Number of consumers     Buffer size
2                       2                       5
5                       5                       10
10                      10                      100
```

That is, run the program with 2 producer threads, 2 consumer threads, and a
buffer size of 5.  Then run the program with 2 producer threads, 2 consumer
threads, and a buffer size of 10.  Then, <2, 2, 100>, <5, 2, 5>, <5, 2, 10>,
and so on.

## Threading

I chose to implement this in C++ but using the C
[`pthreads`](https://man7.org/linux/man-pages/man7/pthreads.7.html) and
[`semaphore`](https://man7.org/linux/man-pages/man7/sem_overview.7.html)
libraries.  While the C++20 standard does provide support for
[semaphores](https://en.cppreference.com/w/cpp/thread\#Semaphores), these were
incompatible with the project because the number of required sempahores was not
able to be declared dynamically. Thread management was (somehow) also more
easily acomplished with the `pthreads` library.

## Scripting
For ease of sample collection (and also for grading purposes) I included two
short scripts:
- `run.sh`: compiles the program if needed; runs all prescribed samples, saving
  each sample run to its own output file
- `stats.sh`: collects the data from each sample and consolidates it into a
  `.csv` file for further analysis

