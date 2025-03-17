/*
*
*   [ Concurrency Thread Pool ]
*
* @title: Concurrency ThreadPool
* @author: Algo1337
* @since: 12/6/2024
* @github: Algo1337
*
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

#include "currency.h"


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

int AppendThread(ConcurrencyThread *c, cThread *thread) {
    if(!c)
        return 0;


    c->Threads[c->ThreadCount] = thread;
    ((cThread *)c->Threads[c->ThreadCount])->Base = c;

    c->ThreadCount++;
    c->Threads = (cThread **)realloc(c->Threads, sizeof(cThread *) * (c->ThreadCount + 1));

    return 1;
}

int RemoveThread(ConcurrencyThread *c, cThread *thread) {
    cThread **arr = (cThread **)malloc(sizeof(cThread *) * 1);
    int idx = 0;

    for(int i = 0; i < c->ThreadCount; i++) {
        if(!c->Threads[i])
            break;

        if(c->Threads[i] == thread) {
            c->Threads[i]->Destruct(thread);
            c->Threads[i] = NULL;
        }

        arr[idx] = c->Threads[i];
        idx++;
        arr = (cThread **)realloc(arr, sizeof(cThread *) * (idx + 1));
    }

    free(c->Threads);
    c->Threads = arr;
    c->ThreadCount = idx;

    return 1;
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

void StartPool(ConcurrencyThread *c) {
    while(1) {
        for(int i = 0; i < c->ThreadCount; i++) {
            if(c->RunningThreads >= c->MAX_THREADS) {
                sleep(1);
                continue;
            }

            if(c->Threads[i]->Completed) {
                RemoveThread(c, c->Threads[i]);
            }

            if(c->Threads[i]->Running) {
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

// void Counter(void *arg) {
//     cThread *c = (cThread *)arg;
//     printf("Starting Thread: %d = > Running State: %d\n", c->TID, c->Running);

//     for(int i = 0; i < (int)c->args[1]; i++)
//         sleep(1);

//     printf("[%d] Completed Thread: %d:%d...!\n", (int)c->args[0], c->TID, c->Running);
//     ToggleThread(c);
//     pthread_exit(NULL);
// }