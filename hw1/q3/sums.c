#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 6

int counter = 0;
int sum = 0;
int elem[30] = {
    20, 18, 16, 14, 12, 10, 8, 6, 4, 2, 
    -20, -18, -16, -14, -12, -10, -8, -6, -4, -2,
    2, 6, 10, 14, 18, 22, 26, 30, 34, 38
};

void *PartialSums(void *threadid) {
    int id = (int) (int *) threadid; // cast to int

    if (id < 0 || id >= NUM_THREADS) {
        fprintf(stderr, "Error: invalid thread id %d\n", id);
    }
    else {
        int i, offset = 5;
        for(i = offset * id; i < offset * (id + 1); i++) sum += elem[i];
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t threads[NUM_THREADS];
    int rc, t;

    for(t = 0;t < NUM_THREADS; t++) {
        printf("Creating thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, PartialSums, (void *)t);
        if (rc) {
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    int i;
    for(i = 0; i < NUM_THREADS; i++) pthread_join(threads[i], NULL);

    printf(" Sum: %d\n", sum);
    pthread_exit(NULL);
}
