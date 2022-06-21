#include "schedulers.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>

int taskSize = 0;

void add(char *name, int priority, int burst, struct node **head){
    pthread_mutex_t lock;
    pthread_mutex_init(&lock,NULL);
    pthread_mutex_lock(&lock);

    Task *t_new = (Task*)malloc(sizeof(Task*));
    t_new->name = name;
    t_new->priority = priority;
    t_new->tid = taskSize;
    taskSize++;
    t_new->burst = burst;
    insert(head, t_new);
    pthread_mutex_unlock(&lock);
}

/* FCFS*/
void schedule(struct node **head){

    struct node *current_node, *front_node, *back_node;
    current_node = *head;
    front_node = NULL;
    //printf("%s\n", current_node->task->name);

    while(current_node!=NULL && current_node->next!=NULL)
    {
        back_node = current_node->next;

        current_node->next = front_node;
        //printf("%s\n", current_node->task->name);

        front_node = current_node;
        current_node = back_node;     
    }
    // process the end
    if(current_node!=NULL && current_node->next==NULL)
        current_node->next = front_node;
    *head = current_node;

    Task *tmp_task;
    while(current_node!=NULL)
    {
        tmp_task = current_node->task;
        run(tmp_task, tmp_task->burst);
        current_node = current_node->next;
    }
    timeCalculator(head);
}


void timeCalculator(struct node **head){
    if(taskSize > 0){
        double turnAroundTime = 0;
        double waitTime = 0;
        double responseTime = 0;

        int timeArray[taskSize];
        int i;

        struct node *current_node;
        current_node = *head;

        for(i=0; i<taskSize; i++){
            timeArray[i] = current_node->task->burst;
            current_node = current_node->next;

            turnAroundTime += (taskSize - i) * timeArray[i];
            responseTime += (taskSize - i - 1) * timeArray[i];
            waitTime += (taskSize - i - 1) * timeArray[i];
        }
        printf("turnAround Time Average is : %lf (ms)\n",turnAroundTime /taskSize);
        printf("response Time Average is : %lf (ms)\n",responseTime /taskSize);
        printf("waitTime Time Average is : %lf (ms)\n",waitTime /taskSize);

    }
}