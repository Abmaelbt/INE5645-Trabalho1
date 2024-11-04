#include "server.h"
#include "account.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>



// Função para enfileirar uma requisição
void enqueue(Request req) {
    pthread_mutex_lock(&queue.lock);
    while (queue.size == MAX_QUEUE_SIZE) {
        pthread_cond_wait(&queue.cond, &queue.lock);
    }
    queue.requests[queue.rear] = req;
    queue.rear = (queue.rear + 1) % MAX_QUEUE_SIZE;
    queue.size++;
    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.lock);
}

// Função para desenfileirar uma requisição
Request dequeue() {
    Request req;
    pthread_mutex_lock(&queue.lock);
    while (queue.size == 0 && !stop_server) {
        pthread_cond_wait(&queue.cond, &queue.lock);
    }
    if (stop_server) {
        pthread_mutex_unlock(&queue.lock);
        exit(0);
    }
    req = queue.requests[queue.front];
    queue.front = (queue.front + 1) % MAX_QUEUE_SIZE;
    queue.size--;
    pthread_cond_signal(&queue.cond);
    pthread_mutex_unlock(&queue.lock);
    return req;
}

// Função da thread do servidor
void* server_thread_func(void* arg) {
    printf("Thread do servidor iniciada.\n");
    srand(time(NULL));
    for (int i = 0; i < MAX_REQUESTS; i++) { 
        Request req;
        req.operation = rand() % 2 + 1; // Seleciona aleatoriamente depósito (1) ou transferência (2)
        req.src_account = rand() % MAX_ACCOUNTS + 1;

        if (req.operation == 1) { // Depósito
            req.amount = (rand() % 200) - 100; // Quantia aleatória entre -100 e +100
        } else { // Transferência
            req.amount = rand() % 100 + 1; // Quantia aleatória entre 1 e 100
            req.dest_account = rand() % MAX_ACCOUNTS + 1;
            while (req.src_account == req.dest_account) { // Garante contas diferentes
                req.dest_account = rand() % MAX_ACCOUNTS + 1;
            }
        }
        enqueue(req);

        if (i % 10 == 0) { // A cada 10 requisições, enfileira um relatório de balanço
            Request balance_req;
            balance_req.operation = 3;
            balance_req.src_account = -1;
            balance_req.dest_account = -1;
            balance_req.amount = 0;
            enqueue(balance_req);
        }

        usleep(100000); // Pausa para controlar a taxa de requisições em milisegundos
    }
    stop_server = 1; // Sinaliza para as threads pararem
    for (int i = 0; i < POOL_SIZE; i++) {
        enqueue((Request){.operation = 0}); // Enfileira sinal de parada
    }
    printf("Thread do servidor encerrando.\n");
    return NULL;
}
