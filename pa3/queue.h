#ifndef QUEUE_H
#define QUEUE_H
#include <pthread.h>
#include <semaphore.h>

#define MAX_NAME_LENGTH 1025
#define ARRAY_SIZE 8

typedef struct
{
    int head;
    int tail;
    char *insides[ARRAY_SIZE];
    pthread_mutex_t read;
    pthread_mutex_t write;
    sem_t empty;
    sem_t full;
} queue;

int push(queue *q, char *entry);
int pop(queue *q, char **popping);
void initializeQueue(queue *q);
void dumpQueue(queue *q);
#endif