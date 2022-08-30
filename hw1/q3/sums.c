#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#define NUM_THREADS 6

int counter = 0;
int sum = 0;
int elem[30] = {
    20, 18, 16, 14, 12, 
    10, 8, 6, 4, 2, 
    -20, -18, -16, -14, -12, 
    -10, -8, -6, -4, -2,
    2, 6, 10, 14, 18, 
    22, 26, 30, 34
};

void *PartialSums(void *threadid)
{
    int id = (int) threadid; // cast to int

    int i;
    switch(id)
    {
        case 0:
            for(i = 0; i < 5; i++) sum += elem[i];
            break;
            
        case 1:
            for(i = 5; i < 10; i++) sum += elem[i];
            break;

        case 2:
            for (i = 10; i < 15; i++) sum += elem[i];
            break;

        case 3:
            for (i = 15; i < 20; i++) sum += elem[i];
            break;

        case 4:
            for (i = 20; i < 25; i++) sum += elem[i];
            break;

        case 5:
            for(i = 25; i < 30; i++) sum += elem[i];
            break;

        default:
            fprintf(stderr, "Invalid thread id: %d\n", *(int*) threadid);
            break;
    }

    pthread_exit(NULL);

}

int main(int argc, char *argv[])
{
    pthread_t threads[NUM_THREADS];
    int rc, t;

    for(t = 0;t < NUM_THREADS; t++){
        //printf("Creating thread %d\n", t);
        rc = pthread_create(&threads[t], NULL, PartialSums, (void *)t);
        if (rc){
            printf("ERROR; return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        
        pthread_join(threads[t], NULL);
    }


    printf(" Sum: %d\n", sum);
    pthread_exit(NULL);
}
