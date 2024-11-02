#ifndef SERVER_H
#define SERVER_H

#include "account.h"

#define MAX_ACCOUNTS 10 // numero de contas
#define MAX_QUEUE_SIZE 20 // tamanho da fila de requisicoes
#define POOL_SIZE 4 // numero de threads
#define MAX_REQUESTS 50 // maximo de requisicoes

typedef struct {
    Request requests[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} RequestQueue;

extern Account accounts[];
extern RequestQueue queue;
extern int stop_server;

void enqueue(Request req);
Request dequeue();
void* worker_thread(void* arg);
void* server_thread_func(void* arg);

#endif // SERVER_H
