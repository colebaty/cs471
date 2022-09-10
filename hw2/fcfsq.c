#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>

static TAILQ_HEAD(tailarrive, Process) arrive = TAILQ_HEAD_INITIALIZER(arrive);
static TAILQ_HEAD(tailwait, Process) wait = TAILQ_HEAD_INITIALIZER(wait);
static TAILQ_HEAD(tailcomplete, Process) complete = TAILQ_HEAD_INITIALIZER(complete);

static struct tailarrive *arrivep;
static struct tailwait *waitp;
static struct tailcomplete *completep;

static struct Process {
    int id;
    int arr;
    int burst;
    int completion;
    TAILQ_ENTRY(Process) entries;
} *p, *prev, *running;

void readFile(char * filename) {
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    fp = fopen(filename, "r");

    //discard first line
    getline(&line, &len, fp);
    
    //get rest of data
    //put in arrive queue
    char * delim = " \t";
    while ((nread = getline(&line, &len, fp)) != -1) {
        p = malloc(sizeof(struct Process));
        p->id = atoi(strtok(line, delim));
        p->arr = atoi(strtok(NULL, delim));
        p->burst = atoi(strtok(NULL, delim));
        TAILQ_INSERT_TAIL(&arrive, p, entries);
    }

    //insert "last" record so loop works in run()
    p = malloc(sizeof(struct Process));
    p->id = p->arr = p-> burst = p->completion = -1;
    TAILQ_INSERT_TAIL(&arrive, p, entries);

    fclose(fp);
}

/*
 * first implementation - arithmetic
 */
void computeCompleteTimes() {
    TAILQ_FOREACH(p, &arrive, entries) {
        prev = TAILQ_PREV(p, tailarrive, entries);
        if (prev != NULL) { // if not first element
            if (p->arr > prev->completion) 
                p->completion = prev->completion 
                    + (p->arr - prev->completion) + p->burst;
            else
                // add this burst time to previous element's completion time
                p->completion = prev->completion + p->burst; 
        }
        else { // first element
            p->completion = p->arr + p->burst;
        }
    }
}

/*
 * second implementation - queue based. introduced a clock to help 
 * scheduler. processes move from "arrival" queue to "running" state,
 * then on to a "completion" queue for printing of results.
 */
void run() {
    int time = 0;

    p = TAILQ_FIRST(&arrive);
    while (!TAILQ_EMPTY(&arrive)) {
        if (running == NULL) {
            if (time >= p->arr) {
                running = p;
                TAILQ_REMOVE(&arrive, p, entries);
                running->completion = time + running->burst;
                p = TAILQ_FIRST(&arrive);
            }
        }
        else {
            if (time >= running->completion) {
                TAILQ_INSERT_TAIL(&complete, running, entries);
                if (time >= p->arr) {
                    running = p;
                    TAILQ_REMOVE(&arrive, p, entries);
                    running->completion = time + running->burst;
                    p = TAILQ_FIRST(&arrive);
                }
                else
                    running = NULL;
            }
        }
        time++;
    }
}

void print() {
    printf("Process ID\tArrival\tCPU burst\tCompletion\n");
    TAILQ_FOREACH(p, &complete, entries) {
        printf("%d\t\t%d\t%d\t\t%d\t\n", p->id, p->arr, p->burst, p->completion);
    }
}

int main (int argc, char **argv) {
    if (argc < 2) {
        printf("\tUsage: ./a.out <file>\n");
        return 1;
    }

    TAILQ_INIT(&arrive);
    TAILQ_INIT(&wait);
    TAILQ_INIT(&complete);

    readFile(argv[1]);
    run();
    print();

    //housekeeping
    while (!TAILQ_EMPTY(&complete)) {
        p = TAILQ_FIRST(&complete);
        TAILQ_REMOVE(&complete, p, entries);
        free(p);
    }

    return 0;
}
