#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include "account.h"
#include "server.h"
#include <stdint.h>

int NUM_CLIENTS = 5;            // Número de threads clientes
int MAX_REQUESTS = 30;          // Número máximo de requisições. Criterio de parada.
int POOL_SIZE = 10;             // Numero de threads workers no pool
int CLIENT_SLEEP_TIME = 500000; // Taxa de geração de novas requisições em milisegundos
int server_running = 1; // variavel para informar se o servidor ta rodando

// Função de cada cliente que gera requisições aleatórias
void* client_function(void *arg) {
    srand(time(NULL) + (intptr_t)arg); // Seed para números aleatórios
    int request_count = 0;
    while (request_count < MAX_REQUESTS && server_running) {
        Request req;
        int rand_op = rand() % 2;

        if (rand_op == 0) { // Depósito
            req.type = DEPOSIT;
            req.account_id = rand() % MAX_ACCOUNTS + 1;

            //req.amount = (rand() % 1000) + 1; Talvez excluir
            if (rand() % 2 == 0) {
                req.amount = (rand() % 1000) + 1; // Depósito 
            } else {
                req.amount = -((rand() % 500) + 1); // Saque 
            }
        } else { // Transferência
            req.type = TRANSFER;
            req.account_id = rand() % MAX_ACCOUNTS + 1;
            do {
                req.to_account_id = rand() % MAX_ACCOUNTS + 1;
            } while (req.to_account_id == req.account_id);
            req.amount = (rand() % 500) + 1;
        }

        add_request(req); // Adiciona a requisição na fila
        usleep(CLIENT_SLEEP_TIME); // Espera entre requisições
        request_count++;
    }
    return NULL;
}

int main() {
    // contas com base no valor de MAX_ACCOUNTS
    for (int i = 1; i <= MAX_ACCOUNTS; i++) {
        create_account(i, 1000.0 * i); // Exemplo de saldo inicial que varia por conta
    }

    pthread_t server_thread;
    Worker workers[POOL_SIZE];
    for (int i = 0; i < POOL_SIZE; i++) {
        workers[i].active = 0;
        pthread_mutex_init(&workers[i].lock, NULL);
        pthread_cond_init(&workers[i].cond, NULL);
        pthread_create(&workers[i].thread, NULL, worker_function, &workers[i]);
    }

    pthread_create(&server_thread, NULL, server_function, (void*)&(struct { Worker* workers; int pool_size; }) { workers, POOL_SIZE });

    // Inicia as threads clientes
    pthread_t clients[NUM_CLIENTS];
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_create(&clients[i], NULL, client_function, (void *)(intptr_t)i);
    }
    pthread_join(server_thread, NULL);
    for (int i = 0; i < POOL_SIZE; i++) {
        pthread_join(workers[i].thread, NULL);
    }
    for (int i = 0; i < NUM_CLIENTS; i++) {
        pthread_join(clients[i], NULL);
    }
    return 0;
}