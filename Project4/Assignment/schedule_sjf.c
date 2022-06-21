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
        if(burst >= current_node->task->burst ){
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


void schedule(struct node** head)
{
    struct node* current_node;
    current_node = *head;


    while(current_node!=NULL)
    {
        run(current_node->task,current_node->task->burst);
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

            //printf("%d\t", timeArray[i]);
        }

        printf("turnAround Time Average is : %lf (ms)\n",turnAroundTime /taskSize);
        printf("response Time Average is : %lf (ms)\n",responseTime /taskSize);
        printf("waitTime Time Average is : %lf (ms)\n",waitTime /taskSize);

    }
}









