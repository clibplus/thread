#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

#include "thread.h"

static Thread *THREAD_COPY;

Thread NewThread(void *Fnc, void **Args, void *Retvar, void *Siggy) {
	Thread t = {
		.ID 		= (rand() % (1000000 - 0 + 1)) + 1000000,
		.Args 		= Args,
		.ReturnVar	= Retvar,
		.Fn			= Fnc,
		.SigHandler = Siggy,
		.Running 	= 0,

		.Toggle 	= _Toggle,
		.SetArgs	= _SetArgs,
		.Execute	= _Execute,
		.Wait		= _Wait,
		.Exit		= _ExitT
	};

	t.ArgsCount = (!Args ? 0 : count_void_arr(Args));

	return t;
}

static void _Toggle(Thread *t) {
	t->Running = (t->Running ? 0 : 1);

	if(!t->Running) {
		_ExitT(t);
	}
}

static int _SetArgs(Thread *t, void **Args) {
	if(!t || t->Running == 1)
		return -1;

	long count = count_void_arr(Args);
	if(t->Args) {
		free(t->Args);
		t->Args = NULL;
	}

	t->Args = Args;
	t->ArgsCount = count;
	return 1;
}

static int _Execute(Thread *t) {
	if(!t)
		return -1;

	if(t->Running == 1)
		return -1;

    struct sigaction sa;
    sa.sa_handler = (!t->SigHandler ? _ThreadSignalHandler : t->SigHandler);
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    // Set up signal handlers for desired signals
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

	pthread_create(&t->TID, NULL, (void *)t->Fn, t->Args);
	t->Running = 1;

	return 1;
}

static int _Wait(Thread *t) {
	if(!t || t->Running != 1)
		return -1;

	(void)(!t->ReturnVar ? pthread_join(t->TID, NULL) : pthread_join(t->TID, t->ReturnVar));
	t->Running = 0;

	return 1;
}

static int _ExitT(Thread *t) {
	if(!t || t->Running != 1)
		return -1;

	pthread_cancel(t->TID);
	_Wait(t);
	return 1;
}

static int DestroyThread(Thread *t) {
	if(!t)
		return -1;

	if(!t->Args)
		return -1;

	for(int i = 0; i < t->ArgsCount; i++) {
		free(t->Args[i]);
		t->Args[i] = NULL;
	}

	free(t->Args);
	t->Args = NULL;

	return 1;
}

static void _ThreadSignalHandler(int signum) {


}

static long count_void_arr(void **Args) {
	long c = 0;
	while(Args[c] != NULL)
		c++;

	return c;
}
