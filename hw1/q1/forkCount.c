#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

//number of calls to fork()
int count = 0;

int main()
{
    /*fork a child process */
    fork();
    count++;
    fork();
    count++;
    fork();
    count++;
    int i;
    for(i = 0; i < 7; i++)
    {
        fork(); /* fork another child process */
        count++;
    }
    fork();
    count++;

    printf("count: %d\n", count);

    return 0;

}
