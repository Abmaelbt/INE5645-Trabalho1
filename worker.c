#include "server.h"
#include "account.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Função das threads trabalhadoras
void* worker_thread(void* arg) { // No longer static
    int thread_id = *((int*)arg);
    printf("Thread trabalhadora %d iniciada.\n", thread_id);
    while (!stop_server) {
        Request req = dequeue();
        if (req.operation == 1) { // Depósito
            pthread_mutex_lock(&accounts[req.src_account - 1].lock);
            accounts[req.src_account - 1].balance += req.amount;
            printf("Thread %d: Depositou %.2f na conta %d. Novo saldo: %.2f\n", 
                   thread_id, req.amount, req.src_account, accounts[req.src_account - 1].balance);
            pthread_mutex_unlock(&accounts[req.src_account - 1].lock);
            usleep(500000); // Simula tempo de processamento
        } else if (req.operation == 2) { // Transferência
            pthread_mutex_lock(&accounts[req.src_account - 1].lock);
            pthread_mutex_lock(&accounts[req.dest_account - 1].lock);
            if (accounts[req.src_account - 1].balance >= req.amount) {
                accounts[req.src_account - 1].balance -= req.amount;
                accounts[req.dest_account - 1].balance += req.amount;
                printf("Thread %d: Transferiu %.2f da conta %d para %d. Novos saldos: %.2f, %.2f\n",
                       thread_id, req.amount, req.src_account, req.dest_account,
                       accounts[req.src_account - 1].balance, accounts[req.dest_account - 1].balance);
            } else {
                printf("Thread %d: Transferência falhou da conta %d: Saldo insuficiente.\n", 
                       thread_id, req.src_account);
            }
            pthread_mutex_unlock(&accounts[req.dest_account - 1].lock);
            pthread_mutex_unlock(&accounts[req.src_account - 1].lock);
            usleep(500000); // Simula tempo de processamento
        } else if (req.operation == 3) { // Balanço
            printf("Thread %d: Relatório de balanço:\n", thread_id);
            for (int i = 0; i < MAX_ACCOUNTS; i++) {
                pthread_mutex_lock(&accounts[i].lock);
                printf("Thread %d: Conta %d: %.2f\n", thread_id, accounts[i].account_id, accounts[i].balance);
                pthread_mutex_unlock(&accounts[i].lock);
            }
            usleep(500000); // Simula tempo de processamento
        }
    }
    printf("Thread trabalhadora %d encerrando.\n", thread_id);
    return NULL;
}
