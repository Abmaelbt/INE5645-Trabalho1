#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>

extern int MAX_QUEUE_SIZE;


typedef struct {
    int account_id;
    float balance;
    pthread_mutex_t lock;
} Account;

typedef struct {
    int operation; // 1: deposito, 2: transferencia, 3: saldo
    int src_account;
    int dest_account;
    float amount;
} Request;

#define MAX_ACCOUNTS 2

typedef struct {
    Request requests[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
    pthread_mutex_t lock;
    pthread_cond_t cond;
} RequestQueue;

void init_accounts(Account accounts[], int max_accounts);

#endif // ACCOUNT_H
