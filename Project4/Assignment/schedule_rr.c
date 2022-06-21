#include "schedulers.h"
#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>


int* turnAroundTime;
int* waitTime;
int* responseTime;

int timeStamp = 0;
int taskSize = 0;

double turnAroundTimeAvg = 0;
double waitTimeAvg = 0;
double responseTimeAvg = 0;


void initArray(struct node **head){
    turnAroundTime = (int*) malloc(sizeof(int) * taskSize);
    waitTime = (int*) malloc(sizeof(int) * taskSize);
    responseTime = (int*) malloc(sizeof(int) * taskSize);

    int i;
    struct node *current_node;
    current_node = *head;

    for (i = 0; i < taskSize; i++) {
        turnAroundTime[i] = 0;
        responseTime[i] = 0;
        waitTime[i] = current_node->task->burst;
    }
}

void freeArray(){
    free(turnAroundTime);
    free(waitTime);
    free(responseTime);
}


void responseTimeCalculator(struct node **head){
    if(taskSize > 0){
        struct node *current_node;
        current_node = *head;
        int sumTmp = 0;
        int i;

        for(i=0; i<taskSize; i++){
            responseTime[i] = sumTmp;
            if(current_node->task->burst < QUANTUM)
                sumTmp += current_node->task->burst;
            else
                sumTmp += QUANTUM;
            responseTimeAvg += responseTime[i];

            current_node = current_node->next;
        }
    }
}

void timeCalculator(struct node **head) {
    int i;
    for (i = 0; i < taskSize; i++) {
        waitTime[i] = turnAroundTime[i] - waitTime[i];
        waitTimeAvg += waitTime[i];
        turnAroundTimeAvg += turnAroundTime[i];

        printf("Pid:[%d] waitTime[%d] turnAroundTime[%d] responseTime[%d] \n",i,waitTime[i],turnAroundTime[i],responseTime[i]);
    }

    printf("turnAround Time Average is : %lf (ms)\n",turnAroundTimeAvg /taskSize);
    printf("response Time Average is : %lf (ms)\n",responseTimeAvg /taskSize);
    printf("waitTime Time Average is : %lf (ms)\n",waitTimeAvg /taskSize);
}


void add(char *name, int priority, int burst, struct node **head){
    struct node *current_node;
    current_node = *head;

    pthread_mutex_t lock;
    pthread_mutex_init(&lock,NULL);
    pthread_mutex_lock(&lock);


    Task *t_new = (Task*)malloc(sizeof(Task*));
    t_new->name = name;
    t_new->priority = priority;
    t_new->burst = burst;
    t_new->tid = taskSize;
    taskSize++;

    pthread_mutex_unlock(&lock);

    if(current_node==NULL)
    {
        struct node *new_node = malloc(sizeof(struct node));
        new_node->task = t_new;
        new_node->next = NULL;
        *head = new_node;
        return;
    }

    while((current_node->next)!=NULL)
    {
        current_node=current_node->next;
    }
    if((current_node->next)==NULL)
    {
        struct node *new_node = malloc(sizeof(struct node));
        new_node->task = t_new;
        new_node->next = NULL;
        current_node->next = new_node;
        return;
    }

}


void schedule(struct node **head){
    initArray(head);
    responseTimeCalculator(head);

    int quantum = QUANTUM;
    struct node* current_node, *front_node;
    current_node = *head;
    front_node = NULL;
    // get tail node
    while((current_node->next)!=NULL)
    {
        current_node = current_node->next;
    }
    current_node->next = *head;
    // change into circular list
    current_node = *head;
    while(current_node!=NULL)
    {
        if(current_node->task->burst <= quantum)
        {
            timeStamp += current_node->task->burst;
            run(current_node->task, current_node->task->burst);
            turnAroundTime[current_node->task->tid] = timeStamp;
            current_node->task->burst = 0;

            struct node *tmp;

            if(front_node!=NULL)
            {
                tmp = front_node->next;
                front_node->next = current_node->next;
                current_node = current_node->next;
                if(current_node->next == current_node)
                {
                    if(current_node->task->burst <= quantum){
                        run(current_node->task, current_node->task->burst);
                        timeStamp += current_node->task->burst;
                        current_node->task->burst -= quantum;
                        turnAroundTime[current_node->task->tid] = timeStamp;
                        timeCalculator(head);
                        return;
                    }
                    else{
                        while(current_node->task->burst > quantum){
                            run(current_node->task, quantum);
                            timeStamp+=quantum;
                            current_node->task->burst -= quantum;
                        }
                        run(current_node->task, current_node->task->burst);
                        timeStamp+=current_node->task->burst;
                        current_node->task->burst -= quantum;

                        turnAroundTime[current_node->task->tid] = timeStamp;
                        free(current_node);
                        timeCalculator(head);
                        return;

                    }

                    timeCalculator(head);
                    return;
                }
                free(tmp);
                continue;
            }
        }
        else
        {
            timeStamp += quantum;
            run(current_node->task, quantum);
            current_node->task->burst -= quantum;
        }   
        front_node = current_node;
        current_node = current_node->next;
    }
}