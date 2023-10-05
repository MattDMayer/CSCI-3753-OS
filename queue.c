#include "queue.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

int push(queue *q, char *entry)
{
    if((pthread_mutex_lock(&q->write) != 0) || sem_wait(&q->empty) == -1)
    {
        return -1;
    }
    q->tail = (q->tail + 1) % ARRAY_SIZE;
    q->insides[q->tail] = malloc(MAX_NAME_LENGTH);
    memcpy(q->insides[q->tail], entry, MAX_NAME_LENGTH);
    if((pthread_mutex_unlock(&q->write) != 0) || (sem_post(&q->full) == -1))
    {
        return -1;
    }
    return 0;
}

int pop(queue *q, char**popping)
{
    if((pthread_mutex_lock(&q->read) == -1) || (sem_wait(&q->full) == -1))
    {
        return -1;
    }
    memcpy(*popping, q->insides[q->head], MAX_NAME_LENGTH);
    free(q->insides[q->head]);
    q->insides[q->head] = NULL;
    q->head = (q->head + 1) % ARRAY_SIZE;
    if((pthread_mutex_unlock(&q->read) != 0) || (sem_post(&q->empty) == -1))
    {
        return -1;
    }
    return 0;
}

void initializeQueue(queue *q)
{
    q->head = 0;
    q->tail = -1;
    pthread_mutex_init(&q->read, NULL);
    pthread_mutex_init(&q->write, NULL);
    sem_init(&q->full, 0, 0);
    sem_init(&q->empty, 0, ARRAY_SIZE);
    return;
}

void dumpQueue(queue *q)
{
    for(int i = 0; i < ARRAY_SIZE; i++)
    {
        if(q->insides[i] != NULL)
        {
            free(q->insides[i]);
        }
    }
    pthread_mutex_destroy(&q->read);
    pthread_mutex_destroy(&q->write);
    sem_destroy(&q->full);
    sem_destroy(&q->empty);
}