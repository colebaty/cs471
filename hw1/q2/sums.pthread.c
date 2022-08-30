#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int sum1; // shared by threads
int sum2; // shared by threads

void *sigma(void *param); // sum of integers from {1, 2, ..., n}
void *squares(void *param); // sum of squares of {1, 2, ..., n}

int main(int argc, char *argv[])
{
    pthread_t tid_sigma, tid_squares; // thread ids
    pthread_attr_t attr; // default thread attributes

    int n = atoi(argv[1]);

    pthread_attr_init(&attr);

    pthread_create(&tid_sigma, &attr, sigma, &n);
    pthread_create(&tid_squares, &attr, squares, &n);

    pthread_join(tid_sigma, NULL);
    pthread_join(tid_squares, NULL);

    printf("The sum of the first %d natural numbers is: %d\n", n, sum1);
    printf("The sum of the squares of the first %d natural numbers is: %d\n", n, sum2);

    return 0;

}

void *sigma(void *param)
{
    int n = *(int*) param; //cast to int pointer, then dereference
    sum1 = (n * (n + 1)) / 2; // formula for sum of first n integers

    pthread_exit(0);
}

void *squares(void *param)
{
    int n = *(int*) param; //cast to int pointer, then dereference
    sum2 = 0;

    int i;
    for (i = 1; i <= n; i++)
    {
        sum2 += i * i;
    }

    pthread_exit(0);
}
