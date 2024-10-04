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

	void		(*Toggle)		(struct Thread *t);
	int 		(*SetArgs)		(struct Thread *t, void **Args);
	int 		(*Execute)		(struct Thread *t);
	int  		(*Wait)			(struct Thread *t);
	int  		(*Exit)			(struct Thread *t);
} Thread;

/*
				| - > Create a new Thread instance
*/
Thread 			NewThread(void *Fnc, void **Args, void *Retvar);

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
