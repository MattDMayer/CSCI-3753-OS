#include "multi-lookup.h"
#include "queue.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    struct timeval start, end, difference;
    gettimeofday(&start, NULL);

    if(argc < 6)
    {
        fprintf(stderr, "Not enough inputs. Input should be: ./a.out, number of requester threads, number of resolver threads, requester log, resolver log, data files holding sites.\n");
        exit(-1);
    }
    if(argc > 5 + MAX_INPUT_FILES)
    {
        fprintf(stderr, "Too many inputs. Input should be: ./a.out, number of requester threads, number of resolver threads, requester log, resolver log, data files holding sites.\n");
        fprintf(stderr, "The max amount of input files allowed is: %d\n", MAX_INPUT_FILES);
        exit(-1);
    }

    int requesters = atoi(argv[1]);
    if(requesters <= 0 || requesters > MAX_REQUESTER_THREADS)
    {
        fprintf(stderr, "Invalid amount of requester threads. Must be a positive integer below %d.\n", MAX_REQUESTER_THREADS);
        exit(-1);
    }
    int resolvers = atoi(argv[2]);
    if(resolvers <= 0 || resolvers > MAX_RESOLVER_THREADS)
    {
        fprintf(stderr, "Invalid amount of resolver threads. Must be a positive integer below %d.\n", MAX_RESOLVER_THREADS);
        exit(-1);
    }
    
    FILE *requesterLog;
    requesterLog = fopen(argv[3], "w");
    if(errno != 0)
    {
        fprintf(stderr, "Failed to open the requester log.\n");
        exit(-1);
    }
    FILE *resolverLog;
    resolverLog = fopen (argv[4], "w");
    if(errno != 0)
    {
        fprintf(stderr, "Failed to open the resolver log.\n");
        exit(-1);
    }

    siteFileQueue siteFiles;
    int validFiles = 0;
    for(int i = 5; i < argc; i++)
    {
        if(access(argv[i], F_OK) == -1)
        {
            fprintf(stderr, "Invalid file %s\n", argv[i]);
        }
        else
        {
            siteFiles.siteInsides[validFiles] = strndup(argv[i], strlen(argv[i]));
            validFiles++;
        }
    }
    if(validFiles == 0)
    {
        fprintf(stderr, "No input files for site names were valid.\n");
        exit(-1);
    }

    queue *buffer = malloc(sizeof(queue));
    initializeQueue(buffer);

    pthread_mutex_t bufferMutex;
    pthread_mutex_t requesterLogMutex;
    pthread_mutex_t requestCountMutex;
    pthread_mutex_t resolverLogMutex;
    pthread_mutex_init(&bufferMutex, NULL);
    pthread_mutex_init(&requesterLogMutex, NULL);
    pthread_mutex_init(&requestCountMutex, NULL);
    pthread_mutex_init(&resolverLogMutex, NULL);

    requester request;
    request.buffer = buffer;
    request.siteFileQueue = siteFiles;
    request.numFiles = validFiles;
    request.requestedLog = requesterLog;
    request.requesterThreads = 0;
    request.bufferMutex = bufferMutex;
    request.logMutex = requesterLogMutex;
    request.countMutex = requestCountMutex;

    resolver resolve;
    resolve.buffer = buffer;
    resolve.resolvedLog = resolverLog;
    resolve.logMutex = resolverLogMutex;

    pthread_t requestThreads[requesters];
    pthread_t resolveThreads[resolvers];
    for(int i = 0; i < requesters; i++)
    {
        pthread_create(&requestThreads[i], NULL, requesting, &request);
    }
    for(int i = 0; i < resolvers; i++)
    {
        pthread_create(&resolveThreads[i], NULL, resolving, &resolve);
    }
    for(int i = 0; i < requesters; i++)
    {
        pthread_join(requestThreads[i], NULL);
    }
    for(int i = 0; i < resolvers; i++)
    {
        pthread_join(resolveThreads[i], NULL);
    }
    for(int i = 0; i < validFiles; i++)
    {
        free(siteFiles.siteInsides[i]);
    }
    fclose(requesterLog);
    fclose(resolverLog);
    dumpQueue(buffer);
    free(buffer);

    gettimeofday(&end, NULL);
    timersub(&end, &start, &difference);
    printf("./multi-lookup: total time is %lu.%06lu seconds\n", difference.tv_sec, difference.tv_usec);
    return 0;
}

void *requesting(void *requestPass)
{
    struct timeval start, end, difference;
    gettimeofday(&start, NULL);
    requester *request = requestPass;
    int filesHandled = 0;
    pthread_mutex_lock(&request->countMutex);
    request->requesterThreads = request->requesterThreads + 1;
    pthread_mutex_unlock(&request->countMutex);
    pthread_mutex_lock(&request->bufferMutex);
    FILE *file;
    char *input;
    char *line;
    int position = 0;
    while(request->numFiles > 0)
    {
        input = request->siteFileQueue.siteInsides[0];
        for(int i = 0; i < request->numFiles; i++)
        {
            if(i == request->numFiles - 1)
            {
                request->siteFileQueue.siteInsides[i] = NULL;
            }
            else
            {
                request->siteFileQueue.siteInsides[i] = request->siteFileQueue.siteInsides[i+1];
            }
        }
        request->numFiles --;
        pthread_mutex_unlock(&request->bufferMutex);
        filesHandled ++;

        file = fopen(input, "r");
        if(errno != 0)
        {
            fprintf(stderr, "File was garbage\n");
            exit(1);
        }
        size_t lineLength = 0;
        while((position = getline(&line, &lineLength, file)) != -1)
        {    
            memset(line + position - 1, 0, 1);
            push(request->buffer, line);
            pthread_mutex_lock(&request->logMutex);
            fprintf(request->requestedLog, "%s\n", line);
            pthread_mutex_unlock(&request->logMutex);
        }
        free(line);
        fclose(file);
    }
    pthread_mutex_unlock(&request->bufferMutex);
    pthread_mutex_lock(&request->countMutex);
    if(request->requesterThreads - 1 == 0)
    {
        push(request->buffer, "quit");
    }
    request->requesterThreads = request->requesterThreads - 1;
    pthread_mutex_unlock(&request->countMutex);
    gettimeofday(&end, NULL);
    timersub(&end, &start, &difference);
    printf("thread %ld serviced %d files in %lu.%06lu seconds\n", pthread_self(), filesHandled, difference.tv_sec, difference.tv_usec);
    return 0;
}

void *resolving(void *resolvePass)
{
    struct timeval start, end, difference;
    gettimeofday(&start, NULL);
    resolver *resolve = resolvePass;
    int filesHandled = 0;
    char *name = malloc(MAX_NAME_LENGTH);
    char *IP = malloc(MAX_IP_LENGTH);

    while(1)
    {
       pop(resolve->buffer, &name);
       if(strcmp(name, "quit") == 0)
       {
            push(resolve->buffer, "quit");
            gettimeofday(&end, NULL);
            timersub(&end, &start, &difference);
            printf("thread %ld serviced %d files in %lu.%06lu seconds\n", pthread_self(), filesHandled, difference.tv_sec, difference.tv_usec);
            free(name);
            free(IP);
            return 0;
       } 
       if(dnslookup(name, IP, MAX_IP_LENGTH))
       {
            memcpy(IP, "NOT_RESOLVED\0", 13);
       }
       else
       {
            filesHandled ++;
       }
       pthread_mutex_lock(&resolve->logMutex);
       fprintf(resolve->resolvedLog, "%s, %s\n", name, IP);
       pthread_mutex_unlock(&resolve->logMutex);
    }
}