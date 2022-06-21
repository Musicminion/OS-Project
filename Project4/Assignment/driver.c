/**
 * Driver.c MainEntrance of the total prog
 * Name Zhang Ziqian
 * ID   520111910121
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"
#include "schedulers.h"

#define SIZE    100

int main(int argc, char *argv[])
{
    FILE *in;
    char *temp;
    char task[SIZE];

    char *name;
    int priority;
    int burst;
    struct node **head=(struct node**)(malloc(sizeof(struct node**)));

    in = fopen(argv[1],"r");
    while (fgets(task,SIZE,in) != NULL) {
        temp = strdup(task);
        name = strsep(&temp,",");
        priority = atoi(strsep(&temp,","));
        burst = atoi(strsep(&temp,","));

        add(name,priority,burst, head);

        free(temp);
    }

    fclose(in);

    // invoke the scheduler
    schedule(head);


    return 0;
}
