#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    pid_t pid1, pid2;
    int sum1 = 0, sum2 = 0;
    int n = atoi(argv[1]);

    pid1 = fork();
    if(pid1 < 0) // error occurred
    {
        fprintf(stderr, "Fork Failed");
        return 1;
    }
    else if (pid1 == 0) // child process
    {
        sum1 = (n * (n + 1)) / 2; // formula for sum1 of first n ints
        printf("The sum of the first %d natural numbers is: %d\n", n, sum1);

        pid2 = fork();
        if (pid2 < 0) // error occurred
        {
            printf("Second fork failed\n");
        }
        else if (pid2 == 0) // grandchild process
        {
            int i;
            for (i = 1; i <= n; i++)
            {
                sum2 += i * i;
            }

            printf("The sum of the squares of the first %d natural numbers is: %d\n", n, sum2);
        }
    }
    else // parent process
    {
        wait(NULL);
    }

    return 0;
}
