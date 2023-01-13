#ifndef __CDEMON_GLOBAL_H
#define __CDEMON_GLOBAL_H 1

#include <stdbool.h>
#include <pthread.h>

static bool restart;
static pthread_mutex_t restart_lock;

extern char *command;
extern char **args;

#endif