#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 1000
long long int spaceAll;
// 定义一个内存块的结构体，最终以链表的形式呈现
typedef struct Memory_Block
{
    long long int compacity;
    long long int start;
    long long int end;
    long long int id;
    // int ifUsed;
    struct Memory_Block * next;
} Memory_Block;

// 在下面的部分写函数的声明
// 销毁和初始化
void memory_Space_Init(Memory_Block * firstblock);
void memory_Space_Clear(Memory_Block ** memory_head);

// 打印机组
void state_Printer(Memory_Block ** firstblock);
void SingleBlock_state_Printer(Memory_Block *oneBlock);

// RQ RL业务组
void RQ_Master(char * tokenInput, Memory_Block ** memory_head);
void RL_Master(char * tokenInput, Memory_Block ** memory_head);

// 核心算法函数组
int First_fit(Memory_Block** head, Memory_Block* newBlock);
int Best_fit(Memory_Block** head, Memory_Block* newBlock);
int Worst_fit(Memory_Block** head, Memory_Block* newBlock);
int release(Memory_Block** head, int id);


// 在上面的部分写函数的声明

int main(int argc, char* argv[]){
    if(argc < 1){
        printf("[Error] Please Input Argc\n");
        return 1;
    }
    // 完成初始化的工作
    spaceAll = atoi(argv[1]);
    Memory_Block * firstblock = malloc(sizeof(Memory_Block));
    memory_Space_Init(firstblock);
    Memory_Block **memory_head = malloc(sizeof(Memory_Block*));
    *memory_head = firstblock;

    int ifrun = 1;
    char tokenInput[SIZE];

    // 开始运行
    while(ifrun){
        printf(" Allocator >> ");
        fgets(tokenInput, SIZE, stdin);

        if(tokenInput[0] == 'S' && tokenInput[1] =='T' && tokenInput[2] == 'A' && tokenInput[3] =='T'){
            state_Printer(memory_head);
        }

        else if(tokenInput[0] == 'R' && tokenInput[1] =='Q'){
            RQ_Master(tokenInput, memory_head);
        }

        else if(tokenInput[0] == 'R' && tokenInput[1] =='L'){
            RL_Master(tokenInput, memory_head);
        }

        else if(tokenInput[0]=='X')
        {
            ifrun = 0;
            continue;
        }
    }

    // 回收内存
    memory_Space_Clear(memory_head);
    return 0;
}

void memory_Space_Init(Memory_Block * firstblock){
    firstblock->start = 0;
    firstblock->end = spaceAll - 1;
    firstblock->compacity = spaceAll;
    firstblock->next = NULL;
    // firstblock->ifUsed = 0;
    firstblock->id = -1;
}


void RQ_Master(char * tokenInput, Memory_Block ** memory_head){
    int allocate_ifsuccess = 0;

    char * tmp = strdup(tokenInput);
    strsep(&tmp, " ");
    char *process_name = strsep(&tmp, " ");
    process_name++;

    int process_id = atoi(process_name);
    long long int process_size = atoi(strsep(&tmp, " "));
    char *mode_insert;
    mode_insert = strsep(&tmp, " ");

    Memory_Block *new_block = malloc(sizeof(Memory_Block));
    new_block->compacity = process_size;
    new_block->id = process_id;

    //if(new_block->id == NULL || new_block->compacity == NULL)
    if(mode_insert == NULL){
        mode_insert = "F";
        printf("[Info] You didn't input any method, so common use first insert!\n");
    }


    switch (mode_insert[0])
    {
        case 'F':
            allocate_ifsuccess = First_fit(memory_head,new_block);
            break;
        case 'B':
            allocate_ifsuccess = Best_fit(memory_head,new_block);
            break;
        case 'W':
            allocate_ifsuccess = Worst_fit(memory_head,new_block);
            break;

        default:
            printf("[Error] No such mode.Please Input again.\n");
            break;
    }

    if(allocate_ifsuccess == 1)
        printf("[Info] Allocate P%lld Success.\n", new_block->id);
    else{
        printf("[Error] Allocate P%lld Fail.\n", new_block->id);
        free(new_block);
    }
}


