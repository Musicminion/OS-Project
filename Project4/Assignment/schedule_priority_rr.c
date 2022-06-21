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

    struct node *current_node, *front_node;
    current_node = *head;
    front_node = NULL;
    if(current_node==NULL)
    {
        insert(head, t_new);
        return ;
    }
    while(current_node!=NULL)
    {
        if(priority <= current_node->task->priority ){
            front_node = current_node;
            current_node = current_node->next;
        }
        else{
            struct node* node_newtask = malloc(sizeof(struct node));
            node_newtask->task = t_new;
            node_newtask->next = current_node;
            if(front_node==NULL){
                    *head = node_newtask;
            }
            else{
                front_node->next = node_newtask;
            }
            return;
        }
    }
    if(current_node==NULL)
    {
        struct node* node_newtask = malloc(sizeof(struct node));
        node_newtask->task = t_new;
        node_newtask->next = NULL;
        front_node->next = node_newtask;
        return;
    }
}

void round_robin(struct node **head){
    int quantum = QUANTUM;
    struct node* current_node, *front_node;
    current_node = *head;
    front_node = NULL;
    // get tail node
    while((current_node->next)!=NULL)
    {
        current_node = current_node->next;
    }
    current_node->next = *head;// change into circular list

    // front_node init val is tail_node
    front_node = current_node;
    // current_node now is head_node
    current_node = *head;

    //special situation
    if(current_node->next==current_node)
    {
        int time = current_node->task->burst;
        while(time > quantum)
        {
            run(current_node->task, quantum);
            timeStamp +=quantum;
            current_node->task->burst -= quantum;
            time = current_node->task->burst;
        }
        run(current_node->task, current_node->task->burst);
        timeStamp += current_node->task->burst;
        turnAroundTime[current_node->task->tid] = timeStamp;

        free(current_node);
        return;
    }

    while(current_node!=NULL)
    {
        if(current_node->task->burst <= quantum)
        {
            run(current_node->task, current_node->task->burst);
            timeStamp += current_node->task->burst;
            current_node->task->burst = 0;
            turnAroundTime[current_node->task->tid] = timeStamp;

            struct node *tmp;
            // Now, it's important to remove this node
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
                        current_node->task->burst = 0;
                        turnAroundTime[current_node->task->tid] = timeStamp;
                        free(current_node);
                        return;
                    }
                    else{
                        while(current_node->task->burst > quantum){
                            run(current_node->task, quantum);
                            timeStamp +=quantum;
                            current_node->task->burst -= quantum;
                        }

                        run(current_node->task, current_node->task->burst);
                        timeStamp += current_node->task->burst;
                        current_node->task->burst = 0;

                        turnAroundTime[current_node->task->tid] = timeStamp;
                        free(current_node);
                        return;
                    }
                }
                free(tmp);
                continue;
            }
        }
        else
        {
            run(current_node->task, quantum);
            timeStamp += quantum;
            current_node->task->burst -= quantum;
        }   
        front_node = current_node;
        current_node = current_node->next;
    }
}


void schedule(struct node **head){
    initArray(head);
    responseTimeCalculator(head);

    //int quantum = QUANTUM;
    
    struct node* current_node;
    current_node = *head;

    struct node** circular_list_head=malloc(sizeof(struct node*));
    // struct node** circular_list_tail=malloc(sizeof(struct node*));
    struct node* current_circular_node;
    current_circular_node = NULL;
    
    while(current_node!=NULL)
    {
        if(current_circular_node==NULL)
        {
            current_circular_node = current_node;
            *circular_list_head = current_node;
            //printf("!!!%d\n", current_circular_node->task->priority);
            current_node = current_node->next;
            continue;
        }

        if(current_node->task->priority==current_circular_node->task->priority)
        {
            current_circular_node = current_circular_node->next;
            current_node = current_node->next;
            continue;
        }
        else
        {
            // a level of priority has been found out
            *head = current_node;
            current_circular_node->next = NULL;
            round_robin(circular_list_head);
            // clear the list
            *circular_list_head = NULL;
            current_circular_node = *circular_list_head;

        }
    }
    if(current_circular_node!=NULL)
    {
        round_robin(circular_list_head);
    }
    timeCalculator(head);
}

