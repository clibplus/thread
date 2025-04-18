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

    pthread_join(thread->id, NULL);
    for(int i = 0; i < c->ThreadCount; i++) {
        if(!c->Threads[i])
            break;

        if(c->Threads[i] == thread) {
            c->Threads[i]->Destruct(thread);
            c->Threads[i] = NULL;
            continue;
        }

        arr[idx] = c->Threads[i];
        idx++;
        arr = (cThread **)realloc(arr, sizeof(cThread *) * (idx + 1));
    }

    if(c->Threads)
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

void StartPool(ConcurrencyThread *c) {
    c->PoolRunning = 1;
    while(1) {
        for(int i = 0; i < c->ThreadCount; i++) {
            if(!c->Threads[i])
                break;

            if(c->RunningThreads >= c->MAX_THREADS) {
                usleep(30000);
                continue;
            }

            if(c->Threads[i]->Running && !c->Threads[i]->Completed)
                continue;

            if(c->Threads[i]->Completed) {
                ToggleThread(c->Threads[i]);
                RemoveThread(c, c->Threads[i]);
                break;
            }

            ToggleThread(c->Threads[i]);
            printf("[ THREADPOOL ] Running %d...\n", c->Threads[i]->TID);
            pthread_create(&c->Threads[i]->id, NULL, (void *)c->Threads[i]->Handler, c->Threads[i]);
        }

        if(c->RunningThreads == 0)
            break;

        usleep(30000);
        printf("[ THREADPOOL ] Running %d threads...\n", c->RunningThreads);
    }

    printf("[ THREADPOOL ] Exiting....!\n");
    c->PoolRunning = 0;
}