#include <stdio.h>
#include <sys/queue.h>
#include <stdlib.h>

/*
 * testing use of sys/queue.h
 */

static TAILQ_HEAD(tailhead, Process) head = TAILQ_HEAD_INITIALIZER(head);

static struct tailhead *headp;

static struct Process {
    int id;
    int arr;
    int burst;
    TAILQ_ENTRY(Process) entries;
} *p, *pp;


int main(int argc, char **argv) {
    TAILQ_INIT(&head);

    int i;
    for (int i = 0; i < 3; i++) {
        p = malloc(sizeof(struct Process));
        p->id = i;
        p->arr = i;
        p->burst = i;
        TAILQ_INSERT_TAIL(&head, p, entries);
    }

    printf("ID\t\tARR\t\tBURST\n");

    TAILQ_FOREACH(pp, &head, entries)
        printf("%d\t\t%d\t\t%d\n", pp->id, pp->arr, pp->burst);

    return 0;
}
