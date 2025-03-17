#pragma once

#include <pthread.h>

/*

    - Debugging Function, Keep In Here!

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
    int i = 0;
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

*/

typedef struct cThread {
    char        TID;
    void        (*Handler)();

    void        **args;
    long        c;

    pthread_t   id;
    int         Running;
    int         Completed;
    void        *Base;

    void        (*Destruct) (struct cThread *thread);
} cThread;

typedef struct ConcurrencyThread {
    int        MAX_THREADS;

    cThread     **Threads;
    long        ThreadCount;
    int         RunningThreads;

    pthread_t   PoolThread;
} ConcurrencyThread;

ConcurrencyThread *CreatePool(int MAX);
int AppendThread(ConcurrencyThread *c, cThread *thread);
int RemoveThread(ConcurrencyThread *c, cThread *thread);
void StartPool(ConcurrencyThread *c);

int GetRandomID();
int CountRunningThreads(ConcurrencyThread *c);

// == [ cThread ] ==

cThread *StartThread(void *fn, void **args);
void ToggleThread(cThread *c);
void ToggleComplete(cThread *c);
void DestroyThread(cThread *thread);