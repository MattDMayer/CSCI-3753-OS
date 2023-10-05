#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include "queue.h"
#include <stdio.h>
#include <pthread.h>

#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

typedef struct siteFileQueue
{
    char *siteInsides[MAX_INPUT_FILES];
}siteFileQueue;

typedef struct
{
    siteFileQueue siteFileQueue;
    int numFiles;
    queue *buffer;
    pthread_mutex_t bufferMutex;
    FILE *requestedLog;
    pthread_mutex_t logMutex;
    int *requesterThreads;
    pthread_mutex_t countMutex;
}requester;

typedef struct
{
    queue *buffer;
    FILE *resolvedLog;
    pthread_mutex_t logMutex;
}resolver;

void *requesting(void *inputRequester);
void *resolving(void *inputResolver);
#endif