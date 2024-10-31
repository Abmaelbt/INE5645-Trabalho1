#include "server.h"
#include "account.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

// estrutura para a pool de threads
void* worker_function(void *arg) {
    Worker *worker = (Worker*) arg; // recebe um ponteiro para a estrutura worker
    while (1) {
        pthread_mutex_lock(&worker->lock);
        while (!worker->active) pthread_cond_wait(&worker->cond, &worker->lock); // espera até ser ativado

        // chamar a funcao para deposito
        if (worker->request.type == DEPOSIT) {
            printf("Worker processando depósito.\n");
            deposit(worker->request.account_id, worker->request.amount);
        } else if (worker->request.type == TRANSFER) {
            printf("Worker processando transferência.\n");
            transfer(worker->request.account_id, worker->request.to_account_id, worker->request.amount);
        } else if (worker->request.type==GENERAL_BALANCE) {
            printf("Worker calculando balanço geral.\n");
            print_balance();
        }

        worker->active = 0; // Marca o worker como inativo após o processamento
        pthread_cond_signal(&worker->cond);
        pthread_mutex_unlock(&worker->lock);    
    }
}