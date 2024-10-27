#include "server.h"
#include "account.h"
#include <pthread.h>
#include <unistd.h>

// estrutura para a pool de threads
void* worker_function(void *arg) {
    Worker *worker = (Worker*) arg;
    while (1) {
        pthread_mutex_lock(&worker->lock);
        while (!worker->active) pthread_cond_wait(&worker->cond, &worker->lock);

        // TODO: talvez mudar as funcoes para PT/BR
        if (worker->request.type == DEPOSIT) { 
            deposit(worker->request.account_id, worker->request.amount);
        } else if (worker->request.type == TRANSFER) {
            transfer(worker->request.account_id, worker->request.to_account_id, worker->request.amount);
        } else if (worker->request.type==GENERAL_BALANCE) {
            print_balance();
        }

        worker->active = 0;
        pthread_cond_signal(&worker->cond);
        pthread_mutex_unlock(&worker->lock);
    }
}