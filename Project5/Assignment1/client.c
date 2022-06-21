/**
 * Example client program that uses thread pool.
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include "threadpool.h"

#define QUEUE_SIZE 100

int global_taskNumber = 0;
pthread_mutex_t ID_lock;

//struct data
//{
//    int a;
//    int b;
//    int clientID;
//    int taskID;
//};


struct data* pointerGroup[6];

void add(void *param)
{
    struct data *temp;
    temp = (struct data*)param;
    printf("[Info] FINISH: From Client ID:%d Task ID: %d Task executor:%d Task Content: %d + %d = %d \n",
           temp->clientID,temp->taskID, temp->executorID, temp->a, temp->b, temp->a + temp->b);
}


static void * submitTaskClient(int idClient){
    //static struct data work1[2 * QUEUE_SIZE];
    struct data* work1 = (struct data*)malloc(2*QUEUE_SIZE * sizeof(struct data));

    pointerGroup[idClient] = work1;
    for (int i = 0; i < 2 * QUEUE_SIZE; i++)
    {
        work1[i].a = rand()%10000;
        work1[i].b = rand()%10000;
        work1[i].clientID = idClient;
        pthread_mutex_lock(&ID_lock);
        global_taskNumber++;
        work1[i].taskID = global_taskNumber;
        printf("[Info] SUBMIT: ClientID: %d submit Global-TaskID %d\n",idClient, work1[i].taskID);
        pthread_mutex_unlock(&ID_lock);

        pool_submit(&add, &work1[i]);
    }
}

int main(void)
{
    pthread_t tid1, tid2, tid3, tid4, tid5, tid6;
    pthread_attr_t attr1, attr2, attr3, attr4, attr5, attr6;
    pthread_attr_init(&attr1);
    pthread_attr_init(&attr2);
    pthread_attr_init(&attr3);
    pthread_attr_init(&attr4);
    pthread_attr_init(&attr5);
    pthread_attr_init(&attr6);


    pool_init();


    pthread_mutex_init(&ID_lock,NULL);

    // create some work to do
//    pthread_create(&tid1, &attr1, submitTaskClient1,NULL);
//    pthread_create(&tid2, &attr2, submitTaskClient2,NULL);
//    pthread_create(&tid3, &attr3, submitTaskClient3,NULL);

    pthread_create(&tid1, &attr1, submitTaskClient,1);
    pthread_create(&tid2, &attr2, submitTaskClient,2);
    pthread_create(&tid3, &attr3, submitTaskClient,3);
    pthread_create(&tid4, &attr4, submitTaskClient,4);
    pthread_create(&tid5, &attr5, submitTaskClient,5);
    pthread_create(&tid6, &attr6, submitTaskClient,6);


    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    pthread_join(tid3, NULL);
    pthread_join(tid4, NULL);
    pthread_join(tid5, NULL);
    pthread_join(tid6, NULL);

    pthread_mutex_destroy(&ID_lock);

    sleep(3);
    pool_shutdown();

    for(int i=0 ;i<6; i++){
        free(pointerGroup[i]);
    }

    return 0;
}