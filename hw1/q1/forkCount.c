#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <math.h>

//number of calls to fork()
int count = 0;

int main()
{
    int pid = getpid();
    /*fork a child process */

    for (int i = 0; i < 11; i++)
    {
        fork();
        count++;
    }

    if (pid == getpid())
    {
        wait(NULL);
        printf("num calls to fork(): %d\n", count);
        printf("num processes created: %d\n", (int) pow(2, count));
    }

    return 0;

}
