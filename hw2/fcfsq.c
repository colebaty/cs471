#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

static TAILQ_HEAD(tailhead, Process) head = TAILQ_HEAD_INITIALIZER(head);

static struct tailhead *headp;

static struct Process {
    int id;
    int arr;
    int burst;
    int completion;
    TAILQ_ENTRY(Process) entries;
} *p, *pp, *prev;

int main (int argc, char **argv) {
    if (argc < 2) {
        printf("\tUsage: ./a.out <file>\n");
        return 1;
    }

    TAILQ_INIT(&head);

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    fp = fopen(argv[1], "r");

    //discard first line
    getline(&line, &len, fp);
    
    //get rest of data

    char * delim = " \t";
    while ((nread = getline(&line, &len, fp)) != -1) {
        p = malloc(sizeof(struct Process));
        p->id = atoi(strtok(line, delim));
        p->arr = atoi(strtok(NULL, delim));
        p->burst = atoi(strtok(NULL, delim));
        TAILQ_INSERT_TAIL(&head, p, entries);
    }

    TAILQ_FOREACH(p, &head, entries) {
        prev = TAILQ_PREV(p, tailhead, entries);
        if (prev != NULL) { // if not first element
            if (p->arr > prev->completion) 
                p->completion = prev->completion + (p->arr - prev->completion) + p->burst;
            else
                p->completion = prev->completion + p->burst; // add this burst time to previous element's completion time
        }
        else { // first element
            p->completion = p->arr + p->burst;
        }
    }

    printf("Process ID\tArrival\tCPU burst\tCompletion\n");
    TAILQ_FOREACH(p, &head, entries) {
        printf("%d\t\t%d\t%d\t\t%d\t\n", p->id, p->arr, p->burst, p->completion);
    }


    while (!TAILQ_EMPTY(&head)) {
        p = TAILQ_FIRST(&head);
        TAILQ_REMOVE(&head, p, entries);
        free(p);
    }

    return 0;
}
