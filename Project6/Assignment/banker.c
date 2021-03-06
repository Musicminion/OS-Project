#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define NUMBER_OF_CUSTOMERS 5
#define NUMBER_OF_RESOURCES 4
#define SIZE 100
/* the available amount of each resource */
int available[NUMBER_OF_RESOURCES];
/*the maximum demand of each customer */
int maximum[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the amount currently allocated to each customer */
int allocation[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];
/* the remaining need of each customer */
int need[NUMBER_OF_CUSTOMERS][NUMBER_OF_RESOURCES];

int safety_algorithm(int *a)
{
    // 首先，这一步我们要执行的是一个预测操作
    // 假设这一个操作执行成功了，我们要检查是否会出现死锁
    // 如果这个操作执行成功了，need[a[0]][j](0<=j<资源数)将会变成need[a[0]][j]-a[j+1]
    // 同样的，假如这个步骤执行成功了， available[i]会变成 available[i]-a[i+1];
    // 你可能会问为什么是i+1，这是由于a[]系列的数组第第一个a[0]元素是表示customer的编号，所以有一个错位

    // 所以按照教材，我们要定义一个数组，work数组最开始初始化为available数组，也就是假定操作成功后的available
    // 假如这个步骤执行成功了， available[i]会变成 available[i]-a[i+1];
    int work[NUMBER_OF_RESOURCES];
    for(int i=0; i<NUMBER_OF_RESOURCES; i++)
        work[i] = available[i]-a[i+1];

    // 然后我们要定义一个finish的数组，表示是否可以完成的任务，初始化为不能完成
    int finish[NUMBER_OF_CUSTOMERS];
    for(int i=0; i<NUMBER_OF_CUSTOMERS; ++i)
        finish[i]=0;

    int all_satisfy;
    int i;
    for(i=0; i<NUMBER_OF_CUSTOMERS; ++i)
    {
        all_satisfy=0;//refresh

        if(finish[i]==0){
            for(int j=0; j<NUMBER_OF_RESOURCES; ++j)
            {
                // 由于need数组针对不同的行，在未来可能的情况不一样，所以分别考虑
                // 假如我们当前检查的行数，是我们要请求资源分配的行数，need数组未来会变成need[i][j]-a[j+1]
                // 反之不会变的
                if(i==a[0])
                {
                    if(need[i][j]-a[j+1]<=work[j])
                        all_satisfy++;
                }
                else if(need[i][j]<=work[j])
                    all_satisfy++;
            }
            if(all_satisfy==NUMBER_OF_RESOURCES)
            {   //this customer can be finished
                for(int j=0; j<NUMBER_OF_RESOURCES; ++j)
                    work[j] += allocation[i][j];
                finish[i]=1;
                i=-1;// back to the start
            }
        }
    }

    int satisfy_number = 0;
    for(i=0; i<NUMBER_OF_CUSTOMERS; ++i){
        if(finish[i]==1)
            satisfy_number++;
        else{
            printf("[Error] DeadLock Detected! If you do such a request, in next status,Task %d won't be executed \n", i);
        }
    }
    if(satisfy_number == NUMBER_OF_CUSTOMERS)
        return 0;   //safe
    else
        return -1;  //unsafe
}

int requestBasicCheck(int *a){
    int client_id = a[0];
    if(client_id>=NUMBER_OF_CUSTOMERS)
    {
        printf("[Error] Request Error: The ID YOU INPUT Exceed THE NUMBER_OF_CUSTOMERS\n");
        return -1;
    }
    // basic check
    for(int i=1;i<=NUMBER_OF_RESOURCES;++i)
    {
        if(a[i]>need[client_id][i-1])
        {
            printf("[Error] Request Error: Your Request Exceed Need\n");
            return -1;
        }
        if(a[i]>available[i-1])
        {
            printf("[Error] Request Error: Request Exceed Available\n");
            return -1;
        }
    }
    return 0;
}



/* request resource*/
int request(int *a)
{
    int client_id = a[0];
    if (requestBasicCheck(a) == -1)
        return -1;
    /* safety algorithm*/
    int is_safe = safety_algorithm(a);
    if(is_safe == -1)
    {
        printf("[Error] Your Request Leads to An Unsafe State! System Denyed!\n");
        return 1;
    }
    else
    {
        printf("Request is satisfied.\n");
    }
    // after the request, the state should be safe.
    for(int i=1;i<=NUMBER_OF_RESOURCES;++i)
    {
        available[i-1] -= a[i];
        need[client_id][i-1] -= a[i];
        allocation[client_id][i-1] += a[i];
    }
    return 0;
}

int releaseBasicCheck(int *a){
    int consumer_id = a[0];
    if(consumer_id >= NUMBER_OF_CUSTOMERS)
    {
        printf("[Error] Request Error: Your Request Release Exceed Need\n");
        return -1;
    }
    // basic check
    for(int i=1;i<=NUMBER_OF_RESOURCES;++i)
    {
        if(a[i]>allocation[consumer_id][i-1])
        {
            printf("[Error] Release Error: Release Exceed Allocation\n");
            return -1;
        }
    }
    return 0;
}


/* release resource*/
int release(int *a)
{
    int consumer_id = a[0];
    if(releaseBasicCheck(a) == -1)
        return -1;
    // no exceed
    for(int i=1;i<=NUMBER_OF_RESOURCES; ++i)
    {
        allocation[consumer_id][i-1] -= a[i];
    }
    return 0;
}


/* show all the states*/
void state_display()
{
    int i;
    int j;
    printf("___________________[Table Available Resource]___________________\n");
    for(i=0;i<NUMBER_OF_RESOURCES;++i)
        printf("resource[%d]=%d ", i, available[i]);
    printf("\n________________________________________________________________");
    printf("\n\n");

    printf("________________________[Table Maximum]________________________\n");
    for(i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        printf("custom[%d]   ", i);
        for(j=0;j<NUMBER_OF_RESOURCES;++j)
            printf("resource[%d]=%d ", j, maximum[i][j]);
        printf("\n________________________________________________________________");
        printf("\n");
    }
    printf("\n");

    printf("______________________[Table Allocation]_______________________\n");
    for(i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        printf("custom[%d]   ", i);
        for(j=0;j<NUMBER_OF_RESOURCES;++j)
            printf("resource[%d]=%d ", j, allocation[i][j]);
        printf("\n________________________________________________________________");
        printf("\n");
    }
    printf("\n");

    printf("_________________________[Table Need]__________________________\n");
    for(i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        printf("custom[%d]   ", i);
        for(j=0;j<NUMBER_OF_RESOURCES;++j)
            printf("resource[%d]=%d ", j, need[i][j]);
        printf("\n________________________________________________________________");
        printf("\n");
    }
    printf("\n");
    printf("_________________________[Table End]__________________________\n");
}


int main()
{
    // initialize array MAXIMUM with txt file
    FILE *in;
    in = fopen("max_need.txt", "r");
    char customer[SIZE];
    char *temp;
    for(int i=0;i<NUMBER_OF_CUSTOMERS;++i)
    {
        fgets(customer,SIZE,in);
        temp = strdup(customer);
        for(int j=0;j<NUMBER_OF_RESOURCES;j++)
            maximum[i][j]=need[i][j]=atoi(strsep(&temp, ",")), allocation[i][j]=0;
    }
    fclose(in);

    // initialize array **available**  with txt file
    in = fopen("available_resource.txt", "r");
    char ava[SIZE];
    fgets(ava, SIZE, in);
    temp = strdup(ava);
    for(int i=0;i<NUMBER_OF_RESOURCES;i++)
        available[i]=atoi(strsep(&temp, ","));
    fclose(in);


    int input_instruction[NUMBER_OF_RESOURCES+1];
    int request_value;
    int release_value;
    int tmp;

    char input_line[SIZE];
    int ifrun = 1;
    while(ifrun)
    {
        printf("Banker >>> ");
        fgets(input_line, SIZE, stdin);

        if(strcmp(input_line, "end\n")==0)
        {
            ifrun = 0;
            printf("[Info] Banker's Algorithm Finished, exit!\n");
            continue;
        }

        if(input_line[0] == 'R' && input_line[1] == 'Q'){
            //decode instruction
            temp = strdup(input_line);
            strsep(&temp," ");
            for(int i=0; i<=NUMBER_OF_RESOURCES;++i)
                input_instruction[i] = atoi(strsep(&temp, " "));
            request_value = request(input_instruction);
            if(request_value!=0)
                printf("Request Failed. Please Follow Rules.\n");
            continue;
        }

        else if(input_line[0] == 'R' && input_line[1] == 'L'){
            temp = strdup(input_line);
            strsep(&temp," ");
            for(int i=0; i<=NUMBER_OF_RESOURCES;++i)
                input_instruction[i] = atoi(strsep(&temp, " "));
            release_value = release(input_instruction);
            if(release_value!=0)
                printf("Release Failed. Please Follow Rules.\n");
            continue;
        }

        else if(input_line[0]=='*')
            state_display();
        else{
            printf("[Error] Invalid Token, Please Input Again!\n");
        }

    }
    return 0;
}