/*
		Use Example

#include "Thread/thread.h"

void *Counter(void **x) {
	Thread *t = (Thread *)x[0];
	int len = atoi((char *)x[1]);
	for(int i = 0; i < len; i++) {
		printf("[ %d ] Thread: %d\n", t->ID, i);

		sleep(1);
	}

	t->Running = 0;
	return (int *)5;
}

int main() {
	int exit_code = 0;

	Thread t = NewThread(Counter, NULL, &exit_code, NULL);
	printf("[ + ] Create New Thread: %d\n", t.ID);

	t.SetArgs(&t, ((void *[]){(void *)&t, (void *)"10"}));
	printf("[ + ] Set Thread Function Arguments\n");


	t.Execute(&t);
	printf("[ + ] Executing Thread....!\n");

	t.Wait(&t);
	printf("[ + ] Waiting for thread to finish....\n");

	printf("Thread has finished with exit code %d...\n", exit_code);
	return 0;
}

*/
#pragma once

#include <pthread.h>

typedef struct Thread {
	int 		ID;
	pthread_t	TID;

	void 		**Args;
	int			ArgsCount;
	void 		*ReturnVar;
	int 		Running;
	void 		(*Fn)			(void **Args);
	void 		(*SigHandler)	();

	void		(*Toggle)		(struct Thread *t);
	int 		(*SetArgs)		(struct Thread *t, void **Args);
	int 		(*Execute)		(struct Thread *t);
	int  		(*Wait)			(struct Thread *t);
	int  		(*Exit)			(struct Thread *t);
} Thread;

/*
				| - > Create a new Thread instance
*/
Thread 			NewThread(void *Fnc, void **Args, void *Retvar, void *Siggy);

/*
				| - > Signal Off For Clean up
*/
static void 	_Toggle(Thread *t);

/*
				| - > Set arguments for the threaded function
*/
static int 		_SetArgs(Thread *t, void **Args);

/*
				| - > Start The Thread
*/
static int 		_Execute(Thread *t);

/*
				| - > Wait for thread to finish execution
*/
static int 		_Wait(Thread *t);

/*
				| - > Exit Thread
*/
static int 		_ExitT(Thread *t);

// == [ Utilities ] ==

static long 	count_void_arr(void **Args);
static int 		DestroyThread(Thread *t);
static void 	_ThreadSignalHandler(int signum);