void RL_Master(char * tokenInput, Memory_Block ** memory_head){
    char* tmp = strdup(tokenInput);
    strsep(&tmp, " ");
    char *process_name = strsep(&tmp, " ");
    process_name++;
    int process_id = atoi(process_name);

    int release_ifsuccess = release(memory_head, process_id);

    if(release_ifsuccess == 1)
        printf("[Info] Release P%d success.\n", process_id);
    else
        printf("[Error] Release P%d Fail.Maybe it doesn't exit at all! \n", process_id);
}

void SingleBlock_state_Printer(Memory_Block *oneBlock){
    if(oneBlock != NULL){
        if(oneBlock->id == -1){
            if(oneBlock->compacity ==1)
                printf("[Result] Addresses [%lld] Unused\n", oneBlock->start);
            else
                printf("[Result] Addresses [%lld:%lld] Unused\n", oneBlock->start, oneBlock->end);
        }
        else{
            if(oneBlock->compacity == 1)
                printf("[Result] Addresses [%lld] used BY: Process %lld \n", oneBlock->start, oneBlock->id);
            else
                printf("[Result] Addresses [%lld:%lld] used BY: Process %lld \n", oneBlock->start, oneBlock->end, oneBlock->id);
        }
    }
}

void state_Printer(Memory_Block ** head){
    Memory_Block * tmp_block = (*head);
    SingleBlock_state_Printer(tmp_block);

    while (tmp_block->next != NULL)
    {
        tmp_block = tmp_block->next;
        SingleBlock_state_Printer(tmp_block);
    }

}


int First_fit(Memory_Block** head, Memory_Block* newBlock){
    Memory_Block * tmp = (*head);

    if(tmp->id == -1){
        if(tmp->compacity >= newBlock->compacity){
            newBlock->start = 0;
            newBlock->end = newBlock->compacity -1;
            tmp->compacity -= newBlock->compacity;
            tmp->start = newBlock->end + 1;

            if(tmp->compacity == 0){
                newBlock->next = NULL;
                free(tmp);
            }
            else
                newBlock->next = tmp;
            *head =newBlock;
            return 1;
        }
        else
            return 0;
    }

    Memory_Block *pre_tmp;

    while(tmp->next != NULL){
        pre_tmp = tmp;
        tmp = tmp->next;

        // 空间不够 全部跳过 空间已被使用 直接跳过
        if(tmp->id != -1 || tmp->compacity < newBlock->compacity)
            continue;

        tmp->compacity -= newBlock->compacity;
        newBlock->start = tmp->start;
        newBlock->end = newBlock->start + newBlock->compacity -1;
        tmp->start = newBlock->end + 1;
        pre_tmp->next = newBlock;

        if(tmp->compacity == 0){
            newBlock->next = tmp->next;
            free(tmp);
        }
        else{
            newBlock->next = tmp;
        }
        return 1;
    }
    return 0;
}


int Best_fit(Memory_Block** head, Memory_Block* newBlock){
    Memory_Block * tmp = (*head);

    if(tmp->id == -1){
        if(tmp->compacity >= newBlock->compacity){
            newBlock->start = 0;
            newBlock->end = newBlock->compacity -1;
            tmp->compacity -= newBlock->compacity;
            tmp->start = newBlock->end + 1;

            if(tmp->compacity == 0){
                newBlock->next = NULL;
                free(tmp);
            }
            else
                newBlock->next = tmp;
            *head =newBlock;
            return 1;
        }
        else
            return 0;
    }

    Memory_Block * pre_tmp;
    int min_space = 0;

    while(tmp->next != NULL){
        pre_tmp = tmp;
        tmp = tmp->next;

        // 空间不够 全部跳过 空间已被使用 直接跳过
        if(tmp->id != -1 || tmp->compacity < newBlock->compacity)
            continue;

        if(min_space == 0)
            min_space = tmp->compacity;

        if(tmp->compacity < min_space)
            min_space = tmp->compacity;
    }

    if(min_space == 0)
        return 0;

    tmp = *head;

    while(tmp->next != NULL){
        pre_tmp = tmp;
        tmp = tmp->next;

        if(tmp->id != -1 || tmp->compacity < newBlock->compacity)
            continue;

        if(tmp->compacity == min_space){
            tmp->compacity -= newBlock->compacity;
            newBlock->start = tmp->start;
            newBlock->end = newBlock->start + newBlock->compacity -1;
            tmp->start = newBlock->end + 1;
            pre_tmp->next = newBlock;

            if(tmp->compacity == 0){
                newBlock->next = tmp->next;
                free(tmp);
            }
            else{
                newBlock->next = tmp;
            }
            return 1;
        }
    }
    return 0;
}



