#ifndef SERVER_H
#define SERVER_H

#include <pthread.h>

#define MAX_QUEUE_SIZE 9     // Tamanho da fila de requisições, sem duplicidade

typedef enum {
    DEPOSIT,
    TRANSFER,
    GENERAL_BALANCE
} RequestType;

typedef struct {
    RequestType type;
    int account_id;
    int to_account_id;
    float amount;
} Request;

typedef struct {
    pthread_t thread;
    Request request;
    int active;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} Worker;

void add_request(Request req);
void* server_function(void *arg);
void* worker_function(void *arg);

#endif
