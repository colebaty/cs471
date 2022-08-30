#include <pthread.h>
#include <stdio.h>

int sum1; // shared by threads
int sum2; // shared by threads

void *sigma(void *param); // sum of integers from {1, 2, ..., n}
void *squares(void *param); // sum of squares of {1, 2, ..., n}

int main(int argc, char *argv[])
{
    pthread_t tid_sigma, tid_squares; // thread ids
    pthread.attr_t attr; // default thread attributes

    pthread.attr.init(&attr);

    pthread.create(&tid_sigma, &attr, sigma, argv[1]);
    pthread.create(&tid_squares, &attr, squares, argv[1]);

    pthread.join(tid_sigma, NULL);
    pthread.join(tid_squares, NULL);

    printf("The sum of the first %d natural numbers is: %d\n", n, sum1);
    printf("The sum of the squares of the first %d natural numbers is: %d\n", n, sum2);

    return 0;

}

void *sigma(void *param)
{
    int n = atoi(param);
    sum1 = (n * (n + 1)) / 2; // formula for sum of first n integers

    pthread.exit(0);
}

void *squares(void *param)
{
    int n = atoi(param);
    sum2 = 0;

    int i;
    for (i = 1; i <= n; i++)
    {
        sum2 += i * i;
    }

    pthread.exit(0);
}