int Worst_fit(Memory_Block** head, Memory_Block* newBlock){
    Memory_Block * tmp = (*head);
    if(tmp->id == -1){
        if(tmp->compacity >= newBlock->compacity){
            newBlock->start = 0;
            newBlock->end = newBlock->compacity -1;
            tmp->compacity -= newBlock->compacity;
            tmp->start = newBlock->end + 1;

            if(tmp->compacity == 0){
                newBlock->next = NULL;
                free(tmp);
            }
            else
                newBlock->next = tmp;
            *head =newBlock;
            return 1;
        }
        else
            return 0;
    }

    Memory_Block * pre_tmp;
    long long int max_space = 0;

    while(tmp->next != NULL){
        pre_tmp = tmp;
        tmp = tmp->next;

        // 空间不够 全部跳过 空间已被使用 直接跳过
        if(tmp->id != -1 || tmp->compacity < newBlock->compacity)
            continue;

        if(max_space == 0)
            max_space = tmp->compacity;

        if(tmp->compacity > max_space)
            max_space = tmp->compacity;
    }

    if(max_space == 0)
        return 0;

    tmp =* head;

    while(tmp->next != NULL){
        pre_tmp = tmp;
        tmp = tmp->next;

        if(tmp->id != -1 || tmp->compacity < newBlock->compacity)
            continue;

        if(tmp->compacity == max_space){
            tmp->compacity -= newBlock->compacity;
            newBlock->start = tmp->start;
            newBlock->end = newBlock->start + newBlock->compacity -1;
            tmp->start = newBlock->end + 1;
            pre_tmp->next = newBlock;

            if(tmp->compacity == 0){
                newBlock->next = tmp->next;
                free(tmp);
            }
            else{
                newBlock->next = tmp;
            }
            return 1;
        }
    }
    return 0;
}


int release(Memory_Block** head, int id){
    Memory_Block * tmp = *head;
    Memory_Block * pre_tmp;

    int if_found = 0;

    while(tmp!=NULL){
        if(tmp->id == id){
            if_found = 1;
            tmp->id = -1;
        }
        tmp = tmp->next;
    }

    if(if_found == 0)
        return 0;

    tmp = *head;
    while(tmp != NULL){
        if(tmp -> id == -1){
            if(pre_tmp != NULL && pre_tmp->id == -1){
                pre_tmp->compacity += tmp->compacity;
                pre_tmp->end = tmp->end;
                pre_tmp->next = tmp->next;
                free(tmp);
                tmp = pre_tmp->next;
            }
            else if(pre_tmp ==NULL && tmp->next == NULL){
                tmp->id = -1;
                return 1;
            }
            else{
                pre_tmp = tmp;
                tmp = tmp->next;
            }
        }
        else{
            pre_tmp = tmp;
            tmp = tmp->next;
        }
    }
    return 1;
}

void memory_Space_Clear(Memory_Block ** head){
    Memory_Block * tmp;
    tmp = *head;
    Memory_Block *tmp_next;
    while(tmp != NULL){
        tmp_next = tmp->next;
        free(tmp);
        tmp = tmp_next;
    }
}