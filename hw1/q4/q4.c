#include <sys/types.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/wait.h>

int main() 
{ 
    pid_t pid, pid1, pid2, pid0,pid3,pid4,pid5; 

    /* fork a child process */
    pid = fork();
    if (pid < 0) { /* error occurred */ 
        fprintf(stderr, "First Fork Failed");
        return 1; 
    }
    else if (pid == 0) { /* child process */ 
        pid1 = getppid();
        pid2= getpid();

        printf("Child PID = %d\n",pid2);
        printf("Parent PID = %d\n",pid1);
        pid3= fork();
        if (pid3 < 0) {
            fprintf(stderr, "Second fork failed");
            return 1;
        } 
        else if (pid3==0) /*Grandchild process */ { 
            pid4=getpid();
            pid5=getppid();
            printf("Grandchild PID = %d\n",pid4);
            printf("Child PID = %d\n",pid5);
        }
    } 
    else { /* parent process */ 
        pid1 = getpid();
        pid0 = getppid();
        printf("Parent: = %d\n",pid1);
        printf("Root: = %d\n",pid0);
        wait(NULL);
    } 

    return 0;
}

