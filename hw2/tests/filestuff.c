#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct Process {
    int id;
    int arr;
    int burst;
} *p;


int main (int argc, char **argv) {

    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t nread;

    fp = fopen("data", "r");

    //discard first line
    getline(&line, &len, fp);
    
    //get rest of data
    while ((nread = getline(&line, &len, fp)) != -1) {
        fwrite(line, nread, 1, stdout);
        p = malloc(sizeof(struct Process));
        p->id = atoi(strtok(line, " \t"));
        p->arr = atoi(strtok(NULL, " \t"));
        p->burst = atoi(strtok(NULL, " \t"));
        printf("Process %d: %d\t%d\n", p->id, p->arr, p->burst);
    }

    free(p);

    return 0;
}
