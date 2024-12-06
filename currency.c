#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

typedef struct cThread {
    char        TID;
    void        (*Handler)();

    void        **args;
    long        c;

    pthread_t   id;
    int         Running;
    void        *Base;
} cThread;

typedef struct ConcurrencyThread {
    int        MAX_THREADS;

    cThread     **Threads;
    long        ThreadCount;
    int         RunningThreads;

    pthread_t   PoolThread;
} ConcurrencyThread;

void ToggleThread(cThread *c) 
{ 
    c->Running = (c->Running ? 0 : 1); 
    (c->Running ? ((ConcurrencyThread *)c->Base)->RunningThreads++ : ((ConcurrencyThread *)c->Base)->RunningThreads--);
}

int GetRandomID() {
    int num = rand();
    if(num > 0)
        return num;

    while((num = rand()) < 1);
    return num;
}

int CountRunningThreads(ConcurrencyThread *c) {
    int Count = 0;
    for(int i = 0; i < c->ThreadCount; i++)
        if(c->Threads[i]->Running)
            Count++;

    return Count;
}

cThread StartThread(void *fn, void **args) {
    cThread c = (cThread){
        .TID = (rand() % (10000 - 1 + 1)) + 1,
        .Handler = fn,
        .args = args,
    };

    while(args[c.c] != NULL) c.c++;
    return c;
}

void StartPool(ConcurrencyThread *c) {
    while(1) {
        for(int i = 0; i < c->ThreadCount; i++) {
            if(c->RunningThreads >= c->MAX_THREADS) {
                sleep(1);
                continue;
            }

            ToggleThread(c->Threads[i]);
            pthread_create(&c->Threads[i]->id, NULL, (void *)c->Threads[i]->Handler, c->Threads[i]);
        }

        if(CountRunningThreads(c) == 0)
            break;

        sleep(1);
    }
}

int AppendThread(ConcurrencyThread *c, cThread thread) {
    if(!c)
        return 0;

    c->Threads[c->ThreadCount] = (cThread *)malloc(sizeof(cThread));
    if(!c->Threads[c->ThreadCount])
        return 0;

    *c->Threads[c->ThreadCount] = thread;
    ((cThread *)c->Threads[c->ThreadCount])->Base = c;

    c->ThreadCount++;
    c->Threads = (cThread **)realloc(c->Threads, sizeof(cThread *) * (c->ThreadCount + 1));

    return 1;
}

ConcurrencyThread *CreatePool(int MAX) {
    ConcurrencyThread *c = (ConcurrencyThread *)malloc(sizeof(ConcurrencyThread));
    *c = (ConcurrencyThread){
        .MAX_THREADS    = MAX,
        .Threads        = (cThread **)malloc(sizeof(cThread *) * 1),
        .ThreadCount    = 0,
        .RunningThreads = 0
    };

    return c;
}

void Counter(void *arg) {
    cThread *c = (cThread *)arg;
    printf("Starting Thread: %d = > Running State: %d\n", c->TID, c->Running);

    for(int i = 0; i < (int)c->args[1]; i++)
        sleep(1);

    printf("[%d] Completed Thread: %d:%d...!\n", (int)c->args[0], c->TID, c->Running);
    ToggleThread(c);
}

int main(void) {
    srand(time(NULL));

    // Initalize a Thread Pool with 2 Concurrency (2 threads running @ once)
    ConcurrencyThread *c = CreatePool(2);

    // Add 10 Threads
    for(int i = 0; i < 10; i++)
        AppendThread(c, StartThread(Counter, (void *[]){(void *)i, (void *)5, NULL}));

    // Run the thread pool in the background
    pthread_create(&c->PoolThread, NULL, (void *)StartPool, (void *)c);

    // Debugging - Checking Threads
    iny i = 0;
    while(1) {
        int running = CountRunningThreads(c);
        if(c->RunningThreads > c->MAX_THREADS)
                printf("[ - ] Max Threads Reached, Waiting...\n");

        if(running == 0)
            break;

        printf("[ %d ]: Threads currently running %d/%d\n", i, running, c->RunningThreads ? c->RunningThreads++ : 0 );
        sleep(1);
        i++;
    }

    printf("[ + ] Completed, Press any key to exit\n");

    char BUFF[10];
    fgets(BUFF, 5, stdin);

    return 0;
}