#include "account.h"
#include <stdio.h>
#include <pthread.h>
#include "server.h"
#include <unistd.h>

// Inicializa as contas
void init_accounts() {
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        accounts[i].account_id = i + 1;
        accounts[i].balance = 1000.0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

// Função de depósito
void deposit(int account_id, float amount, int thread_id) {
    pthread_mutex_lock(&accounts[account_id - 1].lock);
    accounts[account_id - 1].balance += amount;
    printf("Thread %d: Depositou %.2f na conta %d. Novo saldo: %.2f\n", 
           thread_id, amount, account_id, accounts[account_id - 1].balance);
    pthread_mutex_unlock(&accounts[account_id - 1].lock);
    sleep(1); // Simula tempo de processamento 
}

// Função de transferência
void transfer(int src_account_id, int dest_account_id, float amount, int thread_id) {
    pthread_mutex_lock(&accounts[src_account_id - 1].lock);
    pthread_mutex_lock(&accounts[dest_account_id - 1].lock);
    if (accounts[src_account_id - 1].balance >= amount) {
        accounts[src_account_id - 1].balance -= amount;
        accounts[dest_account_id - 1].balance += amount;
        printf("Thread %d: Transferiu %.2f da conta %d para %d. Novos saldos: %.2f, %.2f\n",
               thread_id, amount, src_account_id, dest_account_id,
               accounts[src_account_id - 1].balance, accounts[dest_account_id - 1].balance);
    } else {
        printf("Thread %d: Transferência falhou da conta %d: Saldo insuficiente.\n", 
               thread_id, src_account_id);
    }
    pthread_mutex_unlock(&accounts[dest_account_id - 1].lock);
    pthread_mutex_unlock(&accounts[src_account_id - 1].lock);
    sleep(1);
}

// Função de balanço
void balance_report(int thread_id) {
    printf("Thread %d: Relatório de balanço:\n", thread_id);
    for (int i = 0; i < MAX_ACCOUNTS; i++) {
        pthread_mutex_lock(&accounts[i].lock);
        printf("Thread %d: Conta %d: %.2f\n", thread_id, accounts[i].account_id, accounts[i].balance);
        pthread_mutex_unlock(&accounts[i].lock);
    }
    sleep(1);
}
