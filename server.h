#ifndef SERVER_H
#define SERVER_H

#define MAX_ACCOUNTS 10
#define MAX_WORKERS 4
#define MAX_QUEUE_SIZE 20
#include <pthread.h>

typedef struct {
    int id;
    float balance;
} Account;

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

void create_account(int id, float balance);
void deposit(int id, float amount);
void transfer(int from_id, int to_id, float amount);
void print_balance();
void add_request(Request req)
void* server_function(void *arg);
void* worker_function(void *arg);

#endif