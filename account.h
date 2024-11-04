#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock; // Mutex para proteção de concorrência
} Account;

typedef struct {
    int operation; // 1: depósito, 2: transferência, 3: balanço
    int src_account;  // Conta de origem
    int dest_account; // Conta de destino
    double amount; // Montante para depósito ou transferência
} Request;

#endif // ACCOUNT_H

void init_accounts();
void deposit(int account_id, float amount, int thread_id);
void transfer(int src_account_id, int dest_account_id, float amount, int thread_id);
void balance_report(int thread_id);