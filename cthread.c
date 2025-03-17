#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "currency.h"

cThread *StartThread(void *fn, void **args) {
    cThread *c = (cThread *)malloc(sizeof(cThread));
    *c = (cThread){
        .TID = (rand() % (10000 - 1 + 1)) + 1,
        .Handler = fn,
        .args = args,
        .Completed = 0,
        .Running = 0,
        .Destruct = DestroyThread
    };

    if(args != NULL)
        while(args[c->c] != NULL) c->c++;
    return c;
}

void ToggleThread(cThread *c) 
{ 
    c->Running = (c->Running ? 0 : 1); 
    (c->Running ? ((ConcurrencyThread *)c->Base)->RunningThreads++ : ((ConcurrencyThread *)c->Base)->RunningThreads--);
}

void ToggleComplete(cThread *c) {
    if(!c)
        return;

    c->Completed = 1;
}

void DestroyThread(cThread *thread) {
    free(thread);
}