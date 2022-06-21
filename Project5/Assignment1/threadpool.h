// function prototypes
void execute(void (*somefunction)(void *p), void *p);
int pool_submit(void (*somefunction)(void *p), void *p);
void *worker(int param);
void pool_init(void);
void pool_shutdown(void);

struct data
{
    int a;
    int b;
    int clientID;
    int taskID;
    int executorID;
};